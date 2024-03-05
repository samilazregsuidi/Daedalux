int array[4];

active proctype test (){
	int i = 0;
	do
	:: i < 3 ->
		array[i] = i + 2;
		i++;
	:: else ->
		skip;
	od;
}