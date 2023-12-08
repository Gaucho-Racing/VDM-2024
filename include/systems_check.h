#include "main.h"

//BSE and APPS check for input at startup ONLY
static volatile bool ECU_Startup_Rejection(iCANflex& Car) {
    if(Car.PEDALS.getAPPS1() > 0.05 && Car.PEDALS.getAPPS2() > 0.05) {
        return true;
        // send error code to dash
    }   
    if(Car.PEDALS.getBrakePressureF() > 0.05 || Car.PEDALS.getBrakePressureR() > 0.05) {
        return true;
        // send error code to dash
    }


    Serial.println("ECU REJECTED STARTUP");
}

static volatile bool Critical_Systems_Fault(iCANflex& Car) {
    if("VCU CAN BUS RECEIVER FAILURE not sure how to check"){ 
        return true;
    }
    if (Car.ACU.getMaxCellTemp() <= Car.ACU.getCellTemp_n()){
        return true;
    }
    if (Car.Inverter.getThrottleIn()  < 0.05){
        return true;
    }
    if(Car.Inverter.getMotorTemp() >= Car.Inverter.getMotorTempLim()){
        return true;
    }
    if("CURRENT LIMIT"){
        return true;
    }
    if("FALUT"){
        return true;
    }
    if("CRASH"){
        return true;
    }
    Serial.println("CRITICAL SYSTEMS FAULT");
    return false; //implement later
}

static volatile void Warning_Systems_Fault(iCANflex& Car) {
    Serial.println("NON CRITICAL ERROR CODES");
}