#include "machine.h"
#include "sstream"

volatile State state;
volatile Mode mode;
bool (*errorCheck)(const iCANflex& Car); 
bool BSE_APPS_violation = false;

State sendToError(bool (*erFunc)(const iCANflex& Car)) {
   errorCheck = erFunc; 
   return ERROR;
}


// testing purposes for printing state and mode
// -------------------------------------------------------------------------------
static unordered_map<State, string> state_to_string = {
    {ECU_FLASH, "ECU_FLASH"},
    {GLV_ON, "GLV_ON"},
    {TS_PRECHARGE, "TS_PRECHARGE"},
    {PRECHARGING, "PRECHARGING"},
    {PRECHARGE_COMPLETE, "PRECHARGE_COMPLETE"},
    {DRIVE_NULL, "DRIVE_NULL"},
    {DRIVE_TORQUE, "DRIVE_TORQUE"},
    {DRIVE_REGEN, "DRIVE_REGEN"},
    {ERROR, "ERROR"}
};

// static unordered_map<Mode, string> mode_to_string = {
//     {TESTING, "TESTING"},
//     {LAUNCH, "LAUNCH"},
//     {ENDURANCE, "ENDURANCE"},
//     {AUTOX, "AUTOX"},
//     {SKIDPAD, "SKIDPAD"},
//     {ACC, "ACC"},
//     {PIT, "PIT"}
// };  

// -------------------------------------------------------------------------------

void loop(){
    // reads bspd, ams, and imd pins as analog   
    SystemsCheck::hardware_system_critical(*Car);
    SystemsCheck::system_faults(*Car);
    SystemsCheck::system_limits(*Car);
    SystemsCheck::system_warnings(*Car);

    SEND_SYS_CHECK_FRAMES();
    
    state = active_faults.size() ?  sendToError(*active_faults.begin()) : state;


    digitalWrite(SOFTWARE_OK_CONTROL_PIN, (state == ERROR) ? LOW : HIGH);

    // read in settings from Steering Wheel
    THROTTLE_MAPPING = 0; 
    REGEN_LEVEL = 0;

    PWR_LEVEL = 0;
    PWR_LEVEL = active_limits.size() ? 0 : PWR_LEVEL; // limit power in overheat conditions

    TC_LEVEL = 0;
    
    mode = ENDURANCE;


    State currentState = state;
    Serial.print(state_to_string.find(currentState)->second.c_str());
    Serial.print(" | ");
    Serial.println(/*mode_to_string.find(mode)->second.c_str()*/ " ");

    // STATE MACHINE OPERATION
    switch (state) {
        // ERROR
        case ERROR:
            state = error(*Car, errorCheck);
            break;

        // STARTUP 
        case ECU_FLASH:
            state = ecu_flash(*Car);
            break;
        case GLV_ON:
            state = glv_on(*Car);
            break;
     
        // PRECHARGE
        case TS_PRECHARGE:
            state = ts_precharge(*Car);
            break;
        case PRECHARGING:
            state = precharging(*Car);
            break;
        case PRECHARGE_COMPLETE:
            state = precharge_complete(*Car);
            break;
        
        // DRIVE
        case DRIVE_NULL:
            state = drive_null(*Car, BSE_APPS_violation, mode); 
            break;
        case DRIVE_TORQUE:
            state = drive_torque(*Car, BSE_APPS_violation, mode);
            break;
        case DRIVE_REGEN:
            state = drive_regen(*Car, BSE_APPS_violation, mode);
            break;

        
    }
}



//GLV STARTUP
void setup() {
    Car = new iCANflex();
    Serial.begin(9600);
    Serial.println("Waiting for Serial Port to connect");
    while(!Serial) Serial.println("Waiting for Serial Port to connect");
    Serial.println("Connected to Serial Port 9600");

    Car->begin();

    // set state  
    state = ECU_FLASH; 
    // state = GLV_ON;
}

