# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
bool x;

active proctype test()
{
 x = true;

 do
 :: x = false;
 :: x = true;
 od;
}
# 31 "__workingfile.tmp"
never {
accept_init :
 if
 :: (x) -> goto accept_all
 :: (1) -> goto accept_S1
 fi;
accept_S1 :
 if
 :: (x) -> goto accept_all
 fi;
accept_all :
 skip
}
