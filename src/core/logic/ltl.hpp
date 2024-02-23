#ifndef LTL_H
#define LTL_H

#include <string>

std::string transformLTLStringToNeverClaim(const std::string & ltl);

int appendClaim(const std::string & file, const std::string & path, const std::string & ltl, std::string & error);

int appendClaimToFile(const std::string & file, const std::string & ltl);

#endif