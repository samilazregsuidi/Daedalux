# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
system p1;
system p2;

bool x;

active proctype test()
{
 do
 :: x = false;
 :: x = true;
 od;
}

never {
T0_init :
 if
 :: !(p1.x && p2.x) -> goto accept_S2
 :: (1) -> goto T0_init

 fi;
accept_S2 :
 if
 :: !(p1.x && p2.x) -> goto accept_S2
 fi;
}
