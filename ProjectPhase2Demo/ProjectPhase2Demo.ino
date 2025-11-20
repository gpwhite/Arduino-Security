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
  SINGLE_BUTTON_PRESS,
  DOUBLE_BUTTON_PRESS,
  INCORRECT_PASSWORD,
  CORRECT_PASSWORD,
  DOOR_OPENED
} event;

state current_state;
event current_event;

int TAP_SENSOR = 2;
int RED_LED = 3;
int BLUE_LED = 4;
int GREEN_LED = 5;
int BUTTON = 6;
int DOOR_SENSOR = 7;
int SPEAKER_OUT = 8;

bool red_led_state = false;
bool blue_led_state = false;
bool green_led_state = false;

// Debouncing Variables
unsigned long tapDebounceTime = 0;  
const unsigned long tapDebounceDelay = 200; // Tap Sensor is very sensitive, so it requires a high debounce delay for accurate inputs. It could also be my implementation thats the problem, look into fixing to allow for faster tapping passwords

int prevButtonState = HIGH;
int buttonDebouncedState = HIGH;
unsigned long buttonDebounceTime = 0;  
const unsigned long buttonDebounceDelay = 20;

void setup() {
  // Initial State
  current_state = DISARMED;
  // Initial Event = Idle, nothings happened yet
  current_event = IDLE;
  pinMode(TAP_SENSOR, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(DOOR_SENSOR, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(SPEAKER_OUT,OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Update current_event first so current_state can update accordingly
  handleEvents();
  testTapSensor();
  testButton();
  testDoorSensor();
  
  // FSM implemented with switch cases and nested if statements, shows control flow of to-be-implemented features
  // If current_event is not relevant to current_state it is ignored
  switch (current_state) {
    case DISARMED:
      if (current_event == SINGLE_BUTTON_PRESS) {
        current_state = ARMED;
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
      if (current_event == SINGLE_BUTTON_PRESS) {
        current_state = ENTERING_PASSWORD;
      }
      else if (current_event == DOUBLE_BUTTON_PRESS) {
        current_state = CHANGING_PASSWORD;
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
      if (current_event == SINGLE_BUTTON_PRESS) {
        current_state = DISARMED;
      }
      break;
  }
  // Will execute function associated with current_state after it has updated
  handleStates();
}

// Will use the input data received from each sensor to properly determine the correct state
// Will need to implement synchronization measures to protect shared value current_event at this state
void handleEvents() {}

void handleStates() {
    switch (current_state) {
    case DISARMED:
      // Function to digitalWrite(GREEN_LED, HIGH)
      break;

    case ARMED:
      // Function to digitalWrite(RED_LED, HIGH)
      // Function for motion detection
      // Function to detect door opening
      break;

    case WARNING:
      // Function to Blink RED_LED
      // Function to detect door opening
      break;

    case ENTERING_PASSWORD:
    // Function to detect door opening
    // Function to match tap inputs to a preset password
      break;

    case CHANGING_PASSWORD:
    // Function to detect door opening
    // Function to match tap inputs to a preset password
      break;

    case UPDATING_PASSWORD:
    // Function to enter a password of tap inputs
      break;
  }
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
void testButton() {
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
}

void testDoorSensor() {
  int doorState = digitalRead(DOOR_SENSOR);
  digitalWrite(GREEN_LED, doorState);
}

void testSpeaker(){
  digitalWrite(SPEAKER_OUT,HIGH);
  delay(300);
  digitalWrite(SPEAKER_OUT,LOW);
  delay(300);
}
