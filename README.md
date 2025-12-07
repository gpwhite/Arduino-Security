Arduino-Security
---
Created By:

Sean Fluman & Gregory White
  
---



Purpose  
***

Sensors Used
***
This software is designed to work with any standard arduino platform capable of spporting digital input and outputs. The design relies on three sensors, denoted as 
1) Knock Sensor
2) Door Sensor
3) Motion Sensor

Door Sensor:  
-Standard Magnetic Reed Switch.  
-Should produce a high voltage when the sensor is within range of a magnet.   
-Contains two wires, one is connected to digital input pin, other is connected to GND. 

Knock Sensor:  
-KY-031 Knock Sensor Module.   
-Uses three wires, one should connect to Vcc(5v) one should connect to GND, and the data pin should connect to the digital input pin.   
-Debounced in software.   


