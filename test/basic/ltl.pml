bool x;

active proctype test()
{
	x = true;
	
	do
	:: x = false;
	:: x = true;
	od;
}

never { // !(G ((! x) -> F x)) 
T0_init :
	if
	:: (!x) -> goto accept_S2
	:: (1) -> goto T0_init

	fi;
accept_S2 :
	if
	:: (!x) -> goto accept_S2
	fi;
}

