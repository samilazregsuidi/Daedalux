typedef features {
	bool B1;
	bool B2
}

features f;

variant p1 = f.B1;
variant p2 = f.B2;

//(p1 x p2) x never

byte n, i;

active proctype foo() {
	
	do
	:: break;
	:: n++;
	od;
	
Start:
	skip;
	if
	:: f.B1 -> i = i + 1;
	:: else -> skip;
	fi;
	
	if
	:: f.B2 -> i = i + 2;
	:: else -> skip;
	fi;
	
Final:
	skip;
}

never { 
T0_init:
	skip;
	if
	:: (1) -> goto T0_init;
	:: p1.foo@Start && p2.foo@Start && p1.n == p2.n -> goto T1;
	fi;
	
T1:
	skip;
	if
	:: (1) -> goto T1;
	:: p1.foo@Final && p2.foo@Final && p1.i >= p2.i -> goto accept_all;
	fi;
	
accept_all:
	skip;
}
