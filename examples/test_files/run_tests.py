import glob
import subprocess

separator = '=====================================================\n'

tests = glob.glob('./test/basic/*.pml')
assert(len(tests) > 0)
percent = 0

for test in tests:
	
	print(separator)
	print("------------- "+str(percent * 100 / len(tests))+"% ---------------")
	
	res = subprocess.run(['./deadalux', test], capture_output=True)
	to_print = res.stdout.decode('utf-8')
	
	#print(to_print)
	
	if to_print.find('failed') != -1:
		print('KO test' + test +'\n')
	else:
		print('OK test ' + test +'\n')
		
	percent = percent + 1
