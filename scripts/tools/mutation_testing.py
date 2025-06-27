import glob
import subprocess
import os 

separator = '=====================================================\n'
separator_ = '-----------------------------------------------------\n'

diag = ''
properties_killings = ''
      
killing_report = dict()

time_limit = 60

model_file_name = '../models/minepump/original.pml'
property_file_name = '../models/minepump/ltl.inc'
number_of_mutants = 1

# Detect the properties to check from the property file
property_file = open(property_file_name, 'r')
properties = set()
for line in property_file:
	if line.startswith('never') or line.startswith('ltl'):
		properties.add(line.split(' ')[1].strip())

base_folder = os.path.dirname(os.path.abspath(model_file_name))
relative_folder = os.path.relpath(base_folder, os.getcwd())
mutants_folder = os.path.join(relative_folder, 'mutants')

# Build the mutants from the original model
subprocess.run(['../daedalux -f ' + model_file_name + ' -n ' + str(number_of_mutants) + ' -p ' + property_file_name + ' mutants'], shell=True)
								
properties = {
	'pump_state_consistency',
	'pump_synch_on', 
	'read_msg_consistency',

	'pump_methane_switch_off'
	'pump_methane_safetyness', 
	'pump_safe_methane_starting', 

	'pump_stopping', 
	'pump_stopped',
	'pump_effectiveness', 
	'pump_activation',

	'water_level_consistency',

	'user_cmd_consistency'
}

#properties = {
#'pump_state_consistency',
#'pump_synch_on', 
#'read_msg_consistency',
#'pump_weak_synch_off',
#'pump_strong_synch_off',

#'pump_methane_switch_off'
#'pump_methane_safetyness', 
#'pump_safe_methane_starting', 
#'methane_sensor_liveness',
#'methane_sensor_liveness_light',
#'pump_methane_safetyness_light',

#'pump_stopping', 
#'pump_stopped',
#'pump_effectiveness', 
#'pump_activation',

#'water_level_consistency',
#'medium_water_evolution_consistency',
#'high_water_evolution_consistency',

#'user_cmd_consistency'
#}

mutants = glob.glob(mutants_folder + '/*.pml')
survivors = glob.glob(mutants_folder + '/*.pml')
# Ensure that we have at least one mutant and one property to check
assert(len(mutants) > 0)
assert(len(properties) > 0)

# Keep track of the mutants that are killed by each property
for prop in properties:
	killing_report[prop] = list()

percent = 0

for mutant in mutants:
	print(separator)
	print("------------- "+str(percent * 100 / len(mutant))+"% ---------------")
	print("Checking mutant : " + mutant)
	
	res = subprocess.run(['spin','-a', mutant], capture_output=True)
	to_print = res.stdout.decode('utf-8')
	
	if to_print.find('error') != -1 or to_print.find('Error') != -1 or to_print.find('ERROR') != -1:
		print('syntax or semantic error ' + mutant + '\n')
		continue
	
	print('parsed \t\t' + mutant)
		
	res = subprocess.run(['gcc', '-DNOREDUCE', '-o', 'pan', 'pan.c'], capture_output=True)
	to_print = res.stdout.decode('utf-8')
	if to_print.find('error') != -1:
		diag += 'compilation error ' + mutant+'\n'
		continue
		
	print('compiled \t' + mutant)
	print(separator_)
	
	for prop in properties:
		
		print('checking \t'+mutant+'#'+prop)
		try:
			res = subprocess.run(['./pan', '-a', '-f', '-n', '-m100000', '-w30', '-N', prop], capture_output=True, timeout=time_limit)
			
		except subprocess.TimeoutExpired:
			print('timeout \t'+ mutant + '#' +  prop + '\n')
			diag += (prop+'\t\t'+ mutant +'\t\t timeout\n')
			killing_report[prop].append(mutant)
			if mutant in survivors:
				survivors.remove(mutant)
			continue
		
		to_print = res.stdout.decode('utf-8')
		
		if to_print.find('acceptance cycle') != -1 or to_print.find('assertion violated') != -1:
			print('violated \t'+mutant+'#'+prop+'\n')
			killing_report[prop].append(mutant)
			if mutant in survivors:
				survivors.remove(mutant)
			diag += (prop+'\t\t'+ mutant +'\t\t violated\n')
		else:
			print('satisfied \t'+mutant+'#'+prop+'\n')
			diag += (prop+'\t\t'+ mutant +'\t\t satisfied\n')
			
		print(separator_)
		diag += to_print+'\n'+separator+'\n'
		
	percent = percent + 1

	# remove trail files
	subprocess.run(['rm', '-f', '*.tmp'])
	subprocess.run(['rm', '-f', '**.trail'])
	subprocess.run(['rm', '-f','pan.*'])
	
for prop in properties:
	if (len(killing_report[prop]) * 100 / len(mutants)) > 0:
		properties_killings += "*****************************************************\n\n"
		properties_killings += prop + ' killed '+ str(len(killing_report[prop]) * 100 / len(mutants)) + '% of mutants\n\n'


print(properties_killings)
diag += properties_killings

nb_killed = len(mutants) - len(survivors)
mutation_score = nb_killed * 100 / len(mutants)
mutation_result = '********** MUTATION SCORE : '+ str(mutation_score) + '% **********\n\n'
mutation_result += 'SURVIVORS : \n{\n'
for survivor in survivors:
	mutation_result += '\t' + str(survivor) + '\n'
mutation_result += '}\n'

print(mutation_result)
diag += mutation_result

#print(diag)
log = open("log.txt", 'w')
log.write(diag)

# Remove all the files created by the script
subprocess.run(['rm', '-f', '*.tmp'])
subprocess.run(['rm', '-f', '*trail'])
subprocess.run(['rm', '-f','pan.*'])
