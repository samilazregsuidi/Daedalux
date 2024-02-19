#ifndef TRACEGENERATOR_HPP
#define TRACEGENERATOR_HPP

#include "trace.hpp"
#include "traceReport.hpp"
#include "fsm.hpp"
#include "tvl.hpp"
#include <memory>


class TraceGenerator {
public:
  TraceGenerator(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant) : original(original), mutant(mutant), tvl(nullptr)
  {
  }

  TraceGenerator(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant, TVL * tvl)
      : original(original), mutant(mutant), tvl(tvl)
  {
  }

  std::shared_ptr<trace> generateNegativeTrace(const size_t k = 200, bool ignore_common_prefix = false);
  std::shared_ptr<trace> generatePositiveTrace(const size_t k = 200, bool ignore_common_prefix = false);
  std::unique_ptr<traceReport> generateTraceReport(const size_t no_traces = 20, const size_t len_traces = 200);

private:
  std::shared_ptr<trace> generateTrace(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant,
                                                       const size_t trace_length, bool ignore_common_prefix = false);

  std::shared_ptr<fsm> original;
  std::shared_ptr<fsm> mutant;
  TVL * tvl;
};

#endif // TRACEGENERATOR_HPP
