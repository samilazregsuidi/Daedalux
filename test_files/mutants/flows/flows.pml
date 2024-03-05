typedef structure {
	bool a;
	bool b;
	bool c;
	bool d
}

structure s;

active proctype I(){

	s.a = true;
	s.b = true;
	
	do
	::  s.c = false;
	::  s.d = false;
	::  !s.c && !s.d 
		-> break
	
	:: s.c = true;
	:: s.d = true;
	:: s.c && s.d 
		-> break
	od;
}

active proctype J(){
	do
	:: s.a = !s.a
	:: s.b = !s.b		
	:: s.a || s.b 
		-> break
	od;
}
