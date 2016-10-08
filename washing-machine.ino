#include <LiquidCrystal.h>

LiquidCrystal lcd(11, 10, 9, 8, 7, 6, 5);

const char washStr[] = "Wash";
const char modeStr[] = "Mode";
const char rinseStr[] = "Rinse";
const char typeStr[] = "Type";
const char waterStr[] = "Water";
const char levelStr[] = "Level";
const char lowStr[] = "Low";
const char mediumStr[] = "Medium";
const char highStr[] = "High";
const char timeStr[] = "Time";
const char minStr[] = "min";
const char fillingStr[] = "Filling Up";
const char drainStr[] = "Draining";
const char washingStr[] = "Washing";
const char rinsingStr[] = "Rinsing";
const char completeStr[] = "Complete";
const char collonStr[] = ":";
const char spaceStr[] = " ";
const char plusStr[] = "+";

const int ZERO = 0;
const int ONE = 1;
const int TWO = 2;
const int THREE = 3;
const int FOUR = 4;
const int FIVE = 5;
const int SIX = 6;

const int noOfStages = SIX;
const int noOfWashModes = THREE;
const int noOfWaterLevel = THREE;
const int maxWashTime = 15;
const int maxRinseTime = 5;
const int noOfLcdColumns = 16;
const int noOfLcdRows = 2;

const int waterInletPin = 13;
const int waterDrainPin = 12;
const int motorControlPin = A0;
const int selectButtonPin = 2;
const int upButtonPin = 3;
const int downButtonPin = 4;
const int lowSensorPin = A5;
const int midSensorPin = A4;
const int highSensorPin = A3;
const int drainSensorPin = A2;

int stage = ZERO; // 0 - Wash Mode, 1 - Water Level, 2 - Wash Time, 3 - Rinse Time, 4 - Start, 5 - Complete
int selectedWashMode = ZERO; // 0 - Wash + Rinse, 1 - Wash, 2 - Rinse
int selectedWaterLevel = ONE; // 0 - Low, 1 - Medium, 2 - High
int selectedWashTime = maxWashTime - 1; // In minutes
int selectedRinseTime = THREE - 1; // In minutes
boolean upDownButtonPressed = true;
boolean selectButtonPressed = true;
boolean waterLevelReached = false;

void setup() {
  pinMode(selectButtonPin, INPUT);
  pinMode(upButtonPin, INPUT);
  pinMode(downButtonPin, INPUT);
  pinMode(lowSensorPin, INPUT);
  pinMode(midSensorPin, INPUT);
  pinMode(highSensorPin, INPUT);
  pinMode(drainSensorPin, INPUT);
  pinMode(waterInletPin, OUTPUT);
  pinMode(waterDrainPin, OUTPUT);
  pinMode(motorControlPin, OUTPUT);
  digitalWrite(motorControlPin, LOW);
  digitalWrite(waterInletPin, LOW);
  digitalWrite(waterDrainPin, LOW);
  lcd.begin(noOfLcdColumns, noOfLcdRows);
}

void loop() {
  int selectButtonState = digitalRead(selectButtonPin);
  if (selectButtonState == HIGH) {
    incrementStage();
    delay(200);
  }

  switch (stage) {
    case ZERO:    // Select Wash Mode
      selectValue(&selectedWashMode, noOfWashModes);
      printWashMode();
      break;
    case ONE:    // Select Water Level
      selectValue(&selectedWaterLevel, noOfWaterLevel);
      printWaterLevel();
      break;
    case TWO:    // Select Wash Time
      if (selectedWashMode == TWO) {
        incrementStage();
      } else {
        selectValue(&selectedWashTime, maxWashTime);
        printWashTime();
      }
      break;
    case THREE:    // Select Rinse Time
      if (selectedWashMode == ONE) {
        incrementStage();
      } else {
        selectValue(&selectedRinseTime, maxRinseTime);
        printRinseTime();
      }
      break;
    case FOUR: // Wash in Progress
      drainWater();
      if (selectedWashMode == ZERO || selectedWashMode == ONE) {
        fillUpWater();
        startCycle(selectedWashTime + 1, washingStr);
      }
      drainWater();
      if (selectedWashMode == ZERO || selectedWashMode == TWO) {
        fillUpWater();
        startCycle(selectedRinseTime + 1, rinsingStr);
      }
      drainWater();
      incrementStage();
      break;
    case FIVE: // Wash Complete
      const char *topRow[THREE] = { washStr, spaceStr, completeStr };
      printTopRowIfSelectButtonPressed(topRow, THREE, TWO);
      break;
  }
}

void startCycle(int cycleTimeInMinutes, const char *type) {
  lcd.clear();
  const char *topRow[ONE] = { type };
  printTopRow(topRow, ONE, FIVE);

  digitalWrite(motorControlPin, HIGH);
  for (int cycleTimeLeft = cycleTimeInMinutes; cycleTimeLeft > ZERO; cycleTimeLeft--) {
    char buffer[TWO];
    const char *bottomRow[THREE] = { itoa(cycleTimeLeft, buffer, 10), spaceStr, minStr };
    printBottomRow(bottomRow, THREE, SIX);
    delay(60000);
  }
  digitalWrite(motorControlPin, LOW);
}

void drainWater() {
  lcd.clear();
  const char *topRow[THREE] = { drainStr, spaceStr, waterStr };
  printTopRow(topRow, THREE, ONE);

  while (digitalRead(drainSensorPin) == LOW) {
    digitalWrite(waterDrainPin, HIGH);
    delay(5000);
  }
  digitalWrite(waterDrainPin, LOW);
  waterLevelReached = false;
}

void fillUpWater() {
  lcd.clear();
  const char *topRow[THREE] = { fillingStr, spaceStr, waterStr };
  printTopRow(topRow, THREE, ZERO);

  digitalWrite(waterInletPin, HIGH);
  while (!waterLevelReached) {
    int lowSensor = digitalRead(lowSensorPin);
    int midSensor = digitalRead(midSensorPin);
    int highSensor = digitalRead(highSensorPin);

    if ((selectedWaterLevel == ZERO && lowSensor == LOW) ||
        (selectedWaterLevel == ONE && lowSensor == LOW && midSensor == LOW) ||
        (selectedWaterLevel == TWO && lowSensor == LOW && midSensor == LOW && highSensor == LOW)) {
      waterLevelReached = true;
    }
    delay(1000);
  }
  digitalWrite(waterInletPin, LOW);
}

void incrementStage() {
  stage  = ++stage % noOfStages;
  selectButtonPressed = true;
  upDownButtonPressed = true;
}

void selectValue(int *value, int numberOfValues) {
  int upButtonState = digitalRead(upButtonPin);
  int downButtonState = digitalRead(downButtonPin);
  if (upButtonState == HIGH) {
    *value = ++*value % numberOfValues;
    upDownButtonPressed = true;
    delay(200);
  }
  if (downButtonState == HIGH) {
    *value =  --*value % numberOfValues;
    if (*value < ZERO) {
      *value = *value + numberOfValues;
    }
    upDownButtonPressed = true;
    delay(200);
  }
}

void printWashMode() {
  const char *topRow[FOUR] = { washStr, spaceStr, modeStr, collonStr };
  printTopRowIfSelectButtonPressed(topRow, FOUR, THREE);

  switch (selectedWashMode) {
    case ZERO: {
        const char *bottomRow[THREE] = { washStr, plusStr, rinseStr };
        printBottomRowIfChangeButtonPressed(bottomRow, THREE, THREE);
      }
      break;
    case ONE: {
        const char *bottomRow[ONE] = { washStr };
        printBottomRowIfChangeButtonPressed(bottomRow, ONE, SIX);
      }
      break;
    case TWO: {
        const char *bottomRow[ONE] = { rinseStr };
        printBottomRowIfChangeButtonPressed(bottomRow, ONE, SIX);
      }
      break;
  }
}

void printWaterLevel() {
  const char *topRow[FOUR] = { waterStr, spaceStr, levelStr, collonStr };
  printTopRowIfSelectButtonPressed(topRow, FOUR, TWO);

  switch (selectedWaterLevel) {
    case ZERO: {
        const char *bottomRow[ONE] = { lowStr };
        printBottomRowIfChangeButtonPressed(bottomRow, ONE, SIX);
      }
      break;
    case ONE: {
        const char *bottomRow[ONE] = { mediumStr };
        printBottomRowIfChangeButtonPressed(bottomRow, ONE, FIVE);
      }
      break;
    case TWO: {
        const char *bottomRow[ONE] = { highStr };
        printBottomRowIfChangeButtonPressed(bottomRow, ONE, SIX);
      }
      break;
  }
}

void printWashTime() {
  const char *topRow[FOUR] = { washStr, spaceStr, timeStr, collonStr };
  printTopRowIfSelectButtonPressed(topRow, FOUR, THREE);

  char buffer[TWO];
  const char *bottomRow[THREE] = { itoa(selectedWashTime + ONE, buffer, 10), spaceStr, minStr };
  printBottomRowIfChangeButtonPressed(bottomRow, THREE, FIVE);
}

void printRinseTime() {
  const char *topRow[FOUR] = { rinseStr, spaceStr, timeStr, collonStr };
  printTopRowIfSelectButtonPressed(topRow, FOUR, TWO);

  char buffer[TWO];
  const char *bottomRow[THREE] = { itoa(selectedRinseTime + ONE, buffer, 10), spaceStr, minStr };
  printBottomRowIfChangeButtonPressed(bottomRow, THREE, FIVE);
}

void printTopRowIfSelectButtonPressed(const char **text, int arraySize, int cursorPosition) {
  if (selectButtonPressed) {
    selectButtonPressed = false;
    printTopRow(text, arraySize, cursorPosition);
  }
}

void printBottomRowIfChangeButtonPressed(const char **text, int arraySize, int cursorPosition) {
  if (upDownButtonPressed) {
    upDownButtonPressed = false;
    printBottomRow(text, arraySize, cursorPosition);
  }
}

void printTopRow(const char **text, int arraySize, int cursorPosition) {
  clearRow(ZERO);
  lcd.setCursor(cursorPosition, ZERO);
  printRow(text, arraySize);
}

void printBottomRow(const char **text, int arraySize, int cursorPosition) {
  clearRow(ONE);
  lcd.setCursor(cursorPosition, ONE);
  printRow(text, arraySize);
}

void printRow(const char **text, int arraySize) {
  for (int i = ZERO; i < arraySize; i++) {
    lcd.print(text[i]);
  }
}

void clearRow(int row) {
  for (int  thisCol = ZERO; thisCol < noOfLcdColumns; thisCol++) {
    lcd.setCursor(thisCol, row);
    lcd.write(spaceStr);
  }
}
