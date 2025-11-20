//----------------------------------------------------------------------
//--------------------------------PIN LAYOUT----------------------------
// Pin 7: Input_Pullup for door sensor, other line connects to GND
// Pin 6: Speaker output
//----------------------------------------------------------------------
  
void setup() {
pinMode(7,INPUT_PULLUP); //INIT Door sensor input
pinMode(6,OUTPUT);
Serial.begin(9600);
}

void loop() {

  //Door Detection Code, outputs to serial monitor----------------------
  if(digitalRead(7)==LOW){
     Serial.print("detected closed\n");
     
  }
else{
  digitalWrite(6,HIGH);
}
//END Door Code---------------------------------------------------------
//BEGIN Camera Code


}
