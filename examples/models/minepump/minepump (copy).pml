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
                    
chan cCmd = [0] of {mtype}; 	/* stop, start			*/
chan cAlarm = [0] of {mtype}; 	/* alarm                */
chan cMethane = [0] of {mtype}; /* methanestop, ready   */
chan cLevel = [0] of {mtype}; 	/* low, medium, high    */

active proctype controller() {
	mtype pstate = stopped; 		/* ready, running, stopped, methanestop, lowstop */
	mtype readMsg = commandMsg; 		/* commandMsg, alarmMsg, levelMsg */
	mtype pcommand = start;
	mtype level = medium;
	
	bool pumpOn = false;
	
	do	::	atomic {
				cCmd?pcommand;
				readMsg = commandMsg; 
			};
			if	::	pcommand == stop;
					if	::	f.Stop;
							if	::	atomic {
										pstate == running;
										pumpOn = false;
									}
								::	else
								fi;
							pstate = stopped;
						::	else
						fi;
				::	pcommand == start;
					if	::	f.Start;
							if	::	atomic {
										pstate != running;
										pstate = ready;
									};
								::	else
								fi;
						::	else
						fi;
				fi;
			cCmd!pstate;
			
		::	atomic { 
				cAlarm?_;
				readMsg = alarmMsg;
			};
			if	::	f.MethaneAlarm;
					if	::	atomic {
								pstate == running;
								pumpOn = false;
							};
						::	else
						fi;
					pstate = methanestop;
						
				::	else
				fi;
			
		::	atomic { 
				cLevel?level;
				readMsg = levelMsg;
			};
			if	::	level == high;
					if	::	f.High;
							/* The same block with and without race condition.
							   First, without race condition: */
							if	::	pstate == ready  ||  pstate == lowstop;
									if	::	f.MethaneQuery;
											skip;
											atomic {
												cMethane!pstate;
												cMethane?pstate;
												if	::	pstate == ready;
														pstate = running;
														pumpOn = true;
													::	else
												fi;
											};
										::	else;
											atomic {
												pstate = running;
												pumpOn = true;
											};
										fi;
								::	else
								fi;
						::	else
						fi;
				::	level == low;
					if	::	f.Low;
							if	::	atomic {
										pstate == running;
										pumpOn = false;
										pstate = lowstop;
									};
								::	else
								fi;
						::	else
						fi;
				::	level == medium;
					skip;
				fi;
		od;
}

bool methane = false;

active proctype user() {
	mtype uwants = stop; 			/* what the user wants */
	do	::	if	::	uwants = start;
				::	uwants = stop;
				fi;
			cCmd!uwants;
			cCmd?_;			/* Sends back the state; ignore it */
		od;
}

active proctype methanealarm() {
	do	:: 	methane = true;
			cAlarm!alarm;
		::	methane = false;
		od;
}

active proctype methanesensor() {
	
	do	:: 	atomic {
				cMethane?_;
				if	::	methane;
						cMethane!methanestop;
					::	!methane;
						cMethane!ready;
					fi;
			};
		od;
}

active proctype watersensor() {
	mtype waterLevel = medium;
	do	:: 	atomic {
				if	::	waterLevel == low ->
						if	:: waterLevel = low;
							:: waterLevel = medium;
							fi;
					::	waterLevel == medium ->
						if	:: waterLevel = low;
							:: waterLevel = medium;
							:: waterLevel = high;
							fi;
					::	waterLevel == high ->
						if	:: waterLevel = medium;
							:: waterLevel = high;
							fi;
					fi;
				cLevel!waterLevel;
			};
		od;
};
