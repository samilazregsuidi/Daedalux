#pragma once

#include <unordered_set>
#include <iostream>
#include <string>
#include <cassert> // Use <cassert> for the assert macro

#include "trace.hpp"

class traceReport
{
private:
    std::unordered_set<trace> goodTraces; // Use std::set<trace>
    std::unordered_set<trace> badTraces;  // Use std::set<trace>

public:
    traceReport();
    traceReport(const std::unordered_set<trace>& goodTraces, const std::unordered_set<trace>& badTraces);
    ~traceReport();

    std::unordered_set<trace> getGoodTraces() const { return this->goodTraces; }
    std::unordered_set<trace> getBadTraces() const { return this->badTraces; }

    void addGoodTrace(trace t) {
        // Assert that the trace is not in the bad traces.
        for (const auto& badTrace : this->badTraces) {
            assert(badTrace != t);
        }
        this->goodTraces.insert(t);
    }

    void addBadTrace(trace t) {
        // Assert that the trace is not in the good traces.
        for (const auto& goodTrace : this->goodTraces) {
            assert(goodTrace != t);
        }
        this->badTraces.insert(t);
    }

    void printCSV(std::ostream& goodTraceFile, std::ostream& badTraceFile) const {
        for (const auto& t : this->goodTraces) {
            t.printCSV(goodTraceFile);
        }
        for (const auto& t : this->badTraces) {
            t.printCSV(badTraceFile);
        }
    }
};