#include "traceReport.hpp"

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

std::unique_ptr<traceReport> traceReport::removeCommonPrefixes()
{
  assert(goodTraces.size() == 1);
  assert(badTraces.size() == 1);
  auto goodTrace = *goodTraces.begin();
  auto badTrace = *badTraces.begin();
  auto goodStates = goodTrace->getStates();
  auto badStates = badTrace->getStates();
  auto good_state_it = goodStates.begin();
  auto bad_state_it = badStates.begin();

  while (good_state_it != goodStates.end() && bad_state_it != badStates.end()) {
    auto goodState = *good_state_it;
    auto badState = *bad_state_it;
    if (goodState->delta(badState.get()) < 0.000001) {
      good_state_it++;
      bad_state_it++;
      // Remove the common prefix
      goodStates.pop_front();
      badStates.pop_front();
    }
    else {
      break;
    }
  }
  auto newGoodTrace = std::make_shared<trace>(goodStates);
  auto newBadTrace = std::make_shared<trace>(badStates);
  auto newTraceReport = std::make_unique<traceReport>();
  newTraceReport->addGoodTrace(newGoodTrace);
  newTraceReport->addBadTrace(newBadTrace);
  return newTraceReport;
}