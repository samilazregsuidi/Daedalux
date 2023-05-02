# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
# 188 "__workingfile.tmp"
typedef features {
 bool Start;
 bool Stop;
  bool MethaneAlarm;
  bool MethaneQuery;
 bool Low;
 bool Normal;
 bool High
}

features f;

system p1 = !f.MethaneAlarm;
system p2 = f.MethaneQuery;

mtype = {stop, start, alarm, low, high, ready, running, stopped, methanestop, lowstop}

chan cCmd = [0] of {mtype};
chan cAlarm = [0] of {mtype};
chan cMethane = [0] of {mtype};
chan cLevel = [0] of {mtype};


active proctype controller() {
 mtype pstate = stopped;
 mtype pcommand = start;
 mtype level = low;

 bool pumpOn = false;

 do :: atomic {

   cCmd?pcommand;





   if :: pcommand == stop;
     if :: f.Stop;
       if :: atomic {
          pstate == running;
          pumpOn = false;
         }
        :: else
        fi;
       pstate = stopped;
      :: else
      fi;
    :: pcommand == start;
     if :: f.Start;
       if :: atomic {
          pstate != running;
          pstate = ready;
         };
        :: else
        fi;
      :: else
      fi;
    fi;


   cCmd!pstate;

   }


  :: atomic {

   cAlarm?_;


   if :: f.MethaneAlarm;
     if :: atomic {
        pstate == running;
        pumpOn = false;
       };
      :: else
      fi;

     pstate = stopped;

    :: else
    fi;

   }

  :: atomic {
    cLevel?level;


   if :: level == high;
     if :: f.High;


       if :: pstate == ready;
         if :: f.MethaneQuery;

           atomic {
            cMethane!pstate;
            cMethane?pstate;
            if :: pstate == ready;
              pstate = running;
              pumpOn = true;
             :: else
            fi;
           };
          :: else;
           atomic {
            pstate = running;
            pumpOn = true;
           };
          fi;
        :: else
        fi;

      :: else
      fi;
    :: level == low;
     if :: f.Low;
       if :: atomic {
          pstate == running;
          pumpOn = false;
          pstate = stopped;
         };
        :: else
        fi;
      :: else
      fi;
    fi;
    }
  od;
}

active proctype user() {
 do :: if :: cCmd!start;
    :: cCmd!stop;
    fi;
   cCmd?_;
  od;
}


bool methane = false;

active proctype methanealarm() {
 do :: methane = true;
   cAlarm!alarm;
  :: methane = false;
  od;
}


active proctype methanesensor() {

 do :: atomic {
    cMethane?_;
    if :: methane;
      cMethane!methanestop;
     :: !methane;
      cMethane!ready;
     fi;
   };
  od;
}

active proctype watersensor() {
 do :: atomic {
    if :: cLevel!low;
     :: cLevel!high;
     fi;
   };
  od;
};

never {
T0_init :
 if
 :: (p1.controller.pumpOn != p2.controller.pumpOn) && (p1.controller.pstate == p2.controller.pstate) -> goto accept_S2
 :: (1) -> goto T0_init
 fi;
accept_S2 :
 if
 :: (p1.controller.pumpOn != p2.controller.pumpOn) -> goto accept_S2
 fi;
}
