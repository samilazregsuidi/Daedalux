#define Y 3

int s = 0;

active proctype mdp(){
	
	
	do
	:: s == 0 -> 
		if
		:: s0a0 :: 
			if
			:: [ 0.5 ] s = Y + 1;
			:: [ 0.5 ] s = Y + 2;
			fi;
			
		:: s0a1 :: s = 1;
		fi;
	
	:: s > 0 && s < Y ->
		if
		:: s2a0 :: 
			if
			:: [ 0.05 ] skip;
			:: [ 0.95 ] s++;
			fi;
		
		:: s2a1 :: s = 0;
		fi;
	
	:: s == Y -> 
	
		if
		:: [ 0.05 ] skip;
		:: [ 0.95 ] s++;
		fi;
	
	:: s == Y + 1 -> skip;
	:: s == Y + 2 -> skip;
	od;
}
