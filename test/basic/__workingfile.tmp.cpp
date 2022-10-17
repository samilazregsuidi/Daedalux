# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
mtype = {right, left, up, down}

typedef struct {
 int a;
 bool b;
 mtype t
}

active proctype test (){
 struct s;
 mtype t = right;
 t = up;
 s.t = t;
}
