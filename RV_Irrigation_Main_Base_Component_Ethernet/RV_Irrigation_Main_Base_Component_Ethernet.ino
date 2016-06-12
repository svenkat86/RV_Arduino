#include <SD.h>
#include <VirtualWire.h>

//Receive pin for the  433mhz
const int receive_pin = 8;

//Transmit Pin
const int transmit_pin = 9;

const int numReadings = 1;
int readings[numReadings];
int currReadIndex = 0;
int total = 0;
int avgWLvl = 0; //Average Water Level

const int ylw_lpin = 5;
const int red_lpin = 6;
const int grn_lpin = 7;

File webFile;

//Current Pump State is stored
//We will use this data across arduino to retrieve the pump status
int cpmp = 0;



void setup(){
   Serial.begin(9600);
   initializeEthernet();
   
   if (!SD.begin(4)) {
      Serial.println("ERROR - SD card initialization failed!");
   }
   else{
      //Serial.println("SD card initialization Success!");
   }
    //Initialise the IO and ISR
    vw_set_rx_pin(receive_pin);
    vw_set_ptt_inverted(true);  // Required for DR3100
    vw_setup(1200);   // Bits per sec
    vw_rx_start();   // Start the receiver PLL running

    vw_set_tx_pin(transmit_pin);
    vw_set_ptt_inverted(true);
     
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings[thisReading] = 0;
    }


    populWLvl();
    
    pinMode(ylw_lpin, OUTPUT);
    blinkL(ylw_lpin, 100 , 5);
    
    pinMode(red_lpin, OUTPUT);
    blinkL(red_lpin , 100 , 5);

    pinMode(grn_lpin, OUTPUT);
    blinkL(grn_lpin ,100 , 5);

    //Serial.print("Everythign Initialized");
}

//Populates WaterLevel average
void populWLvl(){
      String receivedVal = getStrValFromReceiver();
      receivedVal.trim();
      
      Serial.print("ReceivedVal : ");
      Serial.println(receivedVal);
      
       if(receivedVal.startsWith("V") && receivedVal.endsWith("R")){
              String wlvl = receivedVal.substring(1 , 4);
                  
              int a = wlvl.toInt();

              //Take readings and average
              if(numReadings != currReadIndex){
                  total = total + a;
                  currReadIndex = currReadIndex + 1;
              }
              else{
                  avgWLvl = map(total/numReadings, 0, 20, 100, 0);
                  currReadIndex = 0;
                  total = 0;
                  
              }
       }
       /*else{
          avgWLvl = random(10, 99);
       }*/
      
}

unsigned long currentMillis = 0;
unsigned long prevMillis = 0;

unsigned long pumpCurrMillis = 0;
unsigned long pumpPrevMillis = 0;
boolean pumpStrted = false;

void loop() {
        renderServer();
        if(cpmp == 0){
          sendMessageOverTransmitter("R0V");
          blinkL(red_lpin , 50 , 4);
          pumpStrted = false; 
        }
        else{
          sendMessageOverTransmitter("R1V");
          blinkL(grn_lpin , 50 , 4);
          
          if(pumpStrted){
            pumpCurrMillis = millis();
            if((pumpCurrMillis - pumpPrevMillis)/1000 > 600){
                pumpPrevMillis = pumpCurrMillis;
                cpmp = 0;
            }
          }
          else{
             pumpStrted = true;
             pumpCurrMillis = millis();
             pumpPrevMillis = millis();
          }
        }
        

        
        currentMillis = millis();
        if((currentMillis-prevMillis)/1000 > 1){
            populWLvl();
            prevMillis = currentMillis;
        }
        delay(400);
}

void blinkL(int p, int dly, int tm){
  for(int i=0; i<tm; i++){
    digitalWrite(p, HIGH);  
    delay(dly);              
    digitalWrite(p, LOW);    
    delay(dly); 
  }
}


void sendMessageOverTransmitter(String msgToSend){
    // Length (with one extra character for the null terminator)
    int str_len = msgToSend.length() + 1;
    char msg[str_len];
    msgToSend.toCharArray(msg, str_len);
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
}


/*Method will read the value from RF reciever and returns the string.
 * Sometimes due to noise, empty or blank string are also returned
 */
String getStrValFromReceiver(){
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
