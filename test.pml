typedef features {
	bool f1;
	bool f2
}

active proctype test() {

	byte t;
	features f;
	do
	:: true;
	:: true -> break; 
	od;
	
	f.f1 = true;
}

active proctype monitor() {
	
	test:t == true;
}

