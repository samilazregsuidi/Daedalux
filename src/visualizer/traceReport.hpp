#pragma once
#include <unordered_set>
#include <memory>
#include "trace.hpp"

class traceReport
{
private:
    std::unordered_set<std::shared_ptr<trace>> goodTraces;
    std::unordered_set<std::shared_ptr<trace>> badTraces;

public:
    traceReport() = default;  // Default constructor

    traceReport(std::unordered_set<std::shared_ptr<trace>> goodTraces, std::unordered_set<std::shared_ptr<trace>> badTraces)
        : goodTraces(std::move(goodTraces)), badTraces(std::move(badTraces))
    {}
    ~traceReport() = default;

    const std::unordered_set<std::shared_ptr<trace>>& getGoodTraces() const;
    const std::unordered_set<std::shared_ptr<trace>>& getBadTraces() const;

    void addGoodTrace(std::shared_ptr<trace> t);

    void addBadTrace(std::shared_ptr<trace> t);

    void printCSV(std::ostream& goodTraceFile, std::ostream& badTraceFile) const;

    void removeCommonPrefixes();

    int getShortestTraceLength() const;
};