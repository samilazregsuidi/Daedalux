# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
typedef features {
 bool B1;
 bool B2
}

features f;

byte n, i;

active proctype foo() {

 do
 :: break;
 :: n++;
 od;

Start:
 skip;
 if
 :: f.B1 -> i = i + 1;
 :: else -> skip;
 fi;

 if
 :: f.B2 -> i = i + 2;
 :: else -> skip;
 fi;

Final:
 skip;
}
