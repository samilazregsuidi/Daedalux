byte i = 0;
byte a = 0;
byte j = 0;
byte b = 0;
	
active proctype I (){
	
	do
	:: atomic {
		i = i + 1; 
		a = a + 1;
		}
	od;	
}

active proctype J (){
	
	do
	:: atomic { 
		j = j - 1; 
		b = b - 1;
		}
	od;		
}

active proctype test(){
	do
	::	assert(i == a && j == b);
	od;
}

