#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "main.h"
#include "list.h"
#include "ltl.h"

int appendClaim(char* file, char* ltl, char* error) {
	char command[44 + strlen(ltl) + 1];
	sprintf(command, "%slib/ltl2ba/ltl2ba -f \"!(%s)\" > __formula.tmp", path, ltl);
	if(system(command) != 0) {
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
		ftarget = fopen(file, "a");

		if(fsource == NULL || ftarget == NULL) failure("Could not append the never claim to the promela file!\n");
		else {
			fputc('\n', ftarget);
			char buffer;
			buffer = fgetc(fsource);
			while(!feof(fsource)) {
				fputc(buffer, ftarget);
				buffer = fgetc(fsource);
			}
			fclose(fsource);
			fclose(ftarget);

			if(!keepTempFiles) remove("__formula.tmp");
			return 1;
		}

		if(fsource != NULL) fclose(fsource);
		if(ftarget != NULL) fclose(ftarget);
	}

	if(!keepTempFiles) remove("__formula.tmp");
	return 0;
}
