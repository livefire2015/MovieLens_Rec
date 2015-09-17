#include <fstream>

#include "user.hpp"
#include "moviewatchers.hpp"
#include "timer.h"

double MovieWatchers::ReadMovieLensData(std::string filename)
{  
    double time_start = timer();
    std::ifstream f(filename);
    unsigned int userid, movieid, rating, time_stamp;

    // Read the movielens data and populate each user's ratings
    while(f >> userid >> movieid >> rating >> time_stamp) {
          auto match = users.find(userid);
          if (match == users.end())       // if this is a new user
              users.insert(std::pair<unsigned int,User>(userid,User(userid)));
          users.at(userid).AddRating(movieid, rating);
    }
    f.close();

    // Compute each user's average rating for future use.
    for(auto &user : users)
        user.second.CmpAvgRating();
    double time_end = timer();
    return time_end - time_start;
}

unsigned int MovieWatchers::GetNumberRatings(void)
{
    unsigned int totalNumberRatings = 0;
    for(auto &user : users)
        totalNumberRatings +=user.second.GetNumberRatings();
    return totalNumberRatings;
}

unsigned int MovieWatchers::GetNumberUsers(void)
{
    return users.size();
}

double MovieWatchers::ComputePCCCorrelations(unsigned int threshold, unsigned int ncorrmax)
{
    double time_start = timer();

    // Loop over one user and all the others to compute PCC for each user
    for(auto &user1 : users) 
        for(auto &user2 : users){
            if(user1.first == user2.first)
               continue;
            user1.second.SetNcorrmax(ncorrmax);
            user2.second.SetNcorrmax(ncorrmax);
            user1.second.ComputePCCCorrelation(user2.second, threshold);
    }
    double time_end = timer();
    return time_end - time_start;   
}

void MovieWatchers::SaveCorrelations(std::string filename)
{
     std::ofstream f(filename);
     f.setf(std::ios::fixed, std::ios::floatfield);
     f.setf(std::ios::showpoint);
     f.precision(2);
     for(auto &user : users)
         user.second.WriteCorrelations(f);
     f.close();
}

double MovieWatchers::ComputeRecommendations(unsigned int nrecmax)
{
    double time_start = timer();

    // Loop over all users to compute recommendations for each one
    for(auto &user : users) {
            user.second.SetNrecmax(nrecmax);
            user.second.ComputeRecommendations(users);
    }
    double time_end = timer();
    return time_end - time_start;   
}
    
void MovieWatchers::SaveRecommendations(std::string filename)
{
     std::ofstream f(filename);
     f.setf(std::ios::fixed, std::ios::floatfield);
     f.setf(std::ios::showpoint);
     f.precision(1);
     for(auto &user : users)
         user.second.WriteRecommendations(f);
     f.close();
}
