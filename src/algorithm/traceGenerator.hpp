#ifndef TRACEGENERATOR_HPP
#define TRACEGENERATOR_HPP

#include "../formulas/formula.hpp"
#include "fsm.hpp"
#include "promela_loader.hpp"
#include "trace.hpp"
#include "traceReport.hpp"
#include "tvl.hpp"
#include <memory>

class TraceGenerator {
public:
  TraceGenerator(const std::string & original_file_path, const std::string & mutant_file_path, TVL * tvl = nullptr) : tvl(tvl)
  {
    auto loader_original = new promela_loader(original_file_path, tvl);
    original = loader_original->getAutomata();
    delete loader_original;
    auto loader_mutant = new promela_loader(mutant_file_path, tvl);
    mutant = loader_mutant->getAutomata();
    delete loader_mutant;
  }

  TraceGenerator(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant) : original(original), mutant(mutant), tvl(nullptr)
  {
  }

  TraceGenerator(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant, TVL * tvl)
      : original(original), mutant(mutant), tvl(tvl)
  {
  }

  std::shared_ptr<trace> generateNegativeTrace(const size_t k = 200,  bool appendLastStateIfDeadlock = false,  bool ignore_common_prefix = false);
  std::shared_ptr<trace> generatePositiveTrace(const size_t k = 200,  bool appendLastStateIfDeadlock = false, bool ignore_common_prefix = false);
  std::unique_ptr<traceReport> generateTraceReport(const size_t no_traces = 20, const size_t len_traces = 200,
                                                   bool ignore_common_prefix = false);

  std::shared_ptr<trace> generateDiscriminatingTrace(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant,
                                                            const size_t trace_length, bool appendLastStateIfDeadlock = false);

  static std::shared_ptr<trace> generateTrace(std::shared_ptr<fsm> original, const size_t trace_length);

private:
  std::shared_ptr<trace> generateTrace(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant, const size_t trace_length,
                                       bool ignore_common_prefix = false);

  std::shared_ptr<fsm> original;
  std::shared_ptr<fsm> mutant;
  TVL * tvl;
};

#endif // TRACEGENERATOR_HPP
