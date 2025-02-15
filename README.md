This sketch will turn any Arduino into an I2C Slave whose pins can be manipulated by a master.  Your wiring will look something like this (depending on your Arduinos):

  ![image](masterslave_arduino_schematic.jpg)


  ![image](masterslave_arduino.jpg)

To keep the interface as simple as possible, there are only two things that the slave can do in response to the master.
It can send a value to a pin (analog or digital, depending on the capability of the pin and how it is used).
Or it can receive a value from a pin (analog or digital, depending on the capability of the pin and how it is used).

If only one byte is sent to the slave, it is interpreted as a read operation to a pin and a value is prepared for return to the master.
If more than one byte is sent to the slave, it is interpreted as a write operation, with the first byte being a pin number and the rest being data 
to be sent to the pin.

Pins normally correspond to whatever the numbering system is used by the Arduino (that is, 3 means D3 or perhaps GPIO3).  But analog pins such as
A1, A2, etc. can also be used as outputs, where, on the Arduino Uno, A0 is at digitalWrite(14, HIGH/LOW).  When using the analog pins as analog 
inputs, they are addressed with addresses starting at 64 to keep them out of the number space of the D pins.

When sending digital values to a pin, use either 0 or 1 for LOW or HIGH.  When sending analog values, use 256 plus the analog value.  Any number
above 255 is interpreted as an analog value.  This minor headache for the developers keeps the data to the slave as simple as possible.

VERY IMPORTANT:  If you don't have any other I2C devices on the bus with your master and slave Arduinos, you might well need pull-up resistors on the 
SCL and SDA lines.  Use 5 or 10 kilohms.  Otherwise the slave might hang after processing one data packet.


To use this, set the I2C Address in the sketch appropriately and then refer to it in your Master code.  
To write digital values to the pins on the slave (once flashed with this software) from the master, put code on your Master doing something like this:

<code>

//set your I2CAddress in you master code to whatever it is in your sketch
int arduinoSlaveAddress = 20;

//simple example:
Wire.beginTransmission(arduinoSlaveAddress);
Wire.write(2); //will write a digital value to D2 on an Arduino Mini
Wire.write(1); //anything between 1 and 255 counts as HIGH
Wire.endTransmission(); 
delay(800); 
Wire.beginTransmission(arduinoSlaveAddress); 
Wire.write(2); 
Wire.write(0);  //counts as LOW
Wire.endTransmission();
delay(800); 


//using the Analog pins as digital outs:
Wire.beginTransmission(arduinoSlaveAddress);
Wire.write(14); //will write a digital value to A0 on an Arduino Mini
Wire.write(11); //anything between 1 and 255 counts as HIGH
Wire.endTransmission(); 
delay(800); 
Wire.beginTransmission(arduinoSlaveAddress); 
Wire.write(14); 
Wire.write(0);  //counts as LOW
Wire.endTransmission();
delay(800); 


 //sending an analog value to a pin that can do PWM:
 Wire.beginTransmission(arduinoSlaveAddress);
 Wire.write(3); //will write a value to D3 on an Arduino Mini
 Wire.write(257); //anything above 255 does an AnalogWrite(value-256) to the pin
 Wire.endTransmission(); 
 delay(800); 
 Wire.beginTransmission(arduinoSlaveAddress); 
 Wire.write(3); 
 Wire.write(511);  //counts as AnalogWrite(255);
 Wire.endTransmission();
 delay(800); 


//reading an analog or digital value from the slave:
Wire.beginTransmission(arduinoSlaveAddress); 
Wire.write(78); //addresses greater than 64 are the same as AX (AnalogX) where X is 64-value
Wire.endTransmission(); 
delay(100); Wire.requestFrom(arduinoSlaveAddress, 4); 
long totalValue = 0; 
int byteCursor = 1; 
while (Wire.available()) { 
  byte receivedValue = Wire.read(); // Read the received value from slave 
  totalValue = totalValue + receivedValue * pow(256, 4-byteCursor); 
  Serial.println(receivedValue); // Print the received value 
  byteCursor++; 
} 
Serial.print(millis()); 
Serial.print(" "); 
Serial.println((int)totalValue); 
delay(600);
</code>
