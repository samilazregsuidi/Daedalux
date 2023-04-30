# 0 "__workingfile.tmp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "__workingfile.tmp"
# 198 "__workingfile.tmp"
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

mtype = {stop, start, alarm, low, medium, high, ready, running, stopped, methanestop, lowstop, commandMsg, alarmMsg, levelMsg}

chan cCmd = [0] of {mtype};
chan cAlarm = [0] of {mtype};
chan cMethane = [0] of {mtype};
chan cLevel = [0] of {mtype};

active proctype controller() {
 mtype pstate = stopped;
 mtype readMsg = commandMsg;
 mtype pcommand = start;
 mtype level = medium;

 bool pumpOn = false;

 do :: atomic {
    cAlarm?_;
    readMsg = alarmMsg;
   };
   if :: f.MethaneAlarm;
     if :: atomic {
        pstate == running;
        pumpOn = false;
       };
      :: else
      fi;
     pstate = methanestop;

    :: else
    fi;

  :: atomic {
    cLevel?level;
    readMsg = levelMsg;
   };
   if :: level == high;
     if :: f.High;


       if :: pstate == ready || pstate == lowstop;
         if :: f.MethaneQuery;
           skip;
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
          pstate = lowstop;
         };
        :: else
        fi;
      :: else
      fi;
    :: level == medium;
     skip;
    fi;
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
 mtype waterLevel = medium;
 do :: atomic {
    if :: waterLevel == low ->
      if :: waterLevel = low;
       :: waterLevel = medium;
       fi;
     :: waterLevel == medium ->
      if :: waterLevel = low;
       :: waterLevel = medium;
       :: waterLevel = high;
       fi;
     :: waterLevel == high ->
      if :: waterLevel = medium;
       :: waterLevel = high;
       fi;
     fi;
    cLevel!waterLevel;
   };
  od;
};
