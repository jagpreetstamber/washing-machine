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
const char collonChar = ':';
const char spaceChar = ' ';
const char plusChar = '+';

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

int stage = 0; // 0 - Wash Mode, 1 - Water Level, 2 - Wash Time, 3 - Rinse Time, 4 - Start
int selectedWashMode = 0; // 0 - Wash + Rinse, 1 - Wash, 2 - Rinse
int selectedWaterLevel = 1; // 0 - Low, 1 - Medium, 2 - High
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
  if (stage == noOfStages || stage == 0) {
    selectValue(&selectedWashMode, noOfWashModes);
    printWashMode();
  } else if (stage == 1) {
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
    clearRow(0);
    lcd.setCursor(0, 0);
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
    if (*value < 0) {
      *value = *value + numberOfValues;
    }
    upDownButtonPressed = true;
    delay(200);
  }
}

void printWashMode() {
  const char *topRow[4] = {
    washStr,
    &spaceChar,
    modeStr,
    &collonChar
  };
  printTopRow(topRow, 4);

  if (upDownButtonPressed) {
    upDownButtonPressed = false;
    clearRow(1);
    lcd.setCursor(0, 1);
    if (selectedWashMode == 0) {
      lcd.print(washStr);
      lcd.print(plusChar);
      lcd.print(rinseStr);
    } else if (selectedWashMode == 1) {
      lcd.print(washStr);
    } else if (selectedWashMode == 2) {
      lcd.print(rinseStr);
    }
  }
}

void printWaterLevel() {
  const char *topRow[4] = {
    waterStr,
    &spaceChar,
    levelStr,
    &collonChar
  };
  printTopRow(topRow, 4);

  if (upDownButtonPressed) {
    upDownButtonPressed = false;
    clearRow(1);
    lcd.setCursor(0, 1);
    if (selectedWaterLevel == 0) {
      lcd.print(lowStr);
    } else if (selectedWaterLevel == 1) {
      lcd.print(mediumStr);
    } else if (selectedWaterLevel == 2) {
      lcd.print(highStr);
    }
  }
}

void printWashTime() {
  const char *topRow[4] = {
    washStr,
    &spaceChar,
    timeStr,
    &collonChar
  };
  printTopRow(topRow, 4);

  if (upDownButtonPressed) {
    upDownButtonPressed = false;
    clearRow(1);
    lcd.setCursor(0, 1);
    lcd.print(selectedWashTime + 1);
    lcd.write(spaceChar);
    lcd.write(minStr);
  }
}

void printRinseTime() {
  const char *topRow[4] = {
    rinseStr,
    &spaceChar,
    timeStr,
    &collonChar
  };
  printTopRow(topRow, 4);
  //  printBottomRow(text, 3);

  if (upDownButtonPressed) {
    upDownButtonPressed = false;
    clearRow(1);
    lcd.setCursor(0, 1);
    lcd.print(selectedRinseTime + 1);
    lcd.write(spaceChar);
    lcd.write(minStr);
  }
}

void printTopRow(const char **text, int arraySize) {
  if (selectButtonPressed) {
    selectButtonPressed = false;
    clearRow(0);
    lcd.setCursor(0, 0);
    printRow(text, arraySize);
  }
}

void printBottomRow(const char **text, int arraySize) {
  if (upDownButtonPressed) {
    upDownButtonPressed = false;
    clearRow(1);
    lcd.setCursor(0, 1);
    printRow(text, arraySize);
  }
}

void printRow(const char **text, int arraySize) {
  for (int i = 0; i < arraySize; i++) {
    lcd.print(text[i]);
  }
}

void clearRow(int row) {
  for (int  thisCol = 0; thisCol < noOfLcdColumns; thisCol++) {
    lcd.setCursor(thisCol, row);
    lcd.write(spaceChar);
  }
}

