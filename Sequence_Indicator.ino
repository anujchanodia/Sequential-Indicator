#include <FastLED.h>
#define NUM_LEDS 256
#define DATA_PIN 3
#define LEFT_TO_RIGHT 0
#define RIGHT_TO_LEFT 1
#define DIRECTION LEFT_TO_RIGHT
#define BRIGHTNESS 10
#define SPEED 16        // arrow
#define TEXT_SPEED 55   // text
#define ROW 32
#define COL 8
#define CUSTOM_SYMBOL_FLAG '@'
#define ARROW_WIDTH 10

uint8_t charMap_u8a[26][6]={
  {0x1F,0x24,0x44,0x24,0x1F,0x00},  //A
  {0x7F,0x49,0x49,0x49,0x36,0x00},  //B
  {0x3E,0x41,0x41,0x41,0x22,0x00},  //C
  {0x00,0x7F,0x41,0x41,0x41,0x3E},  //D
  {0x7F,0x49,0x49,0x49,0x41,0x00},  //E
  {0x7F,0x48,0x48,0x48,0x40,0x00},  //F
  {0x3E,0x41,0x41,0x45,0x26,0x00},  //G
  {0x7F,0x08,0x08,0x08,0x7F,0x00},  //H
  {0x41,0x41,0x7F,0x41,0x41,0x00},  //I
  {0x02,0x01,0x01,0x01,0x7E,0x00},  //J
  {0x7F,0x08,0x14,0x22,0x41,0x00},  //K
  {0x7F,0x01,0x01,0x01,0x01,0x00},  //L
  {0x7F,0x20,0x10,0x20,0x7F,0x00},  //M
  {0x7F,0x10,0x08,0x04,0x7F,0x00},  //N
  {0x3E,0x41,0x41,0x41,0x3E,0x00},  //O
  {0x7F,0x48,0x48,0x48,0x30,0x00},  //P
  {0x3E,0x41,0x45,0x43,0x3F,0x00},  //Q
  {0x7F,0x48,0x4C,0x4A,0x31,0x00},  //R
  {0x32,0x49,0x49,0x49,0x26,0x00},  //S
  {0x40,0x40,0x7F,0x40,0x40,0x00},  //T
  {0x7E,0x01,0x01,0x01,0x7E,0x00},  //U
  {0x7C,0x02,0x01,0x02,0x7C,0x00},  //V
  {0x7F,0x02,0x04,0x02,0x3F,0x00},  //W
  {0x63,0x14,0x08,0x14,0x63,0x00},  //X
  {0x60,0x10,0x0F,0x10,0x60,0x00},  //Y
  {0x43,0x45,0x49,0x51,0x61,0x00},  //Z
};   

uint8_t intMap_u8a[10][6] ={
  {0x7F,0x41,0x41,0x41,0x7F,0x00},  //0
  {0x11,0x21,0x7F,0x01,0x01,0x00},  //1
  {0x21,0x41,0x43,0x45,0x39,0x00},  //2
  {0x22,0x41,0x49,0x49,0x36,0x00},  //3
  {0x0C,0x14,0x24,0x7F,0x04,0x00},  //4
  {0x79,0x49,0x49,0x49,0x46,0x00},  //5
  {0x1E,0x29,0x49,0x49,0x46,0x00},  //6
  {0x41,0x42,0x44,0x48,0x70,0x00},  //7
  {0x36,0x49,0x49,0x49,0x36,0x00},  //8
  {0x32,0x49,0x49,0x49,0x3E,0x00},  //9
};

// Left arrow bitmap that points left (cols 0-9)
uint8_t ArrowLeft [1][ARROW_WIDTH] = {
  0x08,0x1C,0x3E,0x5D,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C
};

// Right arrow bitmap that points right (cols 22-31)
uint8_t ArrowRight [1][ARROW_WIDTH] = {
  0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x5D,0x3E,0x1C,0x08
};

uint8_t CruiseControlSymbolRight[1][8] = { 
  0x50,0x33,0x74,0x0C,0x0A,0x09,0x04,0x03
};

uint8_t CruiseControlSymbolLeft[1][8] = { 
  0x03,0x04,0x09,0x0A,0x0C,0x74,0x33,0x50
};

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

CRGB leds[NUM_LEDS];

uint8_t ledString_u8a[ROW] = {0};
bool isDigitRed_u8a[ROW] = {false};
char strToPrint[100] = "";
uint8_t sizeOfstrToPrint = 12;

uint8_t currentSpeed = 55;
uint8_t speedLimit = 50;

enum ArrowState {
  ARROW_OFF,
  ARROW_GROWING,
  ARROW_FULL
};

ArrowState arrowState = ARROW_OFF;
unsigned long lastArrowAnimTime = 0;
uint8_t currentArrowCol = 0;
unsigned long lastTextScrollTime = 0;  // Separate timer for text scrolling

void setup(){
  FastLED.addLeds<WS2812,DATA_PIN,RGB>(leds,NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

void loop() {
  unsigned long currentMillis = millis();
  
  updateScrollText();              // Update the speed + cruise symbol text

  clearLED();                      // Clear display buffer

  
  if (currentMillis - lastTextScrollTime >= TEXT_SPEED) {
    // Print text according to direction - scrolls in columns 11-21
    switch (DIRECTION) {
      case LEFT_TO_RIGHT:
        printScrollingTextLeftToRight();
        break;
      case RIGHT_TO_LEFT:
        printScrollingTextRightToLeft();
        break;
    }
    lastTextScrollTime = currentMillis;
  }

  displayLED();                    // Draw text to LED array (NO FastLED.show)

  updateDualArrowAnimation();      // Handle both arrows animation simultaneously

  FastLED.show();                  // ONLY called once per frame at the end

  delay(SPEED);                    // Control overall frame rate (16ms for smooth arrows)
}

void printScrollingTextLeftToRight() {
  static int8_t strElement_u8 = -1;
  static int8_t counter_u8 = -1;
  static char currentChar = ' ';

  // Shift display buffer LEFT (scrolling text L→R)
  for (uint8_t i = 0; i < ROW - 1; i++) {
    ledString_u8a[i] = ledString_u8a[i + 1];
    isDigitRed_u8a[i] = isDigitRed_u8a[i + 1];  // Shift red flag array too
  }
  ledString_u8a[ROW - 1] = 0x00;
  isDigitRed_u8a[ROW - 1] = false;

  // Load next character if done with previous
  if (counter_u8 < 0) {
    strElement_u8++;
    if (strElement_u8 >= sizeOfstrToPrint) strElement_u8 = 0;
    currentChar = strToPrint[strElement_u8];
    counter_u8 = (currentChar == CUSTOM_SYMBOL_FLAG) ? 7 : 5;
  }

  uint8_t columnByte = 0;

  if ((currentChar >= '0') && (currentChar <= '9')) {
    uint8_t digitIndex = currentChar - '0';
    columnByte = intMap_u8a[digitIndex][5 - counter_u8];
  } else if (currentChar == CUSTOM_SYMBOL_FLAG) {
    columnByte = CruiseControlSymbolRight[0][counter_u8];
    isDigitRed_u8a[ROW - 1] = true;  // Mark cruise symbol as red
  } else if (currentChar == ' ') {
    columnByte = 0x00;
  }

  ledString_u8a[ROW - 1] = columnByte;
  counter_u8--;
}

void printScrollingTextRightToLeft() {
  static uint8_t strElement_u8 = 0;
  static uint8_t counter_u8 = 0;
  static char currentChar = ' ';

  // Shift buffer LEFT (scrolling R→L)
  for (uint8_t i = 0; i < ROW - 1; i++) {
    ledString_u8a[i] = ledString_u8a[i + 1];
    isDigitRed_u8a[i] = isDigitRed_u8a[i + 1];
  }
  ledString_u8a[ROW - 1] = 0;
  isDigitRed_u8a[ROW - 1] = false;

  currentChar = strToPrint[strElement_u8];

  uint8_t columnByte = 0;
  uint8_t charWidth = (currentChar == CUSTOM_SYMBOL_FLAG) ? 8 : 6;

  if ((currentChar >= '0') && (currentChar <= '9')) {
    uint8_t digitIndex = currentChar - '0';
    columnByte = intMap_u8a[digitIndex][counter_u8];
    if (currentSpeed > speedLimit) isDigitRed_u8a[ROW - 1] = true;
  } else if (currentChar == CUSTOM_SYMBOL_FLAG) {
    columnByte = CruiseControlSymbolLeft[0][counter_u8];
    isDigitRed_u8a[ROW - 1] = true;  // Cruise symbol is permanently red
  } else if (currentChar == ' ') {
    columnByte = 0x00;
  }

  ledString_u8a[ROW - 1] = columnByte;

  counter_u8++;
  if (counter_u8 >= charWidth) {
    counter_u8 = 0;
    strElement_u8++;
    if (strElement_u8 >= sizeOfstrToPrint) strElement_u8 = 0;
  }
}

void displayLED() {
  uint8_t ledPos_u8 = 0;

  // Only display text in columns 11-21 (middle area)
  for (uint8_t row_u8 = 11; row_u8 <= 21; row_u8++) {
    for (uint8_t col_u8 = 0; col_u8 < COL; col_u8++) {
      if (ledString_u8a[row_u8] & (1 << (7 - col_u8))) {
        CRGB color;
        
        // Check if this column contains cruise control symbol (permanently red)
        if (isDigitRed_u8a[row_u8]) {
          color = CRGB::Red;  // Cruise symbol is always red
        } else {
          // Speed digits follow speed limit logic
          color = (currentSpeed > speedLimit) ? CRGB::Green : CRGB::Red;
        }

        ledPos_u8 = (row_u8 * COL) + ((row_u8 & 1) ? (COL - 1 - col_u8) : col_u8);
        leds[ledPos_u8] = color;
      }
    }
  }
  // NO FastLED.show() here - only called once at end of loop()
}

void clearLED() {
  for (uint8_t row_u8 = 0; row_u8 < ROW; row_u8++) {
    for (uint8_t col_u8 = 0; col_u8 < COL; col_u8++) {
      uint8_t ledPos_u8 = (row_u8 * COL) + ((row_u8 & 1) ? (COL - 1 - col_u8) : col_u8);
      leds[ledPos_u8] = CRGB::Black;
    }
  }
}

void displayBothArrows() {
  uint8_t ledPos_u8 = 0;

  // Display left arrow in columns 0-9
  for (uint8_t col_u8 = 0; col_u8 < ARROW_WIDTH; col_u8++) {
    uint8_t arrowByte = ArrowLeft[0][col_u8];
    uint8_t x = col_u8; // columns 0-9

    for (uint8_t rowBit = 0; rowBit < 8; rowBit++) {
      if ((arrowByte << rowBit) & 0x80) {
        uint8_t y = rowBit;

        if (x % 2 == 0)
          ledPos_u8 = x * COL + y;
        else
          ledPos_u8 = x * COL + (COL - 1 - y);

        leds[ledPos_u8] = CRGB(120, 255, 0); // Amber
      }
    }
  }

  // Display right arrow in columns 22-31
  for (uint8_t col_u8 = 0; col_u8 < ARROW_WIDTH; col_u8++) {
    uint8_t arrowByte = ArrowRight[0][col_u8];
    uint8_t x = 22 + col_u8; // columns 22-31

    for (uint8_t rowBit = 0; rowBit < 8; rowBit++) {
      if ((arrowByte << rowBit) & 0x80) {
        uint8_t y = rowBit;

        if (x % 2 == 0)
          ledPos_u8 = x * COL + y;
        else
          ledPos_u8 = x * COL + (COL - 1 - y);

        leds[ledPos_u8] = CRGB(120, 255, 0); // Amber
      }
    }
  }
}

void clearBothArrows() {
  uint8_t ledPos_u8 = 0;

  // Clear left arrow area (columns 0-9)
  for (uint8_t col_u8 = 0; col_u8 < ARROW_WIDTH; col_u8++) {
    uint8_t x = col_u8;

    for (uint8_t rowBit = 0; rowBit < 8; rowBit++) {
      uint8_t y = rowBit;

      if (x % 2 == 0)
        ledPos_u8 = x * COL + y;
      else
        ledPos_u8 = x * COL + (COL - 1 - y);

      leds[ledPos_u8] = CRGB::Black;
    }
  }

  // Clear right arrow area (columns 22-31)
  for (uint8_t col_u8 = 0; col_u8 < ARROW_WIDTH; col_u8++) {
    uint8_t x = 22 + col_u8;

    for (uint8_t rowBit = 0; rowBit < 8; rowBit++) {
      uint8_t y = rowBit;

      if (x % 2 == 0)
        ledPos_u8 = x * COL + y;
      else
        ledPos_u8 = x * COL + (COL - 1 - y);

      leds[ledPos_u8] = CRGB::Black;
    }
  }
}

void updateScrollText() {
  sprintf(strToPrint, " %d  %c ", currentSpeed, CUSTOM_SYMBOL_FLAG);
  sizeOfstrToPrint = strlen(strToPrint);
}

void updateDualArrowAnimation() { 
  unsigned long currentMillis = millis();

  switch (arrowState) {
    case ARROW_OFF:
      // OFF time: 400 ms
      if (currentMillis - lastArrowAnimTime >= 400) {
        arrowState = ARROW_GROWING;
        currentArrowCol = 0;
        lastArrowAnimTime = currentMillis;
      }
      break;

    case ARROW_GROWING:
      // Grow both arrows column-by-column every 16 ms (10 columns = 160 ms total)
      if (currentMillis - lastArrowAnimTime >= 16) {
        drawBothArrowColumns(currentArrowCol);
        currentArrowCol++;
        lastArrowAnimTime = currentMillis;

        if (currentArrowCol >= 10) {
          arrowState = ARROW_FULL;
          lastArrowAnimTime = currentMillis;
        }
      }
      break;

    case ARROW_FULL:
      // Display both complete arrows during the full phase
      displayBothArrows();
      
      // Hold both arrows fully visible for 240 ms (160ms grow + 240ms hold = 400ms total ON time)
      if (currentMillis - lastArrowAnimTime >= 300) {
        clearBothArrows();
        arrowState = ARROW_OFF;
        lastArrowAnimTime = currentMillis;
      }
      break;
  }
}

void drawBothArrowColumns(uint8_t colIndex) {
  uint8_t ledPos_u8 = 0;

  // XUV700-Style Sequential Indicator - Draw ALL columns from 0 to current colIndex
  
  // Left arrow: Draw all columns from tip to current position (RIGHT to LEFT flow)
  for (uint8_t i = 0; i <= colIndex; i++) {
    uint8_t leftArrowByte = ArrowLeft[0][9 - i]; // Reverse index for RTL growth
    uint8_t leftX = 9 - i; // Start from column 9, go to 0

    for (uint8_t rowBit = 0; rowBit < 8; rowBit++) {
      if ((leftArrowByte << rowBit) & 0x80) {
        uint8_t y = rowBit;

        if (leftX % 2 == 0)
          ledPos_u8 = leftX * COL + y;
        else
          ledPos_u8 = leftX * COL + (COL - 1 - y);

        leds[ledPos_u8] = CRGB(120, 255, 0);  // Bright amber
      }
    }
  }

  // Right arrow: Draw all columns from tip to current position (LEFT to RIGHT flow)
  for (uint8_t i = 0; i <= colIndex; i++) {
    uint8_t rightArrowByte = ArrowRight[0][i]; // Normal index for LTR growth
    uint8_t rightX = 22 + i; // Start from column 22, go to 31

    // Bounds safety check
    if (rightX >= ROW) continue;

    for (uint8_t rowBit = 0; rowBit < 8; rowBit++) {
      if ((rightArrowByte << rowBit) & 0x80) {
        uint8_t y = rowBit;

        if (rightX % 2 == 0)
          ledPos_u8 = rightX * COL + y;
        else
          ledPos_u8 = rightX * COL + (COL - 1 - y);

        leds[ledPos_u8] = CRGB(120, 255, 0);  // Bright amber
      }
    }
  }
}