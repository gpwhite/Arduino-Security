

// Knock password size will vary each time, so a linked list is used for dynamic sizing 
typedef struct inputListNode {
  unsigned long time;
  inputListNode *next;
} inputListNode;

// Once the entered password is processed, it is stored in a passwordData struct using Flexible Array Members (FAM) to store different sized arrays
// Source, Geeks for Geeks: https://www.geeksforgeeks.org/c/flexible-array-members-structure-c/
typedef struct passwordData {
  size_t size;
  unsigned long times[];
} passwordData;

// Password structs
inputListNode *head;
inputListNode *tail;
passwordData *correctPassword;
passwordData *enteredPassword;
size_t listSize = 1;


int TAP_SENSOR = 2;
int RED_LED = 3;
int BLUE_LED = 4;
int PASS_BUTTON = 5;
int ARM_BUTTON = 6;
int DOOR_SENSOR = 7;
int SPEAKER_OUT = 8;
int MOTION_SENSOR=9;

// Testing Variables
bool red_led_state = false;
bool blue_led_state = false;

// Debouncing Variables
unsigned long tapDebounceTime = 0;
// Tap Sensor is very sensitive, so it requires a high debounce delay for accurate inputs. Limits total number of knock inputs to 25 (5000ms / 200ms)
const unsigned long tapDebounceDelay = 200;  

int passlastButtonState = HIGH;      // Previous reading from the button
int passdebouncedButtonState = HIGH; 
unsigned long passlastDebounceTime = 0;  // Last time the button state changed
const unsigned long debounceDelay = 20; // Debounce time in milliseconds
int armlastButtonState = HIGH;      // Previous reading from the button
int armdebouncedButtonState = HIGH; 
unsigned long armlastDebounceTime = 0; 

void setup() {

  pinMode(TAP_SENSOR, INPUT_PULLUP);
  pinMode(ARM_BUTTON, INPUT_PULLUP);
  pinMode(DOOR_SENSOR, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(SPEAKER_OUT,OUTPUT);
  pinMode(MOTION_SENSOR,INPUT);
  pinMode(ARM_BUTTON,INPUT_PULLUP);
  pinMode(PASS_BUTTON,INPUT_PULLUP);

  // Initializing Password related structs
  head = malloc(sizeof(inputListNode));
  if (head == NULL) {
    Serial.println("Linked List Allocation failed");
  }
  head->time = 0;
  head->next = NULL;
  tail = head;

  correctPassword = malloc(sizeof(passwordData) + sizeof(unsigned long) * listSize);
  if (correctPassword == NULL) {
    Serial.println("Password Data Allocation failed");
  }
  correctPassword->size = listSize;
  correctPassword->times[0] = 0; // Initial Correct Password is 0 knocks entered

  Serial.begin(115200);
}



void loop() {
  // put your main code here, to run repeatedly:

}
void testTapSensor() {
  int tapState = digitalRead(TAP_SENSOR);
  // Only Update LED when TAP_SENSOR goes from high to low
  if (tapState == LOW) {
    if (millis() - tapDebounceTime > tapDebounceDelay) {
      tapDebounceTime = millis();
      red_led_state = !red_led_state;
      digitalWrite(RED_LED, red_led_state);
    }
  }
}
void testDoorSensor() {
  int doorState = digitalRead(DOOR_SENSOR);
  digitalWrite(RED_LED, doorState);
}
void testSpeaker(){ //Writes high and low inputs to the speaker to confirm functionality 
  digitalWrite(SPEAKER_OUT,HIGH);
  delay(300);
  digitalWrite(SPEAKER_OUT,LOW);
  delay(300);
}
void printCorrectPassword() { //Used when debugging password to view the times of inputs of inputted password vs previously set password
  Serial.println(correctPassword->size);
  for (int i = 0; i < correctPassword->size; i++) {
    Serial.print(correctPassword->times[i]);
    Serial.print(", ");
  }
  Serial.println("");
}

void printEnteredPassword() {
  Serial.println(enteredPassword->size);
  for (int i = 0; i < enteredPassword->size; i++) {
    Serial.print(enteredPassword->times[i]);
    Serial.print(", ");
  }
  Serial.println("");
}

