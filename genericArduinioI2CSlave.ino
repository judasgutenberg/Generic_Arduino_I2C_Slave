/* Gus Mueller, April 2 2024
 * sends data to a small Arduino (I use a Mini Pro at 8MHz) from a
 * NodeMCU or other master over I2C
 * 
 */
#include "Wire.h"
#define I2C_SLAVE_ADDR 20
 
volatile int receivedValue = 0;
long lastMasterSignal = 0;
long millisNow = millis();
long dataToSend = -1;
//String debug = "";

void setup(){
  //Serial.begin(115200);
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receieveEvent); 
  Wire.onRequest(requestEvent);
  //Serial.println("Starting up Arduino Slave...");
}

void loop(){
  //millisNow = millis();
  //Serial.println("millis: ");
  //Serial.print(millisNow);
  //Serial.print(" ");
  //Serial.println(debug);
  //debug = "";
  //Serial.println(" ");
  //delay(2000);
}

//send a bytes to the I2C master.  
void requestEvent(){
  Serial.println("request event");
  //i only worry about longs for this to keep it simple
  writeWireLong(dataToSend);
}

void receieveEvent() {
  cli(); 
  lastMasterSignal = millisNow;
  //debug += "receive event";
  byte byteCount = 0;
  byte byteCursor = 0;
  byte receivedValues[4];
  byte receivedByte = 0;
  byte byteRead = 0;
  byte destination = 0;
  receivedValue = 0;
  while(0 < Wire.available()) // loop through all but the last
  {
    byteRead = Wire.read();
    if(byteCount == 0) {
      destination = byteRead;
      //Serial.println(command);
    } else {
      receivedByte = byteRead;
      //Serial.println("got more than a command");
      receivedValues[byteCursor] = receivedByte;
      //Serial.println(receivedByte);
      byteCursor++;
    }
    byteCount++;
  }
  for(byte otherByteCursor = byteCursor; otherByteCursor>0; otherByteCursor--) {
    receivedValue = receivedValue + receivedValues[otherByteCursor - 1] * (1 << ((byteCursor - 1) * 8));
  }
  //debug += "\nDestination: ";
  //debug += destination;
  
  if(byteCursor > 0) { //we had a write
    //Serial.print("; value: ");
    //debug += "XX\n";
    pinMode((int)destination, OUTPUT);
    //debug += "yy\n";
    if(receivedValue > 255 ) {
      analogWrite((int)destination, receivedValue - 256); //if you want to send analog content, add 256 to it first
    } else if (receivedValue == 0 ) {
      //debug += "aOFF\n";
      digitalWrite((int)destination, LOW);
      //debug += "aOFFdone\n";
      //Serial.print(" SET LOW ");
    } else {
      //debug += "aON\n";
      digitalWrite((int)destination, HIGH);
      //debug += "aONdone\n";
      //Serial.print(" SET HIGH ");
    }

  } else { //we had a read
    if(destination > 63 ) { //it's an analog read, so the destination minus 64 is the A pin
      dataToSend = (long)analogRead((int)destination - 64);
    } else {
      pinMode((int)destination, INPUT);
      dataToSend = (long)digitalRead((int)destination);
    }
    //Serial.print("; Data sent: ");
    //Serial.print(dataToSend);
  }
  sei(); 
}

void writeWireLong(long val) {
  byte buffer[4];
  buffer[0] = val >> 24;
  buffer[1] = val >> 16;
  buffer[2] = val >> 8;
  buffer[3] = val;
  Wire.write(buffer, 4);
}
 
