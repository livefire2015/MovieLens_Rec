#include <cmath>
#include <algorithm>

#include "user.hpp"

// Comparator function for descending sort of tuples by absolute value of second element
bool SortCorrelationTuples(const std::tuple<unsigned int,double,unsigned int> &t0,
                           const std::tuple<unsigned int,double,unsigned int> &t1)
{
  return fabs(std::get<1>(t0)) > fabs(std::get<1>(t1));
}

// Comparator function for descending sort of tuples by real value of second element
bool SortRecTuples(const std::tuple<unsigned int,double> &t0,
                   const std::tuple<unsigned int,double> &t1)
{
  return std::get<1>(t0) > std::get<1>(t1);
}

// Check whether the given movie is already a recommendation for the current user or not. Do this to avoid repeated recommendation. 
bool IsNewRecommendation(std::vector<std::tuple<unsigned int,double>> &recommendations, unsigned int movieid)
{ 
    bool newRec = true;
    if(!recommendations.empty()) {
         for(auto &rec : recommendations)
              if(movieid == std::get<0>(rec)){
                  newRec = false;
                  break;
              }
    }
    return newRec;
}

// Constructor for current user
User::User(unsigned int uid)
{
    this->uid = uid;
}

// Add a new pair (productid, rating) to the ratings of current user
void User::AddRating(unsigned int pid, unsigned int rating)
{
     ratings[pid] = rating;    
}

// Compute the avgrating for current user
void User::CmpAvgRating(void)
{
       double sum = 0.0;
       for(auto &iter : ratings)
           sum +=iter.second;
       avgrating = sum/ratings.size();
}

// Get the number of ratings of current user
unsigned int User::GetNumberRatings(void)
{
     return ratings.size();
}

// Set the ncorrmax for current user
void User::SetNcorrmax(unsigned int ncorrmax)
{
     this->ncorrmax = ncorrmax;
}

// Set the nrecmax for current user
void User::SetNrecmax(unsigned int nrecmax)
{
     this->nrecmax = nrecmax;
}

// Compute the PCC for current user and a given user
void User::ComputePCCCorrelation(User &u, unsigned int threshold)
{
     double num     = 0.; // numerator
     double den1    = 0.; // denominator term for user1
     double den2    = 0.; // denominator term for user2
     double avg1    = avgrating;
     double avg2    = u.avgrating;
     double pcc;
     unsigned int ncommon    = 0;

     for(auto &iter1 : ratings) {
           auto iter2 = u.ratings.find(iter1.first);
           if(iter2 != u.ratings.end()) { // find a common movie
              num += (iter1.second - avg1)*(iter2->second - avg2);
              den1 += pow(iter1.second - avg1, 2);
              den2 += pow(iter2->second - avg2, 2);
              ncommon +=1;
           }
     }
     pcc = num/sqrt(den1*den2 + 1.e-6);
     
     // If less than ncorrmax, put in the pcc calculated above     
     if(ncommon >= threshold and correlations.size() < ncorrmax){
        correlations.emplace_back(u.uid, pcc, ncommon);
        if(correlations.size() == ncorrmax) // sort all correlations for current user once ncorrmax is reached
          std::sort(correlations.begin(), correlations.end(), SortCorrelationTuples);
     }
     // If the pcc calculated above is better than the last candidate, replace
     // the last candidate with it and sort again 
     else if(ncommon >= threshold and fabs(pcc) > fabs(std::get<1>(correlations.back()))) {
          correlations.pop_back();
          correlations.emplace_back(u.uid, pcc, ncommon);
          std::sort(correlations.begin(), correlations.end(), SortCorrelationTuples);
     }
     if(correlations.size() < ncorrmax)
          std::sort(correlations.begin(), correlations.end(), SortCorrelationTuples);
}

// Write all correlations of current user to an open ofstream
void User::WriteCorrelations(std::ofstream &f)
{
     f << uid;
     for(auto &iter : correlations)
         f << " (" << std::get<0>(iter) << "," << std::get<1>(iter) << "," << std::get<2>(iter) << ")";
     f << std::endl; 
}

// Compute up to nrecmax recommendations for current user 
void User::ComputeRecommendations(std::map<unsigned int, User> &users)
{
     // Loop over all correlated users
     for(auto &corruser1 : correlations) {
         auto &u1 = users.at(std::get<0>(corruser1));
         // Loop over the ratings of one of the correlated users
         for(auto &u1rating : u1.ratings) {
             auto matchthisuser = ratings.find(u1rating.first);
             // If current user has not rated this movie, it is a recommendation
             // candidate
             if(matchthisuser == ratings.end()) {
                // If this recommendation already exists, continue searching
                if(!IsNewRecommendation(recommendations, u1rating.first))
                   continue;
                // Initialize the numerator and denomenator in the predicated
                // rating equation
                double num = (u1rating.second - u1.avgrating) * std::get<1>(corruser1);
                double denom = fabs(std::get<1>(corruser1));
                // Loop over other correlated users to check whether more users
                // rated this recommendation candidate
                for(auto &corruser2 : correlations) {
                    if(std::get<0>(corruser2) == std::get<0>(corruser1))
                       continue;
                    auto &u2 = users.at(std::get<0>(corruser2));
                    auto matchotheruser = u2.ratings.find(u1rating.first);
                    // If found one, add to the numerator and denomenator
                    if(matchotheruser != u2.ratings.end()){
                       num += (matchotheruser->second - u2.avgrating) * std::get<1>(corruser2);
                       denom += fabs(std::get<1>(corruser2));
                    }
                }
                double predrating = avgrating + num/denom;

                // Similar way of adding this recommendation to the list as
                // adding correlations 
                if(recommendations.size() < nrecmax) {
                   recommendations.emplace_back(u1rating.first, predrating);
                   if(recommendations.size() == nrecmax)
                      std::sort(recommendations.begin(), recommendations.end(), SortRecTuples);
                }
                else if(predrating > std::get<1>(recommendations.back())){
                     recommendations.pop_back();
                     recommendations.emplace_back(u1rating.first, predrating);
                     std::sort(recommendations.begin(), recommendations.end(), SortRecTuples);
                }
                if(recommendations.size() < nrecmax)
                     std::sort(recommendations.begin(), recommendations.end(), SortRecTuples);
             }
         }
     }
}

// Write this recommendation to an open ofstream. Note that the predicated
// rating has been clamped to be within 1 and 5.
void User::WriteRecommendations(std::ofstream &f)
{
     f << uid;
     for(auto &iter : recommendations){
         double recrating;
         if(std::get<1>(iter) < 1)
            recrating = 1;
         else if(std::get<1>(iter) > 5)
            recrating = 5;
         else
            recrating = std::get<1>(iter);
         f << " (" << std::get<0>(iter) << "," << recrating << ")"; 
     } 
     f << std::endl;
}
