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
  
  if(dbPass()==1){
    Serial.println("Password Button pressed");
  }
  if(dbArm()==1){
    Serial.println("Arm button pressed");
  }

  // Updates current_event to from sensor/button inputs every iteration
  handleStates();
  //testTapSensor();
  //testButton();
  //testDoorSensor();
  
  // FSM implemented with switch cases and nested if statements
  switch (current_state) {
    case DISARMED:
      if (current_event == ARM_BUTTON_PRESSED) {
        current_state = ARMED;
        current_event = IDLE;
      }
      else if(current_event == MOTION_DETECTED){
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
      else if (current_event == ARM_BUTTON_PRESSED) {
        current_state = ENTERING_PASSWORD;
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

    case CHANGING_PASSWORD:
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
      if (current_event == IDLE) {
        current_state = DISARMED;
      }
      
      break;
    
    case TRIGGERED:
      if (current_event == IDLE) {
        current_state = DISARMED;
      }
      break;
  }
}

// Password Timer varaibles
const unsigned long FIVE_SECONDS = 5000; //Length of time used to accept password inputs
unsigned long passwordTimer = 0;
bool timerActive = false;

// Each case has if statements that correspond to their behavior in the main FSM in loop()
void handleStates() {
  bool dbA = dbArm();
  bool dbP = dbPass();
  switch (current_state) {
    dbPass();
    case DISARMED:
      dbArm();
      digitalWrite(RED_LED, LOW);
      digitalWrite(BLUE_LED, LOW);
      //MOTION detector active; will advance to ARMED state if motion detected
      if(digitalRead(MOTION_SENSOR)){
        current_event=MOTION_DETECTED;
      }
      // If Arm button is pressed, move to ARMED state in next iteration
      if(dbA){
        current_event=ARM_BUTTON_PRESSED;
      }
      
      Serial.println("DISARMED");
      break;

    case ARMED:
      digitalWrite(RED_LED, HIGH);
      digitalWrite(BLUE_LED, LOW);
      //MOTION detector active; will advance to WARNING state if motion detected
      if(digitalRead(MOTION_SENSOR)){
        current_event=MOTION_DETECTED;
      }
      // If door sensor is activated, move to TRIGGERED state in next iteration
      if(digitalRead(DOOR_SENSOR)){
        current_event=DOOR_OPENED;
      }
      Serial.println("ARMED");
      break;

    case WARNING:
      digitalWrite(RED_LED,HIGH);
      digitalWrite(BLUE_LED, HIGH);
      if(digitalRead(DOOR_SENSOR)==HIGH){
        current_event=DOOR_OPENED;
      }
      // If Arm button is pressed in warning, move to entering password, redeclare password variables
      else if (dbA) {
        current_event = ARM_BUTTON_PRESSED;
        timerActive = true;
        passwordTimer = millis();
      }
      // Function to detect door opening
      Serial.println("WARNING");
      break;

    case ENTERING_PASSWORD:
      // Detecing door opening
      digitalWrite(RED_LED, HIGH);
      digitalWrite(BLUE_LED, LOW);
      if (digitalRead(DOOR_SENSOR) == HIGH) {
        current_event = DOOR_OPENED;
      }
      // timerActive = true when dbA == 1 in WARNING or ARMED state
      if (timerActive) {
        enterPassword();
        if ((millis() - passwordTimer) > FIVE_SECONDS) {
          size_t size = getSize();
          timerActive = false;
          enteredPassword = listToPWData(size);
          bool verify = checkPassword();
          if (verify) {
            // Sends FSM to DISARMED
            current_event = CORRECT_PASSWORD;
            Serial.println("CORRECT");
          }
          else {
            // Sends FSM to TRIGGERED
            current_event = INCORRECT_PASSWORD;
            Serial.println("INCORRECT");
          }
          resetList();
          //printCorrectPassword();
          //printEnteredPassword();
          free(enteredPassword);
          passwordTimer = 0;
        }
      }
      Serial.println("ENTERING PASSWORD");
      break;

    case CHANGING_PASSWORD:
      digitalWrite(RED_LED, HIGH);
      digitalWrite(BLUE_LED, LOW);
      if (digitalRead(DOOR_SENSOR) == HIGH) {
        current_event = DOOR_OPENED;
      }
      // timerActive = true when dpPass == 1 in CHANGING_PASSWORD state
      if (timerActive) {
        enterPassword();
        if ((millis() - passwordTimer) > FIVE_SECONDS) {
          size_t size = getSize();
          timerActive = false;
          enteredPassword = listToPWData(size);
          bool verify = checkPassword();
          if (verify) {
            // Sends FSM to UPDATING_PASSWORD
            current_event = CORRECT_PASSWORD;
            Serial.println("CORRECT");
          }
          else {
            // Sends FSM to TRIGGERED
            current_event = INCORRECT_PASSWORD;
            Serial.println("INCORRECT");
          }
          resetList();
          //printCorrectPassword();
          //printEnteredPassword();
          free(enteredPassword);
          passwordTimer = 0;
        }
      }
      Serial.println("CHANGING PASSWORD");
      break;

    case UPDATING_PASSWORD:
      digitalWrite(RED_LED, LOW);
      digitalWrite(BLUE_LED, HIGH);
    // Function to enter a password of tap inputs
      if (passwordTimer == 0 && dbP) {
        timerActive = true;
        passwordTimer = millis();
      }

      // timerActive = true when dpP == 1 while in UPDATING_PASSWORD state
      if (timerActive) {
        enterPassword();
        if ((millis() - passwordTimer) > FIVE_SECONDS) {
          size_t size = getSize();
          timerActive = false;
          free(correctPassword);
          correctPassword = listToPWData(size);
          listSize = size;
          resetList();
          passwordTimer = 0;
          // Always sends FSM back to DISARMED state
          current_event = IDLE;
        }
      }
      Serial.println("UPDATING PASSWORD");
      break;
  
    case TRIGGERED:
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      Serial.println("TRIGGERED");
      break;
  }
}

// Takes user input on knock sensor
void enterPassword() {
  int tapState = digitalRead(TAP_SENSOR);
  unsigned long t = millis();
  // Only Update LED when TAP_SENSOR goes from high to low
  if (tapState == LOW) {
    if ((t - tapDebounceTime) > tapDebounceDelay) {
      tapDebounceTime = t;
      // Update Linked List with debounced time
      appendNode(t);
    }
  }
}

bool checkPassword() {
  long TOLERANCE = 75;
  listSize = getSize();
  // If password sizes are not equal, entered password cannot be the correct password
  if (correctPassword->size != enteredPassword->size) {
    Serial.println("Incorrect Size value");
    return false;
  }
  // Initial empty password edge case
  if (correctPassword->size == 1 && enteredPassword->size == 1) {
    return true;
  }
  for (int i = 1; i < listSize; i++) {
    // Tolerance added because people are not perfect metronomes
    // Accepted Range = correctTime - TOLERANECE <= enteredTime <= correctTime + TOLERANCE
    if ((long)(correctPassword->times[i] - enteredPassword->times[i]) > TOLERANCE) {
      Serial.println("Tap Time too fast");
      return false;
    }
    
    else if ((long)(enteredPassword->times[i] - correctPassword->times[i]) > TOLERANCE) {
      Serial.println("Tap Time too slow");
      return false;
    }
  }
  return true;
}

// LINKED LIST HELPER FUNCTIONS
void appendNode(unsigned long t) {
  if (head == NULL) {
    head = malloc(sizeof(inputListNode));
    head->time = t;
    head->next = NULL;
    tail = head;
  }
  else if (head->time == 0) {
    head->time = t;
  }
  else {
    inputListNode *newNode = malloc(sizeof(inputListNode));
    newNode->time = t;
    newNode->next = NULL;
    tail->next = newNode;
    tail = newNode;
  }  
  Serial.println("TAP APPENDED");
}

// Frees memory of inputListNode, so it can be reused in future password inputs
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

// Converts Linked List stored at inputListNode *head to a struct storing its size and time values
passwordData* listToPWData(size_t listSize) {
  // Geeks For Geeks FAM implementation
  passwordData *data = malloc(sizeof(passwordData) + sizeof(unsigned long) * listSize);
  data->size = listSize;
  unsigned long firstTap = head->time;
  inputListNode *temp = head;

  for (int i = 0; i < listSize; i++) {
    // Each time value is zeroed out from the first tap so passwords entered at later times can still be compared
    data->times[i] = temp->time - firstTap;
    temp = temp->next;
  }

  return data;
}




int dbArm(){    // dbArm(); handles the input from the arm/disarm button(ARM_BUTTON) uses the standard in class debounce solution
int toreturn=0;
  int currentButtonState = digitalRead(ARM_BUTTON);
  
  // Check if the button state has changed (due to noise or pressing)
  if (currentButtonState != armlastButtonState) {
    // Reset the debouncing timer
    armlastDebounceTime = millis();
  }
  
  // Check if the button state has been stable for longer than the debounce delay
  if ((millis() - armlastDebounceTime) > debounceDelay) {
    // If the button state has changed from our last debounced state
    if (currentButtonState != armdebouncedButtonState) {
      armdebouncedButtonState = currentButtonState;
      
      // Only take action on button press (transition from HIGH to LOW)
      if (armdebouncedButtonState == LOW) {
        toreturn=1;
        current_event=ARM_BUTTON_PRESSED;
        if (current_state == ARMED || current_state == WARNING) {
          timerActive = true;
          passwordTimer = millis();
        }
      
      }
    }
  }
  armlastButtonState = currentButtonState;
  if(toreturn){
    return 1;
  }
  else{
    return 0;
  }
}
int dbPass(){   // dbPass(); handles the input from the password control button(PASS_BUTTON), uses the standard in class debounce solution
int toreturn=0;
  int currentButtonState = digitalRead(PASS_BUTTON);
  
  // Check if the button state has changed (due to noise or pressing)
  if (currentButtonState != passlastButtonState) {
    // Reset the debouncing timer
    passlastDebounceTime = millis();
  }
  
  // Check if the button state has been stable for longer than the debounce delay
  if ((millis() - passlastDebounceTime) > debounceDelay) {
    // If the button state has changed from our last debounced state
    if (currentButtonState != passdebouncedButtonState) {
      passdebouncedButtonState = currentButtonState;
      
      // Only take action on button press (transition from HIGH to LOW)
      if (passdebouncedButtonState == LOW) {
        toreturn=1;
        current_event=UPDATE_BUTTON_PRESSED;
        timerActive = true;
        passwordTimer = millis();
      }
    }
  }

  //NOTE:
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------
  // dbArm() and dbPass() are modified versions of the provided debounce solution provided in the file titled CSE321_ Debounce solution.zip on UBLearns
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------
  
  passlastButtonState = currentButtonState;
  // Returns true or false based on button state, HIGH input will result in a 0, LOW input will result in a 1. This is opposite to actual input, however this makes more logical sense for our use. 
  if(toreturn){
    return 1;
  }
  else{
    return 0;
  }
}

