void setup() {
  // put your setup code here, to run once:

  //-----------------------------------------------------------------------------------------------------------
  //--------------------------------PIN LAYOUT----------------------------
  //
pinMode(7,INPUT_PULLUP); //INIT Door sensor input

Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

if(digitalRead(7)==LOW){
  Serial.print("detected closed\n");
  //delay(200);
  //digitalWrite(7,LOW);
//Door sensor works, please dont do what I did and assume its a knock sensor and slam it up and down like a fucking moron. 

}
else{
  Serial.print("Open\n");
}
}
