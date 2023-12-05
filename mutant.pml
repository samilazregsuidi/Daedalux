active proctype test(){
	byte a = 0;
	byte b = 1;
	byte temp = 0
	bool fire = true;
	do
	:: fire = true -> skip
	:: fire ->
		temp = a;
		a = b;
		b = temp;
		
	:: else -> skip;
	od;
}
