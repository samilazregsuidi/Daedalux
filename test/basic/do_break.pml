active proctype foo() {

	byte n;

	do 
	:: true -> break;
	:: n++;
	od;
	
	assert(n >= 0);
}
