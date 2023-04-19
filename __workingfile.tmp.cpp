# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
typedef features {
 bool B1;
 bool B2;
 bool B3
}

features f;

system p1 = f.B1;
system p2 = f.B2;
system p3 = f.B3;

byte n;
short i;

active proctype foo() {

 do
 :: break;
 :: n++;
 od;


Start: i = n;

 if :: f.B1 -> i = i + 3; :: else ; fi;

 if :: f.B2 -> i = i + 2; :: else ; fi;

 if :: f.B3 -> i = i + 1 :: else ; fi;

Final: skip;

 do
 :: true;
 od;
}

never {
T0_init :
 if
 :: ( (!p1.foo@Final) || (!p2.foo@Final) || (!p3.foo@Final) || (!(p1.i>=p2.i)) || (!(p2.i>=p3.i)) ) && (p1.foo@Start && p2.foo@Start && p3.foo@Start && p1.n==p2.n && p3.n==p2.n) -> goto accept_S2;
 :: (1) -> goto T0_init;
 fi;
accept_S2 :
 if
 :: (!p1.foo@Final) || (!p2.foo@Final) || (!p3.foo@Final) || (!(p1.i>=p2.i)) || (!(p2.i>=p3.i)) -> goto accept_S2
 fi;
}
