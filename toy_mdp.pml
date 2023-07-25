int s = 0;

active proctype mdp(){
	
	
	do
	:: s == 0 -> 
		if
		:: s0a0 :: 
			if
			:: [ 0.5 ] s = 0;
			:: [ 0.5 ] s = 1;
			fi;
		
		fi;
	
	:: s == 1 ->
		if
		:: s1a0 :: s = 1;
		fi;
	od;
}

never { /* !(G(!s1)) */
T0_init :    /* init */
	if
	:: (s == 1) -> goto accept_all
	:: (s != 1) -> goto T0_init
	fi;
accept_all :    /* 1 */
	skip
}
