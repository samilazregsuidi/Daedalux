#include "ltl.hpp"
#include "assert.h"
#include <errno.h>
#include <filesystem>
#include <iostream> // Include the <iostream> header

std::string transformLTLStringToNeverClaim(const std::string & ltl)
{
  std::string currentPath = std::filesystem::current_path().string();
  std::string ltl2baPath = currentPath + "/../src/bin/ltl2ba";
  // Assert that the ltl2ba binary exists
  if (!std::filesystem::exists(ltl2baPath)) {
    printf("Could not find the ltl2ba binary!\n");
    assert(false);
  }
  // Create a temporary file to store the never claim
  std::string formulaPath = currentPath + "/__formula.tmp";

  std::string negatedLTL = "!(" + ltl + ")";
  // Call LTL2BA to transform the LTL formula into a never claim
  auto result = system(std::string(ltl2baPath + " -f \"" + negatedLTL + "\" > " + formulaPath).c_str());
  if (result != 0) {
    printf("Could not transform the LTL formula into a never claim!\n");
    assert(false);
  }
  FILE * formulaFile;
  formulaFile = fopen("__formula.tmp", "r");
  if (formulaFile == NULL) {
    printf("Could not open the never claim file!\n");
    assert(false);
  }
  std::string neverClaim;
  char buffer;
  buffer = fgetc(formulaFile);
  while (!feof(formulaFile)) {
    neverClaim += buffer;
    buffer = fgetc(formulaFile);
  }
  fclose(formulaFile);
  remove(formulaPath.c_str());
  return neverClaim;
}

int appendClaimToFile(const std::string & file, const std::string & ltl)
{
  std::string neverClaim = transformLTLStringToNeverClaim(ltl);
  FILE * ftarget;
  // Open the file to append the never claim to it
  ftarget = fopen(file.c_str(), "a");
  if (ftarget == NULL) {
    printf("Could not append the never claim to the promela file - invalid file path!\n");
    return 0;
  }
  fputc('\n', ftarget);
  fputs(("# 25 \"" + ltl + "\" \n").c_str(), ftarget);
  for (char c : neverClaim) {
	fputc(c, ftarget);
  }
  fclose(ftarget);
  return 1;
}

int appendClaim(const std::string & file, const std::string & path, const std::string & ltl, std::string & error)
{
  std::string neverClaim = transformLTLStringToNeverClaim(ltl);
  FILE * ftarget;
  // Open the file to append the never claim to it
  ftarget = fopen(file.c_str(), "a");
  if (ftarget == NULL) {
    printf("Could not append the never claim to the promela file - invalid file path!\n");
    return 0;
  }
  fputc('\n', ftarget);
  fputs(("# 25 \"" + ltl + "\" \n").c_str(), ftarget);
  for (char c : neverClaim) {
	fputc(c, ftarget);
  }
  fclose(ftarget);
  return 1;
}