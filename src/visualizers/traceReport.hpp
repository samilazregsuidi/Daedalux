#pragma once

#include <unordered_set>
#include <iostream>
#include <string>
#include <cassert> // Use <cassert> for the assert macro

#include "trace.hpp"

class traceReport
{
private:
    std::unordered_set<trace*> goodTraces;
    std::unordered_set<trace*> badTraces;

public:
    traceReport();
    traceReport(std::unordered_set<trace*> goodTraces, std::unordered_set<trace*> badTraces);
    ~traceReport();

    std::unordered_set<trace*> getGoodTraces() const { return this->goodTraces; }
    std::unordered_set<trace*> getBadTraces() const { return this->badTraces; }

    void addGoodTrace(trace* t) {
        // Ensure that the trace is not in the bad traces.
        if (this->badTraces.find(t) == this->badTraces.end()) {
            this->goodTraces.insert(t);
        } else {
            // Handle the case where the trace is in the bad traces.
            // You may want to throw an exception or log an error.
        }
    }

    void addBadTrace(trace *t) {
        // Ensure that the trace is not in the good traces.
        if (this->goodTraces.find(t) == this->goodTraces.end()) {
            this->badTraces.insert(t);
        } else {
            // Handle the case where the trace is in the good traces.
            // You may want to throw an exception or log an error.
        }
    }

    void printCSV(std::ostream& goodTraceFile, std::ostream& badTraceFile) const {
        for (const auto& t : this->goodTraces) {
            t->printCSV(goodTraceFile);
        }
        for (const auto& t : this->badTraces) {
            t->printCSV(badTraceFile);
        }
    }
};
