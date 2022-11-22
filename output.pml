int s = 0;
active proctype mdp(){
	do
	:: s == 0;
		if
		:: s0a0 :: 			if
			:: s = 0;
			:: s = 2;
			fi;
		:: s0a1 :: 			s = 2;
		fi;
	:: s == 2;
		if
		:: s2a0 :: 			if
			:: s = 0;
			:: s = 2;
			fi;
		:: s2a1 :: 			if
			:: s = 2;
			:: s = 0;
			:: s = 1;
			fi;
		fi;
	:: s == 1;
		if
		:: s1a0 :: 			if
			:: s = 1;
			:: s = 0;
			:: s = 2;
			fi;
		:: s1a1 :: 			if
			:: s = 1;
			:: s = 2;
			fi;
		fi;
	od;
}
