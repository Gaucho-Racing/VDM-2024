#include <Arduino.h>
#include <imxrt.h>
#include <cstdint>
#include "main.h"
#include "constants.h"
#include "iCANflex.h"
#include "machine.h"



volatile State state;
volatile State prevState;
volatile bool (*errorCheck)(iCANflex& Car); 
bool BSE_APPS_violation = false;

State sendToError(volatile State currentState, volatile bool (*erFunc)(iCANflex& Car)) {
   errorCheck = erFunc; 
   prevState = currentState; 
   return ERROR;
}

// Error functions
void handle_ISR_VCU_CAN_BUS(iCANflex& Car) {
    // handle this error
    if("VCU CAN BUS RECEIVER FAILURE not sure how to check"){ // "..." := filler quotes, not code
        // log some kind of critical error
        // do sth
    }
}

void handle_ISR_CELL_TEMP(iCANflex& Car) {
    if (Car.ACU.getMaxCellTemp() <= Car.ACU.getCellTemp_n()){
        // log some kind of critical error
        // shut down
    }
}

void handle_ISR_THROTTLE_SIGNAL(iCANflex& Car) {
    if(Car.Inverter.getThrottleIn()  < 0.05){
        // handle this error
    }
    return;
}

void handle_ISR_MOTOR_TEMP(iCANflex& Car) {
    if(Car.Inverter.getMotorTemp() >= Car.Inverter.getMotorTempLim())
    // handle this error
    return;
}

void handle_ISR_CURRENT_LIMIT(iCANflex& Car) {
    if("CURRENT_LIMIT"){
        // handle this error
    }
    return;
}

void handle_ISR_IMD_FAULT(iCANflex& Car) {
        if("IMD_FAULT"){

    return;
}

void handle_ISR_CRASHED(iCANflex& Car) {
    // handle this error
    return;
}

void loop(){
    switch (state) {
        case OFF:
            state = off(Car, switches);
            break;
        case ON:
            if ((state = on(Car, switches)) == ERROR || statusRegister > 0) sendToError(ON, &ECU_Startup_Rejection);
            break;
        case DRIVE_READY:
            state = drive_ready(Car, switches, BSE_APPS_violation); 
            break;
        case DRIVE:
            state = drive(Car, switches, BSE_APPS_violation);
            break;
        case ERROR:
            state = error(Car, switches, prevState, errorCheck);
            break;
        // case TESTING;
    }

    //test
}
// attach ISR and enable NVIC for ISR error
void attachErrorISR(int errorNumber, volatile bool (*erFunc)(iCANflex& Car), uint8_t priority) {
    attachInterruptVector(errorNumber, erFunc);
    NVIC_SET_PRIORITY(errorNumber, priority);
    NVIC_ENABLE_IRQ(errorNumber);
}

void setup() {
    Serial.begin(9600);
    Car.begin();


    // set all the switchboard pins to  digital read inputs



    // set state  
    state = OFF; 

    // Attach ISR for errors (currently not "prioritized")
    attachErrorISR(100, &handle_ISR_VCU_CAN_BUS, 1); 
    attachErrorISR(102, &handle_ISR_CELL_TEMP, 2); 
    // attachErrorISR(103, &ISR_CURRENT_TO_DTI_MC, 3);  --> not sure to include?
    attachErrorISR(104, &handle_ISR_THROTTLE_SIGNAL, 4); 
    attachErrorISR(105, &handle_ISR_MOTOR_TEMP, 5); 
    attachErrorISR(106, &handle_ISR_CURRENT_LIMIT, 6); 
    attachErrorISR(107, &handle_ISR_IMD_FAULT, 7); 
    attachErrorISR(108, &handle_ISR_CRASHED, 8); 
}

