#ifndef SYSTEMS_CHECK
#define SYSTEMS_CHECK

#include "machine.h"

const int CAN_MS_THRESHOLD = 100; // msec

static volatile bool rtd_brake_fault(const iCANflex& Car); 

static volatile bool critical_sys_fault(const iCANflex& Car);
static volatile void warn_sys_fault(iCANflex& Car);


// CAN RECIEVE FAILURES
static volatile bool critical_can_failure(const iCANflex& Car); 
static volatile bool warn_can_failure(const iCANflex& Car);

// read bspd, ams, and imd pins as analog
// .5v is shit -  ADC: 155
// 3v when almost ok - ADC: 930
// 2.4v is ok - ADC: 744
// 1v = 310
static volatile bool AMS_fault();
static volatile bool IMD_fault();
static volatile bool BSPD_fault();

#endif
