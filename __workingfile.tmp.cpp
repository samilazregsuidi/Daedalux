# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
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
