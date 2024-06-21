from Scarlet.ltllearner import LTLlearner
input_file_path = "trace_report_mutex_scarlet.trace"
output_file_path = "result.csv"
learner = LTLlearner(input_file = input_file_path, thres=0.0, timeout=1800, csvname = output_file_path)
learner.learn()

# Parse the learned LTL formula from the output file
open_file = open(output_file_path, "r")
content = open_file.read()
open_file.close()

print("Output file content: ", content)
# The LTL formula is the last line in the output file
ltl_formula = content.splitlines()[-1]

ltl_formula = ltl_formula.split(",")[2]
print("Learned LTL formula: ", ltl_formula)

# Translate the learned LTL formula to an equivalent LTL formula in Promela