typedef features {
	bool B1;
	bool B2
}

features f;

system p1 = f.B1;
system p2 = f.B2;

byte n;
short i;

active proctype foo() {

	/*do 
	:: break;
	:: n++;
	od;*/
	
	
Start:  i = n;
	
	if :: f.B1 -> i = i + 2; :: else ; fi;
	
	if :: f.B2 -> i = i + 1; :: else ; fi;
	
Final:  skip;

	do
	:: true;
	od;
}

never { // !( G( (p1.foo@Start && p2.foo@Start && p1.n==p2.n) -> F (p1.foo@Final && p2.foo@Final && p1.i>=p2.i) ) ) 
T0_init :    // init 
	if
	:: (1) -> goto T0_init
	:: (!p1.foo@Final && p1.n==p2.n && p2.foo@Start && p1.foo@Start) || (!p2.foo@Final && p1.n==p2.n && p2.foo@Start && p1.foo@Start) || (!(p1.i>=p2.i) && p1.n==p2.n && p2.foo@Start && p1.foo@Start) -> goto accept_S2
	fi;
accept_S2 :    // 1
	if
	:: (!p1.foo@Final) || (!p2.foo@Final) || (!(p1.i>=p2.i)) -> goto accept_S2
	fi;
}

/*never { // !( G( (p1.foo@Start && p2.foo@Start && p1.n==p2.n) -> F (p1.foo@Final && p2.foo@Final && p1.i>=p2.i) ) ) 
T0_init :    // init 
	if
	:: (1) -> goto T0_init
	:: (!p1.foo@Final && p1.n==p2.n && p2.foo@Start && p1.foo@Start) || (!p2.foo@Final && p1.n==p2.n && p2.foo@Start && p1.foo@Start) || (!p1.i>=p2.i && p1.n==p2.n && p2.foo@Start && p1.foo@Start) -> goto accept_S2
	fi;
accept_S2 :    // 1
	if
	:: (!p1.foo@Final) || (!p2.foo@Final) || (!p1.i>=p2.i) -> goto accept_S2
	fi;
}*/

