#ifndef LTL_H
#define LTL_H

#include <string>

class LTLClaimsProcessor {
public:
    static std::string transformLTLStringToNeverClaim(const std::string &ltl);
    static int appendClaim(const std::string &file, const std::string &path, const std::string &ltl, std::string &error);
    static int appendClaimToFile(const std::string &file, const std::string &ltl);
    static int renewClaimOfFile(const std::string& file, const std::string& definitions, const std::string& ltl);
    static int removeClaimFromFile(const std::string& file);
};
#endif