typedef features {
	bool B1;
	bool B2
}

features f;

system p1 = f.B1;
system p2 = f.B2;

active proctype foo() {

	byte n;
	short i;

	do 
	:: n++;
	:: break;
	od;
	
Start:
	i = n;
	
	if :: f.B1 -> i = i+2; :: else -> skip; fi;
	if :: f.B2 -> i = i+1; :: else -> skip; fi;
	
Final:
	assert(i == n + 3);
}

never {
	do
	:: p1.foo@Start && p2.foo@Start -> break;
		/*do
		:: p1.foo.i == p1.foo.n && p2.foo.i == p2.foo.n -> break;
		:: p1.foo@Final && p2.foo@Final -> break;
		:: true -> skip;
		od;*/
		
	:: p1.foo@Final && p2.foo@Final -> break;
	 
	:: true -> skip
	od;
}
