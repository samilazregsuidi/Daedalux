# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
typedef structure {
 bool a;
 bool b;
 bool c;
 bool d
}

structure s;

active proctype I(){

 s.a = true;
 s.b = true;

 do
 :: s.c = false;
 :: s.d = false;
 :: !s.c && !s.d
  -> break

 :: s.c = true;
 :: s.d = true;
 :: s.c && s.d
  -> break
 od;

 assert((s.c && s.d) || (!s.c && !s.d));
}

active proctype J(){
 do
 :: s.a = !s.a
 :: s.b = !s.b
 :: s.a || s.b
  -> break
 od;

 assert(s.a || s.b);
}
