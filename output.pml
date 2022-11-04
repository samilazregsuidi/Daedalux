typedef features {
	bool B1;
	bool B2;
}

features f;
system p1;
system p2;
byte n, i;
active proctype foo(){
	p1.f.B1 = true;
	p2.f.B2 = true;
	do
	:: break;
	:: n++;
	od;
	
Start: 
	if
	:: p1.f.B1;
		i = i + 1;
	:: else;
		skip;
	fi;
	if
	:: p2.f.B2;
		i = i + 2;
	:: else;
		skip;
	fi;
	
Final: 
	printf("i: %d", i);
	assert(p1.i == 1 && p2.i == 3);
}
