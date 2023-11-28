#include <errno.h>
#include "ltl.hpp"
#include "assert.h"

int appendClaim(const std::string& file, const std::string& path, const std::string& ltl, std::string& error) {
	
	if(system(std::string(path + "libs/ltl2ba/ltl2ba -f \"!("+ltl+")\" > __formula.tmp").c_str()) != 0) {
		FILE* fsource;
		fsource = fopen("__formula.tmp", "r");
		if(fsource != NULL) {
			int i = 0;
			do {
				error[i] = fgetc(fsource);
				i++;
			} while(!feof(fsource));
			error[i-2] = '\0';
			fclose(fsource);
		}
	} else {
		FILE* fsource;
		FILE* ftarget;
		fsource = fopen("__formula.tmp", "r");
		ftarget = fopen(file.c_str(), "a");

		if(fsource == NULL || ftarget == NULL) {
			printf("Could not append the never claim to the promela file!\n");
			assert(false);
		} else {
			fputc('\n', ftarget);
			fputs(("# 25 \""+ltl+"\" \n").c_str(), ftarget);
			char buffer;
			buffer = fgetc(fsource);
			while(!feof(fsource)) {
				fputc(buffer, ftarget);
				buffer = fgetc(fsource);
			}
			fclose(fsource);
			fclose(ftarget);

			//if(!keepTempFiles) remove("__formula.tmp");
			return 1;
		}

		if(fsource != NULL) fclose(fsource);
		if(ftarget != NULL) fclose(ftarget);
	}

	//if(!keepTempFiles) remove("__formula.tmp");
	return 0;
}
