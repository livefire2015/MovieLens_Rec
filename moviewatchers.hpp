#ifndef MOVIEWATCHERS_HPP
#define MOVIEWATCHERS_HPP

#include <map>
#include <string>
#include "user.hpp"

// Define a MovieWatchers class for all users
class MovieWatchers
{
  private:
    // Map of users keyed by their user id.
    std::map<unsigned int,User> users;
  public:
    // Read a MovieLens data file and store the data, returns the number of
    // seconds to read the data.
    double ReadMovieLensData(std::string filename);

    // Get information about the the total number of users and the total
    // number of movie ratings by all users.
    unsigned int GetNumberRatings(void);
    unsigned int GetNumberUsers(void);

    // Computes the Pearson Correlation Coefficient (PCC) for all users, using
    // the criteria that they should have ranked at least threshold movies in
    // in common and a maximum of ncorrmax best correlations will be stored.
    // Returns the number of seconds to compute the correlations.
    double ComputePCCCorrelations(unsigned int threshold, unsigned int ncorrmax);

    // Save the correlations for all users to a text file.
    void SaveCorrelations(std::string filename);

    // Computes at most nrecmax recommendations for each user.  Returns the
    // number of seconds to compute the recommendations.
    double ComputeRecommendations(unsigned int nrecmax);

    // Saves the recommendations for all users to a text file.
    void SaveRecommendations(std::string filename);
};

#endif /* MOVIEWATCHERS_HPP */
