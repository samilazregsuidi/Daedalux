#ifndef TVL_H
#define TVL_H

#include <map>
#include <string>

#include "cuddObj.hh"
#include "cuddInt.h"

class expr;
class utypeSymNode;

class TVL {
public:
    static void initBoolFct(void);
    
    static Cudd* getMgr(void);
    
    TVL(void);

    TVL(const std::string& filename);
    
    bool loadFeatureModelDimacs(const std::string& clauseFile, const std::string& mappingFile); // Builds the FD formula from dimacs files.
    
    bool loadFeatureModel(const std::string& filename, const std::string& filter); // Add 'filter' as constraint to a TVL model, feed it into the TVL library, and builds the corresponding FD formula.

    bool filterFeatureModel(const std::string& filename, const std::string& filter) const;

    const ADD& getFeatureModelClauses(void) const; // Returns the previously built FD formula (that is, the "base FD").
    
    const std::vector<BDD>& getVars(void) const ;
    
    int getFeatureID(const std::string& name) const; // Returns the ID mapped to feature 'name'
    
    std::string getFeatureIDName(int id) const; // Returns the feature name mapped to ID 'id'.
    
    int getNbFeatures(void) const; // Returns highest feature ID
    
    int createMapping(const std::string& name); // Returns the ID mapped to feature 'name'. Iff the mapping does not already exist, it is created.
    
    bool hasFeature(const std::string& name) const;

    BDD getFeature(const std::string& name) const;

    BDD getFeature(int id) const;

    ADD getFormula(utypeSymNode* features) const;

    ADD getFormula(const expr* e) const;
    
    std::vector<ADD> getProducts(utypeSymNode* features) const;
    
    void printInfo(void) const;

    void printBool(const ADD& formula) const;

    void printBool(void) const;

    std::vector<ADD> getProducts(const ADD& formula) const;

    int getNbProducts(void) const;

    void printMinterms(const ADD& formula) const;

private:
    
    bool dimacsFileToBool(const std::string& filePath, ADD& fm);

    static Cudd* mgr;

public:
    ADD featureModelClauses;
    std::vector<BDD> vars;

    std::map<std::string, int> featureIDMapping;
    int nbFeatures;
    int maxId;
};

#endif