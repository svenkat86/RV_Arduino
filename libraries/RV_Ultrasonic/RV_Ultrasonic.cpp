/*
  RV_Ultrasonic.cpp - Custom library from Ultrasonic.cpp
  --Ultrasonic.cpp - Library for HC-SR04 Ultrasonic Ranging Module.library -ITead studio
  Created by Venkat. - reused from Itead studio
  TP --> Trigger Pin
  EP --> Echo_pin
*/

#include "Arduino.h"
#include "RV_Ultrasonic.h"

RV_Ultrasonic::RV_Ultrasonic(int TP, int EP)
{
   pinMode(TP,OUTPUT);
   pinMode(EP,INPUT);
   Trig_pin=TP;
   Echo_pin=EP;
   RV_UltrasonicCounter = 0;
}

void RV_Ultrasonic::ResetEchoPin()
{
	pinMode(Echo_pin, OUTPUT);
	digitalWrite(Echo_pin, LOW);
	pinMode(Echo_pin, INPUT);
}

long RV_Ultrasonic::Timing()
{
  digitalWrite(Trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_pin, LOW);
  duration = pulseIn(Echo_pin,HIGH);
  
  if(duration == 0){
	  //After 2 times of duration being 0, resetting the echo pin
	  if(RV_UltrasonicCounter == 2){
		ResetEchoPin();
		RV_UltrasonicCounter = 0;
	  }
	  else{
		RV_UltrasonicCounter++;
	  }
  }
  return duration;
}

long RV_Ultrasonic::RangingInCM()
{
	 Timing();
	 distance_cm = duration /29 / 2 ;
	 return distance_cm;
}

long RV_Ultrasonic::RangingInInch()
{
	Timing();
	distance_inc = duration / 74 / 2;
	return distance_inc;
}