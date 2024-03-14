#include "traceReport.hpp"
#include <limits>

const std::unordered_set<std::shared_ptr<trace>> & traceReport::getGoodTraces() const { return goodTraces; }
const std::unordered_set<std::shared_ptr<trace>> & traceReport::getBadTraces() const { return badTraces; }

void traceReport::addGoodTrace(std::shared_ptr<trace> t)
{
  // Ensure that the trace is not in the bad traces.
  if (badTraces.find(t) == badTraces.end()) {
    goodTraces.insert(std::move(t));
  }
  else {
    // Handle the case where the trace is in the bad traces.
    // You may want to throw an exception or log an error.
  }
}

void traceReport::addBadTrace(std::shared_ptr<trace> t)
{
  // Ensure that the trace is not in the good traces.
  if (goodTraces.find(t) == goodTraces.end()) {
    badTraces.insert(std::move(t));
  }
  else {
    // Handle the case where the trace is in the good traces.
    // You may want to throw an exception or log an error.
  }
}

void traceReport::printCSV(std::ostream & goodTraceFile, std::ostream & badTraceFile) const
{
  goodTraceFile << "Good Traces" << std::endl;
  for (const auto & t : this->goodTraces) {
    t->printCSV(goodTraceFile);
  }
  badTraceFile << "Bad Traces " << std::endl;
  for (const auto & t : this->badTraces) {
    t->printCSV(badTraceFile);
  }
}

// This function removes common prefixes from all the traces in the report.
void traceReport::removeCommonPrefixes()
{
  int shortest = this->getShortestTraceLength();
  int index = 0; // The index of the state to remove and the index of the state to compare with is always 0
  bool considerInternalVariables = false;
  for (int i = 0; i < shortest; i++) {
    bool all_traces_have_same_state = true;
    auto first_state = this->goodTraces.begin()->get()->getStates().front();
    for (auto & good_trace : this->goodTraces) {
      if (!good_trace->getStates().front()->isSame(first_state.get(), considerInternalVariables)) {
        all_traces_have_same_state = false;
        break;
      }
    }
    for (auto & bad_trace : this->badTraces) {
      if (!bad_trace->getStates().front()->isSame(first_state.get(), considerInternalVariables)) {
        all_traces_have_same_state = false;
        break;
      }
    }
    if (all_traces_have_same_state) {
      for (auto & good_trace : this->goodTraces) {
        good_trace->removeStateAt(index);
      }
      for (auto & bad_trace : this->badTraces) {
        bad_trace->removeStateAt(index);
      }
    }
    else {
      break;
    }
  }
}

int traceReport::getShortestTraceLength(void) const
{
  int shortest = std::numeric_limits<int>::max();
  for (const auto & t : this->goodTraces) {
    shortest = std::min(shortest, (int)t->size());
  }
  for (const auto & t : this->badTraces) {
    shortest = std::min(shortest, (int)t->size());
  }
  return shortest;
}
