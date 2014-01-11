
#ifndef __LIGHTS_H_
#define __LIGHTS_H_
#include <Arduino.h>
#include <limits.h>

#include "Device.h"
#include "Pin.h"

// Geiger counter tube models

#define LND_712   0 // LND
#define SBM_20    1 // GSTube
#define J408Gamma 2 // North Optic
#define J306Beta  3 // North optic
 
#define TUBE_MODEL LND_712
#define INTERRUPT_MODE FALLING

const unsigned long updateIntervalInMillis = 10000; // once every 10 secs should collect enough hits?

class GeigerCounter : public Device {
  
  //  int TUBE_MODEL = LND_712; // Change to match your tube! 
    
    // Interrupt mode:
    // * For most geiger counter modules: FALLING
    // * Geiger Counter Twig by Seeed Studio: RISING
   // int INTERRUPT_MODE = FALLING; 
    
  public:
    // Sampling interval (e.g. 60,000ms = 1min)
    
    GeigerCounter():Device(){};
    void device_setup();
    void device_loop(Command cmd);
};
#endif
