#include "ltl.hpp"
#include <filesystem>
#include <iostream> // Include the <iostream> header
#include <fstream>
#include <stdexcept>
#include <string>

std::string LTLClaimsProcessor::transformLTLStringToNeverClaim(const std::string &ltl) {
    namespace fs = std::filesystem;
    // Use filesystem to build paths
    fs::path currentPath = fs::current_path();
    fs::path ltl2baPath = currentPath / "../src/bin/ltl2ba";

    // Check that the ltl2ba binary exists
    if (!fs::exists(ltl2baPath)) {
        throw std::runtime_error("Could not find the ltl2ba binary at " + ltl2baPath.string());
    }

    // Create a temporary file path
    fs::path formulaPath = currentPath / "__formula.tmp";
    // Negate the LTL formula
    std::string negatedLTL = "!(" + ltl + ")";

    // Call LTL2BA to transform the LTL formula into a never claim
    std::string command = ltl2baPath.string() + " -f \"" + negatedLTL + "\" > " + formulaPath.string();
    int result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Could not transform the LTL formula into a never claim using ltl2ba.");
    }

    // Read the never claim from the temporary file
    std::ifstream formulaFile(formulaPath);
    if (!formulaFile.is_open()) {
        throw std::runtime_error("Could not open the never claim file at " + formulaPath.string());
    }
    // Read the entire file into a string
    std::string neverClaim((std::istreambuf_iterator<char>(formulaFile)),
                           std::istreambuf_iterator<char>());

    formulaFile.close();
    // Remove the temporary file
    fs::remove(formulaPath);

    return neverClaim;
}

int LTLClaimsProcessor::appendClaimToFile(const std::string & file, const std::string & ltl)
{
  std::string neverClaim = transformLTLStringToNeverClaim(ltl);
  // Open the file to append the never claim to it
  std::ofstream ftarget(file, std::ios::app); // std::ios::app will append to the file
  if (!ftarget.is_open()) {
    throw std::runtime_error("Could not append the never claim to the promela file. The file path " + file + " is invalid.\n");
    return 0; // Indicate failure
  }
  ftarget << "\n# 25 \"" << ltl << "\" \n" << neverClaim;

  // No need to explicitly close the file; it will be closed when ftarget goes out of scope
  return 1; // Indicate success
}

int LTLClaimsProcessor::appendClaim(const std::string& file, const std::string& path, const std::string& ltl, std::string& error) {
    std::string neverClaim = transformLTLStringToNeverClaim(ltl);
    // Open the file to append the never claim to it
    std::ofstream ftarget(file, std::ios::app); // std::ios::app will append to the file
    if (!ftarget.is_open()) {
        error = "Could not append the never claim to the promela file - invalid file path!\n";
        return 0; // Indicate failure
    }
    ftarget << "\n# 25 \"" << ltl << "\" \n" << neverClaim;

    // No need to explicitly close the file; it will be closed when ftarget goes out of scope
    return 1; // Indicate success
}

int LTLClaimsProcessor::renewClaimOfFile(const std::string& file, const std::string& definitions, const std::string& ltl) {
    std::string neverClaim = transformLTLStringToNeverClaim(ltl);
    std::cout << "File: " << file << "\n";
    std::ifstream fsource(file);
    if (!fsource.is_open()) {
        throw std::runtime_error("Could not open the file " + file + " for reading.\n");
    }

    std::string content, line;
    bool neverClaimFound = false;
    while (std::getline(fsource, line)) {
        // Skip lines starting with #define 
        if (line.find("#define") == 0) {
            continue;
        }else if (line.find("# 25") == 0) {
            neverClaimFound = true;
            continue;
        }else if (neverClaimFound) {
            // Skip rest of the file
            break;
        }
        content += line + "\n";
    }
    fsource.close();

    // Open the file to write the new content
    std::ofstream ftarget(file, std::ofstream::out | std::ofstream::trunc);
    if (!ftarget.is_open()) {
        throw std::runtime_error("Could not open the target file for writing.\n");
    }

    // Write the new content
    ftarget << content << "\n" << definitions << "\n" << "# 25 \"" << ltl << "\" \n" << neverClaim;
    ftarget.close();

    return 1;
}