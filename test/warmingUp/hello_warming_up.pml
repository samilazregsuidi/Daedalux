typedef features {
	bool B1;
	bool B2
}

features f;

system p1 = f.B1;
system p2 = f.B2;

bool n, i;

active proctype foo() {

	do 
	:: n = true;
	:: break;
	od;
	
	if 
	:: f.B1 -> i = n; 
	:: else -> skip; 
	fi;
	
	if 
	:: f.B2 -> i = !n; 
	:: else -> skip; 
	fi;
}
