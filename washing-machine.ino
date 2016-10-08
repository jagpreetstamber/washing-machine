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

const int noOfStages = 5;
const int noOfWashModes = 3;
const int noOfWaterLevel = 3;
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

int stage = ZERO; // 0 - Wash Mode, 1 - Water Level, 2 - Wash Time, 3 - Rinse Time, 4 - Start
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
    stage  = ++stage % noOfStages;
    selectButtonPressed = true;
    upDownButtonPressed = true;
    delay(200);
  }

  if (stage == ZERO) {
    selectValue(&selectedWashMode, noOfWashModes);
    printWashMode();
  } else if (stage == ONE) {
    selectValue(&selectedWaterLevel, noOfWaterLevel);
    printWaterLevel();
  } else if (stage == TWO) {
    if (selectedWashMode == TWO) {
      stage++;
    } else {
      selectValue(&selectedWashTime, maxWashTime);
      printWashTime();
    }
  } else if (stage == THREE) {
    if (selectedWashMode == ONE) {
      stage++;
    } else {
      selectValue(&selectedRinseTime, maxRinseTime);
      printRinseTime();
    }
  }
  else if (stage == FOUR) {
    drainWater();
    fillUpWater();

    while(true);
  }
}

void startWashCycle() {
  clearRow(ZERO);
  clearRow(ONE);
  lcd.setCursor(ONE, ZERO);
  lcd.print(drainStr);
  lcd.print(spaceStr);
  lcd.print(waterStr);
  while (digitalRead(drainSensorPin) == LOW) {
    digitalWrite(waterDrainPin, HIGH);
    delay(5000);
  }
  digitalWrite(waterDrainPin, LOW);
  waterLevelReached = false;
}

void drainWater() {
  clearRow(ZERO);
  clearRow(ONE);
  lcd.setCursor(ONE, ZERO);
  lcd.print(drainStr);
  lcd.print(spaceStr);
  lcd.print(waterStr);
  while (digitalRead(drainSensorPin) == LOW) {
    digitalWrite(waterDrainPin, HIGH);
    delay(5000);
  }
  digitalWrite(waterDrainPin, LOW);
  waterLevelReached = false;
}

void fillUpWater() {
  clearRow(ZERO);
  clearRow(ONE);
  lcd.setCursor(ZERO, ZERO);
  lcd.print(fillingStr);
  lcd.print(spaceStr);
  lcd.print(waterStr);
  digitalWrite(waterInletPin, HIGH);
  while (!waterLevelReached) {
    int lowSensor = digitalRead(lowSensorPin);
    int midSensor = digitalRead(midSensorPin);
    int highSensor = digitalRead(highSensorPin);

    if (selectedWaterLevel == ZERO && lowSensor == LOW) {
      waterLevelReached = true;
    }
    if (selectedWaterLevel == ONE && lowSensor == LOW
        && midSensor == LOW) {
      waterLevelReached = true;
    }
    if (selectedWaterLevel == TWO && lowSensor == LOW
        && midSensor == LOW && highSensor == LOW) {
      waterLevelReached = true;
    }
    delay(1000);
  }
  digitalWrite(waterInletPin, LOW);
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
  const char *topRow[FOUR] = {
    washStr,
    spaceStr,
    modeStr,
    collonStr
  };
  printTopRow(topRow, FOUR, THREE);

  if (selectedWashMode == ZERO) {
    const char *bottomRow[THREE] = {
      washStr,
      plusStr,
      rinseStr
    };
    printBottomRow(bottomRow, THREE, THREE);
  } else if (selectedWashMode == ONE) {
    const char *bottomRow[ONE] = {
      washStr
    };
    printBottomRow(bottomRow, ONE, SIX);
  } else if (selectedWashMode == TWO) {
    const char *bottomRow[ONE] = {
      rinseStr
    };
    printBottomRow(bottomRow, ONE, SIX);
  }
}

void printWaterLevel() {
  const char *topRow[FOUR] = {
    waterStr,
    spaceStr,
    levelStr,
    collonStr
  };
  printTopRow(topRow, FOUR, TWO);

  if (selectedWaterLevel == ZERO) {
    const char *bottomRow[ONE] = {
      lowStr
    };
    printBottomRow(bottomRow, ONE, SIX);
  } else if (selectedWaterLevel == ONE) {
    const char *bottomRow[ONE] = {
      mediumStr
    };
    printBottomRow(bottomRow, ONE, FIVE);
  } else if (selectedWaterLevel == TWO) {
    const char *bottomRow[ONE] = {
      highStr
    };
    printBottomRow(bottomRow, ONE, SIX);
  }
}

void printWashTime() {
  const char *topRow[FOUR] = {
    washStr,
    spaceStr,
    timeStr,
    collonStr
  };
  printTopRow(topRow, FOUR, THREE);

  char buffer[TWO];
  const char *bottomRow[THREE] = {
    itoa(selectedWashTime + ONE, buffer, 10),
    spaceStr,
    minStr
  };
  printBottomRow(bottomRow, THREE, FIVE);
}

void printRinseTime() {
  const char *topRow[FOUR] = {
    rinseStr,
    spaceStr,
    timeStr,
    collonStr
  };
  printTopRow(topRow, FOUR, TWO);

  char buffer[TWO];
  const char *bottomRow[THREE] = {
    itoa(selectedRinseTime + ONE, buffer, 10),
    spaceStr,
    minStr
  };
  printBottomRow(bottomRow, THREE, FIVE);
}

void printTopRow(const char **text, int arraySize, int cursorPosition) {
  if (selectButtonPressed) {
    selectButtonPressed = false;
    clearRow(ZERO);
    lcd.setCursor(cursorPosition, ZERO);
    printRow(text, arraySize);
  }
}

void printBottomRow(const char **text, int arraySize, int cursorPosition) {
  if (upDownButtonPressed) {
    upDownButtonPressed = false;
    clearRow(ONE);
    lcd.setCursor(cursorPosition, ONE);
    printRow(text, arraySize);
  }
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

