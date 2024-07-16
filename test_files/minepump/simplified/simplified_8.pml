#define readCommand (readMsg == commandMsg)
#define readAlarm (readMsg == alarmMsg)
#define readLevel (readMsg == levelMsg)
#define userStart (uwants == start)
#define userStop (uwants == stop)
#define highWater (waterLevel == high)
#define mediumWater (waterLevel == medium)
#define lowWater (waterLevel == low)
#define stateReady (pstate == ready)
#define stateRunning (pstate == running)
#define stateStopped (pstate == stopped)
#define stateMethanestop (pstate == methanestop)
#define stateLowstop (pstate == lowstop)

mtype = {levelMsg, stop, methanestop, alarm, running, commandMsg, start, alarmMsg, high, low, stopped, medium, ready, lowstop}
chan cMethane = [0] of {mtype};
chan cLevel = [0] of {mtype};
mtype pstate = stopped;
mtype readMsg = commandMsg;
bool pumpOn = false;
bool methane = false;
mtype waterLevel = medium;
active proctype controller(){
	mtype pcommand = start;
	mtype level = medium;
	do
	::	atomic {
			cLevel?level;
			readMsg = levelMsg;
		};
		if
		::	level == high;
			if
			::	pstate == ready || pstate == lowstop;
				atomic {
					cMethane!pstate;
					cMethane?pstate;
					if
					::	pstate == ready;
						pstate = running;
						pumpOn = true;
					::	else;
					fi;
				};
			::	else;
			fi;
			
		::	level == low;
			if
			::	atomic {
					pstate == running;
					pumpOn = false;
					pstate = lowstop;
				};
			::	else;
			fi;
		::	level == medium;
		fi;
	od;
}
active proctype methanecycle(){
	do
	::	methane = true;
	::	methane = false;
	od;
}
active proctype methanesensor(){
	do
	::	atomic {
			cMethane?_;
			if
			::	methane;
				cMethane!methanestop;
			::	!methane;
				cMethane!ready;
			fi;
		};
	od;
}
active proctype watersensor(){
	do
	::	atomic {
			if
			::	waterLevel == low;
				if
				::	waterLevel = low;
				::	waterLevel = medium;
				fi;
			::	waterLevel == medium;
				if
				::	waterLevel = low;
				::	waterLevel = medium;
				::	waterLevel = high;
				fi;
			::	waterLevel == high;
				if
				::	waterLevel = medium;
				::	waterLevel = high;
				fi;
			fi;
			cLevel!waterLevel;
		};
	od;
}
