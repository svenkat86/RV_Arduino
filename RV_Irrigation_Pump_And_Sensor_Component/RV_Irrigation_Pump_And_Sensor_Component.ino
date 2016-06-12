#include <RV_Ultrasonic.h>
#include <VirtualWire.h>

/*
@RvSystemz  -  This file will be uploaded on the reciever end of Arduino UNO.
Module to be made available in the recieving end to start and stop pump
VirtualWire to be used to send & receive 433mhz radio waves
HC-SR04 Ping distance sensor.

VCC to arduino 5v GND to arduino GND
Echo to Arduino pin xx Trig to Arduino pin yy
*/
//Receive pin for the  433mhz
const int receive_pin = 11;

//Transmit Pin
const int transmit_pin = 12;

//Analog Pin for controlling pump
const int pump_ctrl_pin3 = 13;
const int pump_ctrl_pin2 = 2;

boolean pumpRunning = false;
unsigned long pumpStartTime = 0;
unsigned long currentMillis = 0;

//Ultrasonic - Initialization
const int echoPin = 6;
const int trigPin = 8;
RV_Ultrasonic rv_ultrasonic(trigPin ,echoPin);

unsigned long waterLevel = 0;
unsigned long prevLvl = 0;

//signal Led Pin
const int ylw_lpin = 14;
const int red_lpin = 9;
const int grn_lpin = 19;
const int blu_lpin = 1;

//RefillMode Switch
const int refill_switch = 5;

//Invesre - 13cm is distance bw sensor (tub brim) and water level 
//0cm is the tank full
const int lowLvlTreshold = 13;
const int topLvlTreshold = 0;

void setup() {
    delay(1000);
    //Serial.begin(9600);  
    //Serial.println("setup");

    //Pump control
    pinMode(pump_ctrl_pin3, OUTPUT);
    pinMode(pump_ctrl_pin2, OUTPUT);

    //433mhz initialize
    initializeTransmitterAndReceiver();
    
    //UltrasonicInitialize 
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    waterLevel = getDistanceUsingUltrasonic();
    delay(1000);
    waterLevel = getDistanceUsingUltrasonic();
    //Serial.println(waterLevel);

    //ledPin - output initialization
    pinMode(ylw_lpin, OUTPUT);
    blinkL(ylw_lpin, 100 , 5);
    
    pinMode(red_lpin, OUTPUT);
    blinkL(red_lpin , 100 , 5);

    pinMode(grn_lpin, OUTPUT);
    blinkL(grn_lpin ,100 , 5);

    pinMode(blu_lpin, OUTPUT);
    blinkL(blu_lpin ,100 , 5);
    
    //Refill Mode Switch - configure refill_switch as an input and enable the internal pull-up resistor
    pinMode(refill_switch, INPUT_PULLUP);
    
}

void startPumpTimer(){
  pumpStartTime = millis();
}

void startPump(){
    if(!pumpRunning){
      //By changing values from 0 to 255 you can control motor speed
      analogWrite(pump_ctrl_pin3, 255); 
      digitalWrite(pump_ctrl_pin2, HIGH); 
      pumpRunning = true;
      startPumpTimer();
    }
}

/*
 * Monitor the pump if its running and stop after
 * certain time.
 */
void monitorPumpTime(){
  if(pumpRunning){
        currentMillis = millis();
        unsigned long pumpRunningFor = (currentMillis-pumpStartTime);
        //Pump will shutdown after 5 minutes
        if(pumpRunningFor > 300000){
          panicButton();
        }
    }
}

void stopPump(){
  // By changing values from 0 to 255 you can control motor speed
  digitalWrite(pump_ctrl_pin3, 0); 
  digitalWrite(pump_ctrl_pin2, 0); 
  pumpRunning = false;
}

/*This will be like reset switch where if the motor runs for more than 
certain persiod it will shutdown unless manually stopped and started*/
void panicButton(){
  stopPump(); 
}

void blinkL(int p, int dly, int tm){
  for(int i=0; i<tm; i++){
    digitalWrite(p, HIGH);  
    delay(dly);              
    digitalWrite(p, LOW);    
    delay(dly); 
  }
}

void maintenanceMode(int c){
    if(c == 1){
      blinkL(blu_lpin , 100 , 3);
      digitalWrite(blu_lpin, HIGH);  
    }
    else{
      digitalWrite(blu_lpin, LOW);  
    }
}

//Sent from pmp end
String pmpLvlMsg = "V000R";

void loop() {
    
   int sensorVal = digitalRead(refill_switch);       
            
   if(sensorVal == LOW || waterLevel > lowLvlTreshold){
        //If On- dont recieve - just keep sending
        // Or if waterLvel is 13 -its empty
        stopPump(); 
        maintenanceMode(1);
   }
   else{
        maintenanceMode(0);
        
        //No connection faulty is Open - HIGH - so always else.
        //My Pattern RXV - X can be 0 or 1
        // 0 - is off condition ; 1 - is on condition
        String receivedCode = getStringValueFromReceiver();
        receivedCode.trim();
    
        if(receivedCode.length() == 0){
          for(int i =0;i< 6; i++){
              receivedCode = getStringValueFromReceiver();
              if(receivedCode.length() > 1){
                break;
              }
              delay(250);
          }
        }
    
       //Serial.print("ReceivedCode ");
        //Serial.println(receivedCode);
        
        //Accepting value is 'If block', else if is used for sending - avoid looping
        if(receivedCode.startsWith("R") && receivedCode.endsWith("V")){
            //Receiving End 
            String power = receivedCode.substring(1 , 2);
            if(power == "1"){
                blinkL(grn_lpin , 50 , 4);
                startPump();
            }
            else{
                stopPump();
                blinkL(red_lpin , 50 , 4);
            }
    
        }
        else if(receivedCode.startsWith("V") && receivedCode.endsWith("R")){
            //Transmitting values
        }
        else{
            //Transmitter down - so start clock
            blinkL(blu_lpin, 100 , 3);
        }
        monitorPumpTime();
    }

    waterLevel = getDistanceUsingUltrasonic();
      
    if(prevLvl != waterLevel){
        prevLvl = waterLevel;
        pmpLvlMsg = "";
        
        //Message Send  - VXYZR --- other places we can use from 4th char
        pmpLvlMsg.concat("V");
        if(waterLevel < 10){
          pmpLvlMsg.concat("00");
          pmpLvlMsg.concat(waterLevel);
        }
        else if(waterLevel > 9 && waterLevel < 100){
          pmpLvlMsg.concat("0");
          pmpLvlMsg.concat(waterLevel);
        }
        else if(waterLevel > 99 && waterLevel < 1000){
          pmpLvlMsg.concat(waterLevel);
        }
        else{
          pmpLvlMsg.concat("000");
        }
        pmpLvlMsg.concat("R");
    }
         
    //Serial.print("Broadcasting ");
    //Serial.println(pmpLvlMsg);
    blinkL(ylw_lpin, 50 , 2);
    sendMessageOverTransmitter(pmpLvlMsg);
    
    delay(500);
}

/*
 * Ultrasonic Method to initalize 
 * and sends back the distance
 */
long getDistanceUsingUltrasonic(){
      long currlevel = rv_ultrasonic.RangingInCM();
      return currlevel;
}

/*
 * Method will send message over transmitter
 */
void sendMessageOverTransmitter(String msgToSend){
    // Length (with one extra character for the null terminator)
    int str_len = msgToSend.length() + 1;
    char msg[str_len];
    msgToSend.toCharArray(msg, str_len);
    
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
}

/*
 * Method will read the value from RF reciever and returns the string.
 * Sometimes due to noise, empty or blank string are also returned.
 */
String getStringValueFromReceiver(){
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    String retStr = "";
    if (vw_have_message() && vw_get_message(buf, &buflen)) // Non-blocking
    {
        int i;
        for (i = 0; i < buflen; i++)
        {
           retStr += char(buf[i]);
        }
    }
    return retStr;
}


void initializeTransmitterAndReceiver(){
   //Initialising the IO and ISR for Reciever
    vw_set_rx_pin(receive_pin);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(1200);  // Bits per sec
    vw_rx_start();       // Start the receiver PLL running

    //Initialize Transmitter
    vw_set_tx_pin(transmit_pin);
    vw_set_ptt_inverted(true); //Required for DR3100
    //vw_setup(2000);   //Bits per sec
    
}











