#ifndef USER_HPP
#define USER_HPP

#include <fstream>
#include <map>
#include <tuple>
#include <vector>

// Define a User class for storing and retreiving information for each user
class User
{
  private:
    // Unique user id for this user.
    unsigned int uid;

    // Average rating, and maximum number of correlations and
    // recommendations for this user.
    double avgrating;
    unsigned int ncorrmax;
    unsigned int nrecmax;

    // Ratings, correlations, and recommendations for this user.
    std::map<unsigned int,unsigned int> ratings;
    std::vector<std::tuple<unsigned int,double,unsigned int>> correlations;
    std::vector<std::tuple<unsigned int,double>> recommendations;
  public:
    // Constructor
    User(unsigned int uid);

    // Add a product id / rating pair to the ratings for this user.
    void AddRating(unsigned int pid, unsigned int rating);

    // Compute the avgrating for current user
    void CmpAvgRating(void);

    // Get the number of ratings of current user
    unsigned int GetNumberRatings(void);

    // Set the ncorrmax for current user
    void SetNcorrmax(unsigned int ncorrmax);

    // Set the nrecmax for current user
    void SetNrecmax(unsigned int nrecmax);

    // Compute the PCC correlation between this user and user u,
    // and store it for each user as appropriate (i.e. either user
    // does not yet have ncorrmax correlations, or if they do this
    // one is better than an existing correlation.)
    void ComputePCCCorrelation(User &u, unsigned int threshold);

    // Write the correlations for this user to an open output stream.
    void WriteCorrelations(std::ofstream &f);

    // Compute up to the nrecmax best recommendations for this user.
    void ComputeRecommendations(std::map<unsigned int,User> &users);

    // Write the recommendations for this user to an open output stream.
    void WriteRecommendations(std::ofstream &f);
};

#endif /* USER_HPP */
