/*
  RV_Ultrasonic.cpp - Custom library from Ultrasonic.cpp
  Ultrasonic.h - Library for HR-SC04 Ultrasonic Ranging Module.
  Created by ITead studio. Alex, Apr 20, 2010.
  iteadstudio.com
  
  TP --> Trigger Pin
  EP --> Echo_pin
*/
#ifndef RV_Ultrasonic_h
#define RV_Ultrasonic_h

#include "Arduino.h"

#define CMS 1
#define INCH 0

class RV_Ultrasonic
{
  public:
    RV_Ultrasonic(int TP, int EP);
    long Timing();
    long RangingInCM();
	long RangingInInch();
	void ResetEchoPin();
	
    private:
		int Trig_pin;
		int Echo_pin;
		long  duration, distance_cm, distance_inc;
		int RV_UltrasonicCounter;
    
};

#endif