#include "traceReport.hpp"

const std::unordered_set<std::shared_ptr<trace>>& traceReport::getGoodTraces() const { return goodTraces; }
const std::unordered_set<std::shared_ptr<trace>>& traceReport::getBadTraces() const { return badTraces; }

void traceReport::addGoodTrace(std::shared_ptr<trace> t) {
    // Ensure that the trace is not in the bad traces.
    if (badTraces.find(t) == badTraces.end()) {
        goodTraces.insert(std::move(t));
    } else {
        // Handle the case where the trace is in the bad traces.
        // You may want to throw an exception or log an error.
    }
}

void traceReport::addBadTrace(std::shared_ptr<trace> t) {
    // Ensure that the trace is not in the good traces.
    if (goodTraces.find(t) == goodTraces.end()) {
        badTraces.insert(std::move(t));
    } else {
        // Handle the case where the trace is in the good traces.
        // You may want to throw an exception or log an error.
    }
}

void traceReport::printCSV(std::ostream& goodTraceFile, std::ostream& badTraceFile) const {
    goodTraceFile << "Good Traces" << std::endl;
    for (const auto& t : this->goodTraces) {
        t->printCSV(goodTraceFile);
    }
    badTraceFile << "Bad Traces " << std::endl;
    for (const auto& t : this->badTraces) {
        t->printCSV(badTraceFile);
    }
}