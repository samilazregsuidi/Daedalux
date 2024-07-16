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
chan cCmd = [0] of {mtype};
mtype pstate = stopped;
mtype readMsg = commandMsg;
bool pumpOn = false;
mtype uwants = stop;
active proctype controller(){
	mtype pcommand = start;
	mtype level = medium;
	do
	::	atomic {
			cCmd?pcommand;
			readMsg = commandMsg;
		};
		
		if
		::	pcommand == stop;
			if
			::	atomic {
					pstate == running;
					pumpOn = false;
				};
			::	else;
			fi;
			pstate = stopped;
			
		::	pcommand == start;
			if
			::	atomic {
					pstate != running;
					pstate = ready;
				};
			::	else;
			fi;
		fi;
		cCmd!pstate; 
	od;
}
active proctype user(){
	do
	::	if
		::	uwants = start;
		::	uwants = stop;
		fi;
		cCmd!uwants;
		cCmd?_;
	od;
}
