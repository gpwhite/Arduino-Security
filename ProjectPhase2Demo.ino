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



void setup() {
  // put your setup code here, to run once:
  current_state = DISARMED;
  current_event = IDLE;
}

void loop() {
  // put your main code here, to run repeatedly:
  handleEvents();

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
  handleStates();
}


void handleEvents() {
  

}

void handleStates() {}
