/* Gus Mueller, April 2 2024
 * sends data to a small Arduino (I use a Mini Pro at 8MHz) from a
 * NodeMCU over I2C
 * 
 */
#include "Wire.h"
#define I2C_SLAVE_ADDR 20
 
 
volatile int receivedValue = 0;
long lastMasterSignal = 0;
long millisNow = millis();
long dataToSend = -1;
 

void setup(){
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receieveEvent); 
  Wire.onRequest(requestEvent);
  Serial.begin(115200);
  Serial.println("Starting up Arduino Slave...");
}

void loop(){
  millisNow = millis();
  //if it's been more than 20 minutes since a raspberry pi i2c signal:
  Serial.print("millis: ");
  Serial.print(millisNow);
  Serial.print(" ");
  Serial.print(lastMasterSignal);
  Serial.println(" ");
  delay(2000);
}

//send a byte to the I2C master.  
//on the ATTiny, the the master calls this x times, not just once as I'd originally thought
//but in the true Wire library it seems you can send whole arrays, which is all that matters here
void requestEvent(){
  //lastMasterSignal = millisNow;
  //the usual data dump
  writeWireLong(dataToSend);
}

void receieveEvent() {
  lastMasterSignal = millisNow;
  //Serial.println("receive event");
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
    receivedValue = receivedValue + receivedValues[otherByteCursor-1] * pow(256, byteCursor-1)  ;
    //Serial.println("qoot: ");
    //Serial.print(byteCursor-1);
    //Serial.print(":");
    //Serial.print(receivedValue);
  }
  Serial.print("Destination: ");
  Serial.print(destination);
  
  if(byteCursor > 0) { //we had a write
    Serial.print("; value: ");
    Serial.print(receivedValue);
    pinMode((int)destination, OUTPUT);
    if(receivedValue > 255 ) {
      analogWrite((int)destination, receivedValue - 256); //if you want to send analog content, add 256 to it first
    } else if (receivedValue == 0 ) {
      digitalWrite((int)destination, LOW);
      Serial.print(" SET LOW ");
    } else {
      digitalWrite((int)destination, HIGH);
      Serial.print(" SET HIGH ");
    }

  } else { //we had a read
    if(destination > 63 ) { //it's an analog read, so the destination minus 64 is the A pin
      dataToSend = (long)analogRead((int)destination - 64);
    } else {
      pinMode((int)destination, INPUT);
      dataToSend = (long)digitalRead((int)destination);
     
    }
    //writeWireLong((long)dataToSend);
    Serial.print("; Data sent: ");
    Serial.print(dataToSend);
  }
 
  Serial.println();
}

void writeWireLong(long val) {
  byte buffer[4];
  buffer[0] = val >> 24;
  buffer[1] = val >> 16;
  buffer[2] = val >> 8;
  buffer[3] = val;
  Wire.write(buffer, 4);
}

void writeWireInt(int val) {
  byte buffer[2];
  buffer[0] = val >> 8;
  buffer[1] = val;
  Wire.write(buffer, 2);
}
 
 
 
