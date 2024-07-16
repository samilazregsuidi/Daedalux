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
chan cAlarm = [0] of {mtype};
mtype pstate = stopped;
mtype readMsg = commandMsg;
bool pumpOn = false;
bool methane = false;
active proctype controller(){
	mtype pcommand = start;
	mtype level = medium;
	do
	::	atomic {
			cAlarm?_;
			readMsg = alarmMsg;
		};
		if
		::	atomic {
				pstate == running;
				pumpOn = false;
			};
		::	else;
		fi;
		pstate = methanestop;
	od;
}
active proctype methanealarm(){
	do
	::	methane = true;
		cAlarm!alarm;
	::	methane = false;
	od;
}
