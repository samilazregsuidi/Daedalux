typedef features {
    bool Alarm;
}

bool safe = true;
bool danger, stopped = false;

active proctype Motor() {
    do
    :: safe ->
        if :: skip;
           :: safe = false; danger = true;
        fi;
    
    :: danger ->
        if :: Alarm -> danger = false; 
                       stopped = true;
           :: else -> skip; 
        fi;
    
    :: stopped
        if :: skip;
           :: stopped = false; 
              safe = true; 
        fi; 
    od
 }
