bool x;

active proctype test()
{
	x = true;
	do
	:: x = false;
	:: x = true;
	od;
}

never { // !x
T0_init :    // init
	if
	:: (!x) -> goto accept_all
	fi;
accept_all :    //1
	skip
}

/*never { // x
T0_init :    // init
	if
	:: (x) -> goto accept_all
	fi;
accept_all :    //1
	skip
}*/

/*never { // (! x) -> X (x) 
accept_init :    // init
	if
	:: (x) -> goto accept_all
	:: (1) -> goto accept_S1
	fi;
accept_S1 :    // 1 
	if
	:: (x) -> goto accept_all
	fi;
accept_all :    // 2 
	skip
}*/

/*never { // (!x) -> <> (x)
T0_init :    // init
	if
	:: (x) -> goto accept_all
	:: (1) -> goto T0_init
	fi;
accept_all :    //1
	skip
}*/

/*never { // []  ((! x) -> X (x)) 
accept_init :    // init 
	if
	:: (1) -> goto accept_S2
	:: (x) -> goto accept_init
	fi;
accept_S2 :    // 1 
	if
	:: (x) -> goto accept_init
	fi;
}*/

/*never { // []  ((! x) -> X (x)) 
accept_init :    // init 
	if
	:: (1) -> goto accept_S2
	:: (x) -> goto accept_init
	fi;
accept_S2 :    // 1 
	if
	:: (x) -> goto accept_init
	fi;
}*/

/*never { // []  ((! x) -> <> (x)) 
T0_init :    // init 
	if
	:: (x) -> goto accept_S1
	:: (1) -> goto T0_init
	fi;
accept_S1 :    // 1 
	if
	:: (x) -> goto accept_S1
	:: (1) -> goto T0_init
	fi;
}*/
