active proctype foo(){
	byte n;
	Start: 
n++;
}
active proctype test(){
	foo@Start: 
n++;
;
}
