#include "traceReport.hpp"

traceReport::traceReport() : goodTraces(std::unordered_set<trace*>()), badTraces(std::unordered_set<trace*>()) {}

traceReport::traceReport(std::unordered_set<trace*> goodTraces, std::unordered_set<trace*> badTraces)
    : goodTraces(goodTraces), badTraces(badTraces)
{
  // Assert that the two sets of traces are disjoint.
  for (const auto & goodTrace : goodTraces) {
    for (const auto & badTrace : badTraces) {
      assert(goodTrace != badTrace);
    }
  }
}

traceReport::~traceReport() {
  // Delete all of the traces in the good and bad traces.
  for (const auto & goodTrace : goodTraces) {
    delete goodTrace;
  }
  for (const auto & badTrace : badTraces) {
    delete badTrace;
  }
}