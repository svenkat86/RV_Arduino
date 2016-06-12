void renderServer(){
  String HTTP_req;
  EthernetClient cl = server.available();
 
  String rStr; 
  
  if (cl) {
    //Serial.println("new client");
    
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (cl.connected()) {
      if (cl.available()) {
        char c = cl.read();
        Serial.write(c);

        if (rStr.length() < 100) {
          rStr += c; 
        }
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        
        if (c == '\n' && currentLineIsBlank) {

              cl.println("HTTP/1.1 200 OK");
              cl.println("Content-Type: text/html");
              cl.println("Connection: close");  
              cl.println();
                  
              if(rStr.indexOf("?") >= 0)
              {         
                 if(rStr.indexOf("wlvl=2") >0) {
                    populWLvl();
                    cl.print("{\"wlvl\":\"");
                    cl.print(avgWLvl);
                    blinkL(ylw_lpin, 50 , 4);
                }
                else if(rStr.indexOf("pmp=1") >0) 
                {
                    cl.print("{\"cpmp\":\"");
                    cpmp = 1;
                    cl.print(cpmp);
                }
                else if(rStr.indexOf("pmp=0") >0)
                {
                    cl.print("{\"cpmp\":\"");
                    cpmp = 0;
                    cl.print(cpmp);
                }
                else if (rStr.indexOf("cpmp=2") >0){
                    cl.print("{\"cpmp\":\"");
                    cl.print(cpmp);
                }
                //cpmp - Retrieves the pump status
                //2 is always get, 1 and 0 is On and OFF
                
                cl.print("\"}");
                break; 
                  
              }
              else{
                  
                  cl.println("<!DOCTYPE HTML>");
      
                  webFile = SD.open("1.htm");
                  if (webFile) {
                       while(webFile.available()) {
                           cl.write(webFile.read());
                       }
                       webFile.close();
                  }
                  else{
                       cl.println("404");
                  }
                  
                  break;  
              }
          
        }
        /*if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }*/
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    cl.stop();
    //clearing string for next read
    rStr="";
    
    //Serial.println("Discone");
  }
}





