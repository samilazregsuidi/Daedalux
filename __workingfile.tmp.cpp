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

byte n, i;

features f;

active proctype foo() {

 skip;

 do
 :: n++;
 :: else -> break;
 od;

Start:
 i = n;

 if :: f.B1 -> i = i+2; :: else -> skip; fi;
 if :: f.B2 -> i = i+1; :: else -> skip; fi;

Final:
 assert(i == n + 3);
}
