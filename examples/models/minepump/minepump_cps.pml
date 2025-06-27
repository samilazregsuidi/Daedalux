/*
======================================
    LTL PROPERTIES FOR VERIFICATION
======================================

P1. Security: The pump must never be on when methane is present
    [] (methane -> !pumpOn)

P2. Reactivity: If water is high and there is no methane, the pump should eventually turn on
    [] ((high && !methane) -> <> pumpOn)

P3. Safety shut-off: If water is low or methane appears, the pump must eventually turn off
    [] ((h <= 0.2 || m >= 1.0) -> <> !pumpOn)

P4. Liveness: The pump does not remain indefinitely off when water is high and no methane
    [] ((h >= 2.0 && m < 1.0) -> <> pumpOn)

P5. Avoid deadlock: The system always evolves and doesn’t freeze
    []<>(true)

P6. No indefinite operation with danger: We never reach a state where pump is on and methane is high forever
    !<>[] (pumpOn && m >= 1.0)
*/

#define readAlarm (controller.readMsg == alarmMsg)
#define readLevel (controller.readMsg == levelMsg)

#define stateMethanestop (controller.pstate == methanestop)
#define stateLowstop (controller.pstate == lowstop)

#define highWater (watersensor.waterLevel == high)
#define mediumWater (watersensor.waterLevel == medium)
#define lowWater (watersensor.waterLevel == low)


// États physiques globaux
real h = 1.0;             // Niveau d’eau (m)
real Q_in = 0.1;          // Débit d’entrée d’eau (m/min)
real Q_pump = 0.3;        // Débit pompe (m/min)

real m = 0.0;             // Concentration méthane (%)
real R_in = 0.05;         // Taux d’accumulation de méthane (%/min)
real R_out = 0.02;        // Taux de dissipation (%/min)

bool pumpOn = false;

// Canaux de communication
chan cLevel = [0] of {mtype};  // waterLevel: low, medium, high
chan cMethane = [0] of {bool}; // alarme méthane

mtype = {low, medium, high, levelMsg, alarmMsg, running, stopped, methanestop, lowstop};

// ----- Water Sensor (intègre h' et envoie état discret) -----
active proctype WaterSensor() {
    mtype level;
    do ::
        h'= Q_in - (pumpOn ? Q_pump : 0.0);

        if
        :: h >= 2.0 -> level = high;
        :: h <= 0.2 -> level = low;
        :: else     -> level = medium;
        fi;

        cLevel!level;
    od;
}

// ----- Methane Sensor (intègre m' et envoie alarme booléenne) -----
active proctype MethaneSensor() {
    bool alarm;
    do ::
        m' = R_in - R_out;

        alarm = (m >= 1.0);
        cMethane!alarm;
    od;
}

// ----- Contrôleur (lit les capteurs et commande la pompe) -----
active proctype Controller() {
	mtype readMsg = levelMsg
	mtype pstate = ready;
    mtype level = medium;
    bool alarm = false

    do :: cLevel?level;
    	readMsg = levelMsg;
    	if
        :: level == high && (pstate == ready  ||  pstate == lowstop) ->
        					atomic {
        								pstate = running;
        								pumpOn = true;
        							};
        	
        :: level == low -> 
        					atomic {
										pstate == running;
										pumpOn = false;
										pstate = lowstop;
									};
        :: else -> skip;
        fi;
        
       :: cMethane?alarm;
       	readMsg = methaneMsg
        if
        :: alarm -> 
        					atomic {
										pstate == running;
										pumpOn = false;
										pstate = methanestop;
									};
							
        :: !alarm ->
        					atomic {
        								pstate == methanestop;
        								pumpOn = true;
        								pstate = running
        					       };
        fi;
    od;
}

