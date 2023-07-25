# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
int s = 0;

active proctype mdp(){


 do
 :: s == 0 ->
  if
  :: s0a0 ::
   if
   :: [ 0.5 ] s = 0;
   :: [ 0.5 ] s = 2;
   fi;

  :: s0a1 :: s = 2;
  fi;

 :: s == 2 ->
  if
  :: s2a0 ::
   if
   :: [ 0.4 ] s = 0;
   :: [ 0.6 ] s = 2;
   fi;

  :: s2a1 ::
   if

   :: [ 0.7 ] s = 0;
   :: [ 0.3 ] s = 1;
   fi;
  fi;

 :: s == 1 ->
  if
  :: s1a0 ::
   if
   :: [ 0.1 ] s = 1;
   :: [ 0.7 ] s = 0;
   :: [ 0.2 ] s = 2;
   fi;

  :: s1a1 ::
   if
   :: [ 0.95 ] s = 1;
   :: [ 0.05 ] s = 2;
   fi;
  fi;
 od;
}

never {
T0_init :
 if
 :: (s == 1) -> goto accept_all
 :: (s != 1) -> goto T0_init
 fi;
accept_all :
 skip
}
