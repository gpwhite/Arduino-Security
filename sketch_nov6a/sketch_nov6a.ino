void setup() {
  // put your setup code here, to run once:
pinMode(7,OUTPUT);
pinMode(6,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

if(analogRead(0)>100){
  digitalWrite(7,HIGH);
  delay(200);
  digitalWrite(7,LOW);
//Door sensor works, please dont do what I did and assume its a knock sensor and slam it up and down like a fucking moron. 

}
}
