typedef features {
	bool B1;
	bool B2
}

byte n, i;

features f;

active proctype foo() {

	skip;
	
	do 
	:: n++;
	:: else -> break;
	od;
	
Start:
	i = n;
	
	if :: f.B1 -> i = i+2; :: else -> skip; fi;
	if :: f.B2 -> i = i+1; :: else -> skip; fi;
	
Final:
	assert(i == n + 3);
}
