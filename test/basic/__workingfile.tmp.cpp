# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
typedef structure {
 bool B1;
 bool B2
}

byte n, i;

active proctype foo() {

 structure s;
 s.B1 = true;
 s.B2 = true;

 do
 :: break;
 :: n++;
 od;

Start:
 i = n;

 if :: s.B1 -> i = i+2; :: else -> skip; fi;
 if :: s.B2 -> i = i+1; :: else -> skip; fi;

Final:
 assert(i == n + 3);
}
