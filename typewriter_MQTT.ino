I////////TYPEWRITER TESTING//////////  

#include <Wire.h>  // Comes with Arduino IDE
#include <Shifter.h>
#include "SPI.h"
#include "WiFly.h"
#include "PubSubClient.h"
#include <stdlib.h> // for the atol() function
  
#define SER_Pin 4 //SER_IN
#define RCLK_Pin 2 //L_CLOCK
#define SRCLK_Pin 3 //CLOCK
#define NUM_REGISTERS 5 //how many registers are in the chain
    

Shifter shifter(SER_Pin, RCLK_Pin, SRCLK_Pin, NUM_REGISTERS); 
//broker settings
  byte server[] = { 146,185,168,127 }; // replace with the IP address of your broker - swap . for ,
  int port = 1883; //default port is 1883. To help get around network restrictions,you may need broker set as port 80
  char subscribedChannelin[] = "nodeinput2";
    char subscribedChanneltype[] = "typed";
       // char subscribedChannelwearetyping[] = "donttype";
                char subscribedChannelcharcount[] = "charcount";
                                char subscribedChannelwetype[] = "wetype";

                                char charReset[] = "charReset";




boolean typed = false;

////////VARIABLES//////////  

  char deviceName[] = "typewriter"; // set a unique name for each device connected to the broker
  //String payloadString = "baa baa black sheep";
  int charCount = 1;
    int charCount2;
    int charCombo = charCount + charCount2;
 
 String payloadString;
   int delaytime = random(100, 200);
    boolean caps = false;

 
 ////////MQTT//////////  
 
void convertPayload(byte array[], byte len){
    payloadString = "";
    for (byte i = 0; i < len;){
      char c = array[i++];
      if (c < 0x20 || c > 0x7e){
       Serial.print('.');
       payloadString += '.';
      }
      else {
        payloadString += char(c);
      }
    }
  }
  
  
    void join() {
      
//connect to network

    WiFly.begin();
    if (!WiFly.join("Joe", "Diamond.888")) { //use if network has passphrase
    //if (!WiFly.join("ssid")) { // use id it doesn't
      Serial.println("Association failed.");
      
    }else{
      Serial.println("Joining");
      Serial.println(WiFly.ip());
    }    
      
    }
    
     WiFlyClient wifiClient;
 PubSubClient client(server, port, callback, wifiClient);
  
  //////////////BROKER CONNECTION//////////////
  
  void connectToBroker(){
      if (client.connect(deviceName)) {
        Serial.println("Connecting to broker");
       client.subscribe(subscribedChannelin);
      //   client.subscribe(subscribedChannelwearetyping);
             client.subscribe(subscribedChannelcharcount);
                          client.subscribe(subscribedChannelwetype);

               
  
        Serial.println("Connected to broker");
      } else{
        Serial.println("Connection Error");
      }
    
  }

  void loop(){
    
        if(charCombo == 60) {
   returnCarridge();
   delay(5000);
     client.publish(subscribedChanneltype,"reset"); 

    }
    
  
     //check and maintain the connection to the broker
    if(!client.connected()){
       Serial.println("Connection lost");
       setup();
      // join();
      // connectToBroker();
       
    }else{
      client.loop();  
    

    }
   
  }
  
  
  
  
  
  ////////MAIN FUNCTION/////////// 
  
    void callback(char* topic, byte* payload, unsigned int length) {
      
          if (String(topic) == String(subscribedChannelwetype)){
   // Serial.println(topic);
    //convert the payload to a string, then print it out
    convertPayload(payload, length);
    //Serial.println(payloadString);
     delay(5000);
  }
      
      if (String(topic) == String(subscribedChannelcharcount)){
   // Serial.println(topic);
    //convert the payload to a string, then print it out
    convertPayload(payload, length);
    //Serial.println(payloadString);
   charCount2 = payloadString.toInt();
     
         charCombo ++;
        Serial.println(charCombo);
  }
  
      if (String(topic) == String(subscribedChannelin)){
  
   Serial.println("Message Recieved");
           client.publish(subscribedChanneltype,"1");
        delay(200);
       spaceShift(25); 
       delay(200);
         convertPayload(payload, length);
            Serial.println(payloadString);
         cyclePayload();
         client.publish(subscribedChanneltype,"2"); 
payloadString = "";
     }
    }
    
    
    void cyclePayload() {
      
    char allChars [payloadString.length()];
    //cycle through char
for (int i = 0; i < payloadString.length(); i++) {
      allChars[i] = payloadString.charAt(i);
      
      int t = allChars[i];
       int j = allChars[i];

///////////////////////// character grouping//////////////////
      if (t >= 97 && t <= 122) {
       Serial.println("alphabet ");
                    j = allChars[i] - 97;
                    delaytime = random(100, 200);
      }
      
      if (t == 32) {
       Serial.println("space");
                j = allChars[i] - 7;
      }
      if (t == 45) {
       Serial.println("dash");
                j = allChars[i] - 11;
      }
      
        if (t == 46) {
       Serial.println("fullstop");
                j = allChars[i] - 11;
                              delaytime = random(100, 300);

      }
        if (t == 44) {
       Serial.println("comma");
                j = allChars[i] - 10;
      }
        
        
        if (t >= 50 && t <= 57 ) {
       Serial.println("numbers");
              j = allChars[i] - 24;
              delaytime = random(100, 300);
      }
      
         if (t >= 65 && t <= 90 ) {
       Serial.println("Capital Letters");
        caps = true;
              j = allChars[i] - 65;
              delaytime = 1000;
      }
///////////////////////end /////////////////////////

     shiftCycle(j);
     
     ////////realistic type delay///////
     delay(delaytime);
}
      
    }
    
    void spaceShift(int pintwo){
        
      Serial.println("SPACE");
 // shifter.clear(); //set all pins on the shift register chain to LOW
  //shifter.write(); //send changes to the chain and display them  
  
  shifter.setPin(pintwo, HIGH); //set pin 1 in the chain(second pin) HIGH 
  shifter.write(); //send changes to the chain and display them 
  delay(100);
  shifter.clear(); //set all pins on the shift register chain to LOW
  shifter.write(); //send changes to the chain and display them 
  Serial.println(pintwo);
    }
  

  
  //////// SETUP//////////  

  void setup()
  {  
    
    Serial.begin(9600); 
 

    join();
    connectToBroker();
            // client.publish(subscribedChanneltype,"2"); 

////////END///////////

    }
    
  
        //////////////USER INPUT FUNCTION//////////////

    void userInput()  {
      
      
      
    }

     
  
    //////////////SHIFTING FUNCTION//////////////

  void shiftCycle(int pin) {

    
  shifter.clear(); //set all pins on the shift register chain to LOW
  shifter.write(); //send changes to the chain and display them  
  
  shifter.setPin(pin, HIGH); //set pin 1 in the chain(second pin) HIGH 
  shifter.write(); //send changes to the chain and display them 
  delay(100);
  shifter.clear(); //set all pins on the shift register chain to LOW
  shifter.write(); //send changes to the chain and display them 
  
    Serial.println( pin);
    
    charCombo ++;
     Serial.println(charCombo);
     
        if(charCombo == 60) {
   returnCarridge();
   delay(5000);
     client.publish(subscribedChanneltype,"reset"); 

    }
    
  }
  
  
 
  
  void returnCarridge() {
    
 
    /////////CRAZY MOTOR SHIT/////
       Serial.println("Returning the carridge!");
charCombo = 0;

  }
   


