typedef features {
	bool B;
	bool C
}

features f;

int s = 0;

active proctype test(){
	do
	:: s++;
	:: s--;
	od;
}

fMultiLtl prop A{p1}[f.B], A{p2}[f.C]{
	
	[](s{p1} == s{p2})
}
