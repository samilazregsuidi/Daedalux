# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
int i = 0;

active proctype I (){
 if
 :: i++ -> skip;
 :: i-- -> skip;
 :: else -> skip;
 fi;
}

active proctype J (){
 if
 :: i++ -> skip;
 :: i-- -> skip;
 :: else -> skip;
 fi;
}
