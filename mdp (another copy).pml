int s = 0;

active proctype mdp(){
	
	
	do
	:: s == 0 -> 
		ac
		:: s0a0 -> 
			if
			:: [ 0.5 ] s = 0;
			:: [ 0.5 ] s = 2;
			fi;
			
		:: s0a1 -> s = 2;
		ca;
	
	:: s == 2 ->
		ac
		:: s2a0 -> 
			if
			:: [ 0.4 ] s = 0;
			:: [ 0.6 ] s = 2;
			fi;
			
		:: s2a1 ->
			if
			:: [ 0.4 ] s = 2;
			:: [ 0.3 ] s = 0;
			:: [ 0.3 ] s = 1;
			fi;
		ca;
		
	:: s == 1 -> 
		ac 
		:: s1a0 ->
			if 
			:: [ 0.1 ] s = 1;
			:: [ 0.7 ] s = 0;
			:: [ 0.2 ] s = 2;
			fi;
			
		:: s1a1 ->
			if 
			:: [ 0.95 ] s = 1;
			:: [ 0.05 ] s = 2;
			fi;
		ca;
	od;
}

bltl prop0 {
	[] {k > 10} (s == 1)
}

