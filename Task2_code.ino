/*Name: Priyankar Das
  ID: 2026A4PS1208H
  Task 2: Odysseus Monitoring System - State Machine
*/

#include <LiquidCrystal.h>

//Pin definitions
const int BUTTON_PIN = 2;
const int ECHO_PIN    = 3;
const int LED_PIN     = 4;
const int BUZZER_PIN  = 5;
const int TRIG_PIN    = 6;
const int LCD_RS = 7, LCD_E = 8, LCD_D4 = 9, LCD_D5 = 10, LCD_D6 = 11, LCD_D7 = 12;
const int LDR_PIN = A0;

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// State Definitions
enum ShipState { OPEN_SEA, ANCHOR_DROPPED, STORM, CHARYBDIS, WRECKED };

// Function Prototypes
void updateDisplay(ShipState state);
void updateLED(ShipState state);
void updateBuzzer(ShipState state);
void handleButtonPress(bool stormCondition, bool charybdisCondition);
void updateStateFromSensors(bool stormCondition, bool charybdisCondition);
bool checkButtonPressed();
long readDistanceCM();

// Tunable thresholds
const int LIGHT_THRESHOLD    = 83;  // 1023 = full light, < 512 IS NOT storm halfway point but rather it is around 83
const int DISTANCE_THRESHOLD = 100;  
const unsigned long WRECK_TIME_MS  = 5000;  // continuous danger before wrecking
const unsigned long BLINK_INTERVAL = 250;   // LED blink rate while in STORM
const unsigned long DEBOUNCE_DELAY = 50;    // ms, standard button debounce window

// State Variables
ShipState currentState = OPEN_SEA;
unsigned long dangerStartTime = 0;

// Button debounce state
int lastRawReading    = HIGH;
int stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lcd.begin(16, 2);
  Serial.begin(9600); 

  updateDisplay(currentState);
}

void loop() {
  // 1. Sense the world (Inverted reading)
  int   lightValue = 1023 - analogRead(LDR_PIN);
  long distanceCm  = readDistanceCM();
  
  bool stormCondition     = (lightValue < LIGHT_THRESHOLD);
  bool charybdisCondition = (distanceCm  < DISTANCE_THRESHOLD);

  // 2. Sense the button
  bool pressed = checkButtonPressed();

  // 3. Decide the next state
  if (currentState != WRECKED) {
    if (pressed) {
      handleButtonPress(stormCondition, charybdisCondition);
    } else {
      updateStateFromSensors(stormCondition, charybdisCondition);
    }
  }

  // 4. Drive the outputs
  updateDisplay(currentState);
  updateLED(currentState);
  updateBuzzer(currentState);

  // Debug monitor
  Serial.print("light="); Serial.print(lightValue);
  Serial.print(" dist=");  Serial.print(distanceCm);
  Serial.print(" state="); Serial.println(currentState);

  delay(20);
}

// 
void handleButtonPress(bool stormCondition, bool charybdisCondition) {
  if (currentState != ANCHOR_DROPPED) {
    currentState = ANCHOR_DROPPED;
  } else {
    if (stormCondition) {
      currentState = STORM;
      dangerStartTime = millis();
    } else if (charybdisCondition) {
      currentState = CHARYBDIS;
      dangerStartTime = millis();
    } else {
      currentState = OPEN_SEA;
    }
  }
}

//
void updateStateFromSensors(bool stormCondition, bool charybdisCondition) {
  switch (currentState) {
    case OPEN_SEA:
      if (stormCondition) {
        currentState = STORM;
        dangerStartTime = millis();
      } else if (charybdisCondition) {
        currentState = CHARYBDIS;
        dangerStartTime = millis();
      }
      break;

    case STORM:
      if (!stormCondition) {
        currentState = OPEN_SEA;
      } else if (millis() - dangerStartTime >= WRECK_TIME_MS) {
        currentState = WRECKED;
      }
      break;

    case CHARYBDIS:
      if (!charybdisCondition) {
        currentState = OPEN_SEA;
      } else if (millis() - dangerStartTime >= WRECK_TIME_MS) {
        currentState = WRECKED;
      }
      break;

    case ANCHOR_DROPPED:
    case WRECKED:
      break; 
  }
}

//
bool checkButtonPressed() {
  int reading = digitalRead(BUTTON_PIN);
  bool pressedEvent = false;

  if (reading != lastRawReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != stableButtonState) {
      stableButtonState = reading;
      if (stableButtonState == LOW) {
        pressedEvent = true;
      }
    }
  }

  lastRawReading = reading;
  return pressedEvent;
}

// 
long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999; 

  return duration * 0.0343 / 2;
}

//
void updateDisplay(ShipState state) {
  static ShipState lastShown = WRECKED;
  static bool firstCall = true;

  if (!firstCall && state == lastShown) return;
  firstCall = false;
  lastShown = state;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STATE:");
  lcd.setCursor(0, 1);
  switch (state) {
    case OPEN_SEA:       lcd.print("OPEN SEA");       break;
    case ANCHOR_DROPPED: lcd.print("ANCHOR DROPPED"); break;
    case STORM:          lcd.print("STORM");          break;
    case CHARYBDIS:      lcd.print("CHARYBDIS");      break;
    case WRECKED:        lcd.print("WRECKED");        break;
  }
}

//
void updateLED(ShipState state) {
  static unsigned long lastBlink = 0;
  static bool ledOn = false;

  if (state == STORM) {
    if (millis() - lastBlink >= BLINK_INTERVAL) {
      lastBlink = millis();
      ledOn = !ledOn;
      digitalWrite(LED_PIN, ledOn);
    }
  } else if (state == WRECKED) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
    ledOn = false;
  }
}

//
void updateBuzzer(ShipState state) {
  if (state == CHARYBDIS) {
    tone(BUZZER_PIN, 1000);
  } else if (state == WRECKED) {
    tone(BUZZER_PIN, 300);
  } else {
    noTone(BUZZER_PIN);
  }
}
