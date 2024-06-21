#pragma once
// This file captures the file paths of the promela files that are being tested.

#include <string>

class TestFilesUtils {
public:
  explicit TestFilesUtils(const std::string & current_path) : current_path(current_path) {}

  std::string array_model() { return current_path + "/test_files/basic/array.pml"; }
  std::string flows_model() { return current_path + "/test_files/basic/flows.pml"; }

  std::string array_model_original() { return current_path + "/test_files/mutants/array.pml"; }
  std::string array_model_mutant() { return current_path + "/test_files/mutants/array_mutant.pml"; }
  std::string array_model_mutant_alt() { return current_path + "/test_files/mutants/array_mutant_alt.pml"; }

  std::string flows_model_original() { return current_path + "/test_files/mutants/flows/flows.pml"; }
  std::string flows_model_mutant() { return current_path + "/test_files/mutants/flows/flows_mutant.pml"; }

  std::string minepump_model_original() { return current_path + "/test_files/mutants/minepump/minepump.pml"; }
  std::string minepump_model_mutant() { return current_path + "/test_files/mutants/minepump/minepump_mutant.pml"; }

  std::string structure_model_original() { return current_path + "/test_files/mutants/structure/structure.pml"; }
  std::string structure_model_mutant() { return current_path + "/test_files/mutants/structure/structure_mutant.pml"; }

  std::string trafficLight_model_original() { return current_path + "/test_files/mutants/trafficLight/trafficlight.pml"; }
  std::string two_trafficLight_model_original()
  {
    return current_path + "/test_files/mutants/trafficLight/two_trafficlight.pml";
  }
  std::string trafficLight_model_mutant() { return current_path + "/test_files/mutants/trafficLight/trafficlight_mutant.pml"; }
  std::string trafficLight_model_mutant_alt()
  {
    return current_path + "/test_files/mutants/trafficLight/trafficlight_mutant_alt.pml";
  }

  std::string process_model_original() { return current_path + "/test_files/mutants/3Process/threeProcess.pml"; }
  std::string process_model_mutant() { return current_path + "/test_files/mutants/3Process/threeProcess_mutant.pml"; }

  std::string array_model_never() { return current_path + "/test_files/mutants/array_never.pml"; };
  std::string array_mutant_never() { return current_path + "/test_files/mutants/array_mutant_never.pml"; }

  std::string dijkstra_original() { return current_path + "/test_files/mutants/dijkstra/original.pml"; }
  std::string mutex_original() { return current_path + "/test_files/mutants/mutex/mutex.pml"; }
  std::string peterson_original() { return current_path + "/test_files/mutants/peterson/original.pml"; }
  std::string leader_election_original() { return current_path + "/test_files/mutants/leader_election/original.pml"; }

private:
  const std::string current_path;
};