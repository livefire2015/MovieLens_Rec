#include <iostream>
#include <string>

#include "moviewatchers.hpp"

int main(int argc, char *argv[])
{
  // Gather command line arguments
  if (argc != 4)
  {
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << argv[0] << " <input file> <corr file> <rec file>" << std::endl;
    return 0;
  }
  std::string inputfile = argv[1];
  std::string corrfile = argv[2];
  std::string recfile = argv[3];

  // Create instance of MovieWatchers and load a MovieLens dataset.
  MovieWatchers mwatchers;
  double time = mwatchers.ReadMovieLensData(inputfile);
  unsigned int nratings = mwatchers.GetNumberRatings();
  unsigned int nusers = mwatchers.GetNumberUsers();
  std::cout << "Read " << nratings << " ratings from " << nusers;
  std::cout << " users in " << time << " seconds." << std::endl;

  // Compute correlations between users using the Pearson Correlation Coefficient (PCC)
  // Users must have at least 6 movies in common, and only the 25 best will be kept.
  unsigned int threshold = 6;
  unsigned int ncorrmax = 25;
  time = mwatchers.ComputePCCCorrelations(threshold, ncorrmax);
  std::cout << "Computed correlations in " << time << " seconds." << std::endl;
  mwatchers.SaveCorrelations(corrfile);

  // Compute movie recommendations for each user. Only the 3 recommendations with the
  // highest predicted rating will be kept.
  unsigned int nrecmax = 3;
  time = mwatchers.ComputeRecommendations(nrecmax);
  std::cout << "Computed recommendations in " << time << " seconds." << std::endl;
  mwatchers.SaveRecommendations(recfile);

  return 0;
}
