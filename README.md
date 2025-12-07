# Arduino-Security

Created By:

Sean Fluman & Gregory White
  
<br>



# **Purpose**
***
The purpose of this project is to create an affordable home security system built on the Arduino platform. The system provides security measures which when triggered, will sound an alarm in order to alert the user. The system is designed to be easily disarmed by the user with the use of a "knock sensor" which can be installed on the users door in order to make disarming a convenient process. The password is remembered as a tapping rythm inputted by the user.

## **Sensors Used**
***
This software is designed to work with any standard Arduino platform capable of spporting digital input and outputs. The design relies on three sensors, denoted as 
1) Knock Sensor
2) Door Sensor
3) Motion Sensor

**Door Sensor:**  
-Standard Magnetic Reed Switch.  
-Should produce a high voltage when the sensor is within range of a magnet.   
-Contains two wires, one is connected to digital input pin, other is connected to GND. 

**Knock Sensor:**  
-KY-031 Knock Sensor Module.   
-Uses three wires, one should connect to Vcc(3.3-5v) one should connect to GND, and the data pin should connect to the digital input pin.   
-Debounced in software.   
-Produces high voltage output when knock sensor is tapped.  

**Motion Sensor:**  
-Uses any standard PIR(Passive Infared) module.  
-Uses three wires, one should connect to Vcc(Typically 5-12v) one should connect to GND, and the data pin should connect to the digital input pin.     
-Prouduces high voltage output when motion is detected.  

