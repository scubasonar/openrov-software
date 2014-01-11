#include <limits.h>

#include "Device.h"
#include "Pin.h"
#include "GeigerCounter.h"
#include "Settings.h"

static Pin geigerPin("geiger", GEIGER_COUNTER_PIN, true, true);

// CPM to uSv/h conversion factor
float conversionCoefficient = 0;
int count = 0; // geiger counter particle hit counter
long lastUpdateTime = 0; // last time we updated ROV sensor value



// The time to update ROV sensor value
unsigned long nextUpdateMillis = 0;

void trigger()
{
  count++;
}
 
 // Calculate elapsed time. this takes into account rollover.
unsigned long elapsedTime(unsigned long startTime) {
  unsigned long stopTime = millis();

  if (startTime >= stopTime) {
    return startTime - stopTime;
  }
  else {
    return (ULONG_MAX - (startTime - stopTime));
  }
}

void GeigerCounter::device_setup(){
  Settings::capability_bitarray |= (1 << RADIATION_CAPABLE);
  
// Set the conversion coefficient from cpm to �Sv/h
  switch (TUBE_MODEL) {
  case LND_712:
    // Reference:
    // http://einstlab.web.fc2.com/geiger/geiger3.html
    conversionCoefficient = 0.00833;
    break;
  case SBM_20:
    // Reference:
    // http://www.libelium.com/wireless_sensor_networks_to_control_radiation_levels_geiger_counters
    conversionCoefficient = 0.0057;
    break;
  case J408Gamma:
    // Reference:
    // http://garden.seeedstudio.com/index.php?title=Geiger_Counter
    //
    // 300CPS = 0.0084Gy/s
    // 18,000CPM = 30.24Gy/h
    // 1CPM = 0.00168Gy/h
    conversionCoefficient = 0.00168;
    break;
  case J306Beta:
    // Reference:
    // http://garden.seeedstudio.com/index.php?title=Geiger_Counter
    //
    // 300CPS = 0.0084Gy/s
    // 18,000CPM = 30.24Gy/h
    // 1CPM = 0.00168Gy/h
    conversionCoefficient = 0.00168;
    break;
  default:
    break;
  }
  
  attachInterrupt(GEIGER_COUNTER_PIN, trigger, INTERRUPT_MODE);

  unsigned long now = millis();
  nextUpdateMillis = now + updateIntervalInMillis;
//  light.write(0);
}


void GeigerCounter::device_loop(Command command){
  if (elapsedTime(lastUpdateTime) < updateIntervalInMillis) {
    return;
  }
  
  float countsPerMinute = (float)count / (float)updateIntervalInMillis/60000;
    // Convert from cpm to µSv/h with the pre-defined coefficient
  float microSievertPerHour = countsPerMinute * conversionCoefficient;
  count = 0;
  
  envdata::RAD = microSievertPerHour;
}






