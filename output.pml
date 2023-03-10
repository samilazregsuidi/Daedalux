bool x;
active proctype test(){
	x = true;
	do
	:: x = false;
	:: x = true;
	od;
}

never {
	
T0_init: 
	if
	:: (!x);
		goto accept_all;
	fi;
	
accept_all: 
	skip;
};
