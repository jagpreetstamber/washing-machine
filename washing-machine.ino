#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

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
const char collonStr[] = ":";
const char spaceStr[] = " ";
const char plusStr[] = "+";

const int noOfStages = 5;
const int noOfWashModes = 3;
const int noOfWaterLevel = 3;
const int maxWashTime = 15;
const int maxRinseTime = 5;
const int noOfLcdColumns = 16;
const int noOfLcdRows = 2;
const int waterInletPin = 13;
const int selectButtonPin = 19;
const int upButtonPin = 18;
const int downButtonPin = 17;

const int ZERO = 0;
const int ONE = 1;

int stage = ZERO; // 0 - Wash Mode, 1 - Water Level, 2 - Wash Time, 3 - Rinse Time, 4 - Start
int selectedWashMode = ZERO; // 0 - Wash + Rinse, 1 - Wash, 2 - Rinse
int selectedWaterLevel = ONE; // 0 - Low, 1 - Medium, 2 - High
int selectedWashTime = maxWashTime - 1; // In minutes
int selectedRinseTime = maxRinseTime - 1; // In minutes
boolean upDownButtonPressed = true;
boolean selectButtonPressed = true;

void setup() {
  pinMode(selectButtonPin, INPUT);
  pinMode(upButtonPin, INPUT);
  pinMode(downButtonPin, INPUT);
  pinMode(waterInletPin, OUTPUT);
  lcd.begin(noOfLcdColumns, noOfLcdRows);
}

void loop() {
  int selectButtonState = digitalRead(selectButtonPin);
  if (selectButtonState == HIGH) {
    stage = ++stage % noOfStages;
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
  } else if (stage == 2) {
    selectValue(&selectedWashTime, maxWashTime);
    printWashTime();
  } else if (stage == 3) {
    selectValue(&selectedRinseTime, maxRinseTime);
    printRinseTime();
  }
  else if (stage == 4) {
    clearRow(ZERO);
    lcd.setCursor(ZERO, ZERO);
    lcd.print(selectedWashMode);
    lcd.print(selectedWaterLevel);
    lcd.print(selectedWashTime);
    lcd.print(selectedRinseTime);
    delay(300);
  }
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
  const char *topRow[4] = {
    washStr,
    spaceStr,
    modeStr,
    collonStr
  };
  printTopRow(topRow, 4, 3);

  if (selectedWashMode == ZERO) {
    const char *bottomRow[3] = {
      washStr,
      plusStr,
      rinseStr
    };
    printBottomRow(bottomRow, 3, 3);
  } else if (selectedWashMode == ONE) {
    const char *bottomRow[ONE] = {
      washStr
    };
    printBottomRow(bottomRow, ONE, 6);
  } else if (selectedWashMode == 2) {
    const char *bottomRow[ONE] = {
      rinseStr
    };
    printBottomRow(bottomRow, ONE, 6);
  }
}

void printWaterLevel() {
  const char *topRow[4] = {
    waterStr,
    spaceStr,
    levelStr,
    collonStr
  };
  printTopRow(topRow, 4, 2);

  if (selectedWaterLevel == ZERO) {
    const char *bottomRow[ONE] = {
      lowStr
    };
    printBottomRow(bottomRow, ONE, 6);
  } else if (selectedWaterLevel == ONE) {
    const char *bottomRow[1] = {
      mediumStr
    };
    printBottomRow(bottomRow, ONE, 5);
  } else if (selectedWaterLevel == 2) {
    const char *bottomRow[ONE] = {
      highStr
    };
    printBottomRow(bottomRow, ONE, 6);
  }
}

void printWashTime() {
  const char *topRow[4] = {
    washStr,
    spaceStr,
    timeStr,
    collonStr
  };
  printTopRow(topRow, 4, 3);

  char buffer[2];
  const char *bottomRow[3] = {
    itoa(selectedWashTime + ONE, buffer, 10),
    spaceStr,
    minStr
  };
  printBottomRow(bottomRow, 3, 5);
}

void printRinseTime() {
  const char *topRow[4] = {
    rinseStr,
    spaceStr,
    timeStr,
    collonStr
  };
  printTopRow(topRow, 4, 2);

  char buffer[2];
  const char *bottomRow[3] = {
    itoa(selectedRinseTime + ONE, buffer, 10),
    spaceStr,
    minStr
  };
  printBottomRow(bottomRow, 3, 5);
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

