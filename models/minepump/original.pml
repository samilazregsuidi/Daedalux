#define readCommand (controller.readMsg == commandMsg)
#define readAlarm (controller.readMsg == alarmMsg)
#define readLevel (controller.readMsg == levelMsg)
#define userStart (user.uwants == start)
#define userStop (user.uwants == stop)
#define highWater (watersensor.waterLevel == high)
#define mediumWater (watersensor.waterLevel == medium)
#define lowWater (watersensor.waterLevel == low)
#define stateReady (controller.pstate == ready)
#define stateRunning (controller.pstate == running)
#define stateStopped (controller.pstate == stopped)
#define stateMethanestop (controller.pstate == methanestop)
#define stateLowstop (controller.pstate == lowstop)

mtype = {/*1*/	levelMsg, 
	/*2*/	stop, 
	/*3*/	methanestop, 
	/*4*/	alarm, 
	/*5*/	running, 
	/*6*/	commandMsg, 
	/*7*/	start, 
	/*8*/	alarmMsg, 
	/*9*/	high, 
	/*10*/	medium, 
	/*11*/	low, 
	/*12*/	stopped, 
	/*13*/	ready, 
	/*14*/	lowstop}

chan cCmd = [0] of {mtype};
chan cAlarm = [0] of {mtype};
chan cMethane = [0] of {mtype};
chan cLevel = [0] of {mtype};

bool pumpOn = false;

active proctype controller(){
	mtype pcommand = start;
	mtype level = medium;
	
	mtype pstate = stopped;
	mtype readMsg = commandMsg;
	
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
						skip;
					fi;
				};
			::	else;
				skip;
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
		::	skip;
			
		fi;
	od;
}
active proctype user(){
	mtype uwants = stop;
	do
	::	if
		::	uwants = start;
		::	uwants = stop;
		fi;
		cCmd!uwants;
		cCmd?_;
	od;
}

bool methane = false;

active proctype methanealarm(){
	do
	::	methane = true;
		cAlarm!alarm;
		
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
	mtype waterLevel = medium;

	do
	::	atomic {
			if
			::	waterLevel == low;
				if
				::	skip
				::	waterLevel = medium;
				fi;
			::	waterLevel == medium;
				if
				::	waterLevel = low;
				::	skip
				::	waterLevel = high;
				fi;
			::	waterLevel == high;
				if
				::	waterLevel = medium;
				::	skip
				fi;
			fi;
			cLevel!waterLevel;
		};
	od;
}
