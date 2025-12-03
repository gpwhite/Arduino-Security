typedef enum state {
  DISARMED,
  ARMED,
  WARNING,
  ENTERING_PASSWORD,
  CHANGING_PASSWORD,
  UPDATING_PASSWORD,
  TRIGGERED
} state;

typedef enum event {
  IDLE, // Do nothing
  MOTION_DETECTED,
  ARM_BUTTON_PRESSED,
  UPDATE_BUTTON_PRESSED,
  INCORRECT_PASSWORD,
  CORRECT_PASSWORD,
  DOOR_OPENED
} event;

state current_state;
event current_event;

typedef struct inputListNode {
  unsigned long time;
  inputListNode *next;
} inputListNode;

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

unsigned long lastPassTime=0;
unsigned long lastArmTime=0;


int TAP_SENSOR = 2;
int RED_LED = 3;
int BLUE_LED = 4;
int PASS_BUTTON = 5;
int ARM_BUTTON = 6;
int DOOR_SENSOR = 7;
int SPEAKER_OUT = 8;
int MOTION_SENSOR=9;


bool red_led_state = false;
bool blue_led_state = false;

// Debouncing Variables
unsigned long tapDebounceTime = 0;  
const unsigned long tapDebounceDelay = 200; // Tap Sensor is very sensitive, so it requires a high debounce delay for accurate inputs. It could also be my implementation thats the problem, look into fixing to allow for faster tapping passwords

int prevButtonState = HIGH;
int buttonDebouncedState = HIGH;
unsigned long buttonDebounceTime = 0;  
const unsigned long buttonDebounceDelay = 20;
int lastArmState=LOW;
int lastPassState=LOW;
int armState;
int passState;

void setup() {
  // Initial State
  current_state = DISARMED;
  // Initial Event = Idle, nothings happened yet
  current_event = IDLE;
  pinMode(TAP_SENSOR, INPUT_PULLUP);
  pinMode(ARM_BUTTON, INPUT_PULLUP);
  pinMode(DOOR_SENSOR, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(SPEAKER_OUT,OUTPUT);
  pinMode(MOTION_SENSOR,INPUT);
  pinMode(ARM_BUTTON,INPUT_PULLUP);
  pinMode(PASS_BUTTON,INPUT_PULLUP);

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
  correctPassword->times[0] = 0; // MAKE SURE NOT TO TOUCH SENSOR WHEN SETTING INITIAL PASSWORD, OR ELSE REBOOT IS NECESSARY

  Serial.begin(9600);
}

void loop() {
  // Update current_event first so current_state can update accordingly
  handleEvents();
  testTapSensor();
  //testButton();
  testDoorSensor();
  
  // FSM implemented with switch cases and nested if statements, shows control flow of to-be-implemented features
  // If current_event is not relevant to current_state it is ignored
  switch (current_state) {
    case DISARMED:
      if (current_event == ARM_BUTTON_PRESSED) {
        current_state = ARMED;
      }
      if(current_event == MOTION_DETECTED){
        current_state = ARMED;
      }
      else if (current_event == UPDATE_BUTTON_PRESSED) {
        current_state = CHANGING_PASSWORD;
      }
      break;

    case ARMED:
      if (current_event == MOTION_DETECTED) {
        current_state = WARNING;
      }
      else if (current_event == DOOR_OPENED) {
        current_state = TRIGGERED;
      }
      break;

    case WARNING:
      if (current_event == ARM_BUTTON_PRESSED) {
        current_state = ENTERING_PASSWORD;
      }
      else if (current_event == DOOR_OPENED) {
        current_state = TRIGGERED;
      }
      break;

    case ENTERING_PASSWORD:
      if (current_event == DOOR_OPENED) {
        current_state = TRIGGERED;
      }
      else if (current_event == INCORRECT_PASSWORD) {
        current_state = TRIGGERED;
      }
      else if (current_event == CORRECT_PASSWORD) {
        current_state = DISARMED;
      }
      break;

    /*case CHANGING_PASSWORD:
      if (current_event == DOOR_OPENED) {
        current_state = TRIGGERED;
      }
      else if (current_event == INCORRECT_PASSWORD) {
        current_state = TRIGGERED;
      }
      else if (current_event == CORRECT_PASSWORD) {
        current_state = UPDATING_PASSWORD;
      }
      break;

    case UPDATING_PASSWORD:
      if (current_event == SINGLE_BUTTON_PRESS) {
        current_state = DISARMED;
      }
      break;
  */
  // Will execute function associated with current_state after it has updated
  handleStates();
}
}
// Will use the input data received from each sensor to properly determine the correct state
// Will need to implement synchronization measures to protect shared value current_event at this state
void handleEvents() {}

void handleStates() {
    switch (current_state) {
    case DISARMED:
      // no lights will be enabled during disarm
      if(digitalRead(MOTION_SENSOR)){ //MOTION detector active; will advance to armed state if motion detected
        current_event=MOTION_DETECTED;
      }
      if(dbArm()==1){//if arm button is pressed
      current_event=ARM_BUTTON_PRESSED;
      }
      break;

    case ARMED:
      digitalWrite(RED_LED, HIGH);
      if(digitalRead(MOTION_SENSOR)){
        current_event=MOTION_DETECTED;
      }
      if(digitalRead(DOOR_SENSOR)){
        current_event=DOOR_OPENED;
      }
      break;

    case WARNING:
      // Function to turn on red and blue 
      digitalWrite(RED_LED,HIGH);
      digitalWrite(BLUE_LED, HIGH);
      if(digitalRead(DOOR_SENSOR)==HIGH){
        current_event=DOOR_OPENED;
      }

      // Function to detect door opening
      break;

    case ENTERING_PASSWORD:
      // Detecing door opening
      if (digitalRead(DOOR_SENSOR) == HIGH) {
        current_event = TRIGGERED;
      }
    // Function to match tap inputs to a preset password
      enterPassword();
      if ((millis() - passwordTimer) > FIVE_SECONDS) {
        size_t size = getSize();
        timerActive = false;
        enteredPassword = listToPWData(size);
        bool verify = checkPassword();
        if (verify) {
          current_event = DISARMED;
        }
        else {
          current_event = TRIGGERED;
        }
        resetList();
        free(enteredPassword);
        passwordTimer = 0;
      }
      break;

    case CHANGING_PASSWORD:
      if (digitalRead(DOOR_SENSOR) == HIGH) {
        current_event = TRIGGERED;
      }
      enterPassword();
      if ((millis() - passwordTimer) > FIVE_SECONDS) {
        size_t size = getSize();
        timerActive = false;
        enteredPassword = listToPWData(size);
        bool verify = checkPassword();
        if (verify) {
          current_event = UPDATING_PASSWORD;
        }
        else {
          current_event = TRIGGERED;
        }
        resetList();
        free(enteredPassword);
        passwordTimer = 0;
      }
      break;

    case UPDATING_PASSWORD:
    // Function to enter a password of tap inputs
      enterPassword();
      if ((millis() - passwordTimer) > FIVE_SECONDS) {
        size_t size = getSize();
        timerActive = false;
        correctPassword = listToPWData(size);
        listSize = size;
        resetList();
        passwordTimer = 0;
      }
      break;
  
    case TRIGGERED:
      break;
  }
}

void enterPassword() {
  int tapState = digitalRead(TAP_SENSOR);
  unsigned long t = millis();
  // Only Update LED when TAP_SENSOR goes from high to low
  if (tapState == LOW) {
    if ((t - tapDebounceTime) > tapDelay) {
      tapDebounceTime = t;
      appendNode(t);
    }
  }
}

bool checkPassword() {
  listSize = getSize();
  if (correctPassword->size != enteredPassword->size) {
    return false;
  }
  for (int i = 0; i < listSize; i++) {
    // 150ms tolerance , adjust until working properly
    if (!((correctPassword->times[i] - 75) < enteredPassword->times[i] && (correctPassword->times[i] + 75) > enteredPassword[i])) {
      return false;
    }
  }
  return true;
}

// LINKED LIST FUNCTIONS
void appendNode(unsigned long t) {
  if (head->time == 0) {
    head->time = t;
  }
  else {
    inputListNode *newNode = malloc(sizeof(inputListNode));
    newNode->time = t;
    newNode->next = NULL;
    tail->next = newNode;
    tail = newNode;
  }  
}

void resetList() {
  while (head != NULL) {
    inputListNode *temp = head->next;
    free(head);
    head = temp;
  }
  listSize = 0;
}

size_t getSize() {
  size_t out = 0;
  inputListNode *temp = head;
  while (temp != NULL) {
    out++;
    temp = temp->next;
  }
  return out;
}

// Remember to site geeks for geeks Flexible Array Member
passwordData* listToPWData(size_t listSize) {
  passwordData *data = malloc(sizeof(passwordData) + sizeof(unsigned long) * listSize);
  data->size = listSize;
  unsigned long firstTap = head->time;
  inputListNode *temp = head;

  for (int i = 0; i < listSize; i++) {
    data->times[i] = temp->time - firstTap;
    temp = temp->next;
  }

  return data;
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

// Source: Button Debounce Solution - CSE 321 UBLearns
/*void testButton() {
  int buttonState = digitalRead(BUTTON);

  if (buttonState != prevButtonState) {
    buttonDebounceTime = millis();
  }

  if (millis() - buttonDebounceTime > buttonDebounceDelay) {
    if (buttonDebouncedState != buttonState) {
      buttonDebouncedState = buttonState;
      if (buttonState == LOW) {
        buttonDebounceTime = millis();
        blue_led_state = !blue_led_state;
      }
    }
  }
  digitalWrite(BLUE_LED, blue_led_state);
  prevButtonState = buttonState;
}*/

void testDoorSensor() {
  int doorState = digitalRead(DOOR_SENSOR);
  //digitalWrite(GREEN_LED, doorState);
}

//dbArm() - debeounces input to ARM_BUTTON and returns 1 or 0
int dbArm(){
  int reading = digitalRead(ARM_BUTTON);
  if(reading!=lastArmState){
    lastArmTime=millis();
  }
  if((millis()-lastArmTime)>buttonDebounceDelay){
    if(reading!= armState){
      armState=reading;    
    if(armState==LOW){
      lastArmTime=millis();
      return 1;
    }}
    return 0;
  }
}
int dbPass(){ //same as dbArm just for the PASS button
  int reading = digitalRead(PASS_BUTTON);
  if(reading!=lastPassState){
    lastPassState=millis();
  }
  if((millis()-lastPassTime)>buttonDebounceDelay){
    if(reading!= passState){
      passState=reading;    
    if(passState==LOW){
      lastPassTime=millis();
      return 1;
    }}
    return 0;
  }
}
void testSpeaker(){
  digitalWrite(SPEAKER_OUT,HIGH);
  delay(300);
  digitalWrite(SPEAKER_OUT,LOW);
  delay(300);
}
