#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
//
#include <lyrahgames/pareto/gallery/gallery.hpp>
#include <lyrahgames/pareto/pareto.hpp>

using namespace std;
using namespace lyrahgames;

using real = float;

int main(int argc, char* argv[]) {
  // Check command-line arguments.
  if (argc != 2) {
    cerr << "Error: No output file given.\n";
    cout << "Usage:\n" << argv[0] << " <output file path>\n";
    return -1;
  }

  // Try to open file for output.
  fstream file(argv[1], ios::out);
  if (!file) {
    cerr << "Error: Failed to open given file '" << argv[1] << "'.\n";
    return -1;
  }

  // Random number generator needed for Pareto optimization.
  mt19937 rng{random_device{}()};

  // The problem is defined by an external macro.
  const auto problem = PARETO_PROBLEM;

  // Start time measurement.
  using clock = chrono::high_resolution_clock;
  const auto start = clock::now();

  // Estimate the Pareto frontier in one step.
  const auto pareto_front = pareto::nsga2::optimization<pareto::frontier<real>>(
      problem, rng, {.iterations = 1000, .population = 1000});

  // End time measurment.
  const auto end = clock::now();
  const auto seconds = chrono::duration<double>(end - start).count();
  cout << setw(20) << "time = " << setw(20) << seconds << " s\n";

  // Output Pareto frontier to CSV file.
  // Comments are introduced for number of objectives and parameters.
  // Every sample is put on a new line.
  // First, objectives are printed and then configurations.
  file << "# " << setw(20) << "objectives count = " << setw(10)
       << problem.objective_count() << '\n'
       << "# " << setw(20) << "parameters count = " << setw(10)
       << problem.parameter_count() << '\n';
  for (size_t i = 0; i < pareto_front.sample_count(); ++i) {
    for (auto y : pareto_front.objectives(i))
      file << setw(20) << y << ' ';
    for (auto x : pareto_front.parameters(i))
      file << setw(20) << x << ' ';
    file << '\n';
  }
}
