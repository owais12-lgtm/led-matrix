#include <Arduino.h>

const uint8_t ROW_PINS[6] = { 2, 3, 4, 5, 6, 7 };
const uint8_t COL_PINS[6] = { 8, 9, 10, 16, 14, 15 };

const uint8_t NUM_ROWS = 6;
const uint8_t NUM_COLS = 6;

bool frame[NUM_ROWS][NUM_COLS];

const uint16_t ROW_ON_TIME_US = 400;

void setup() {
  for (uint8_t i = 0; i < NUM_ROWS; i++) {
    pinMode(ROW_PINS[i], INPUT);
  }
  for (uint8_t i = 0; i < NUM_COLS; i++) {
    pinMode(COL_PINS[i], INPUT);
  }
  clearAll();
  randomSeed(analogRead(A0));
}

void loop() {
  runForMillis(2000, effectChaseColumns);
  runForMillis(2000, effectChaseRows);
  runForMillis(2500, effectWaveFill);
  runForMillis(2000, effectSparkle);
  runForMillis(1500, effectBlinkAll);
}

void refreshMatrix() {
  for (uint8_t r = 0; r < NUM_ROWS; r++) {
    for (uint8_t i = 0; i < NUM_ROWS; i++) pinMode(ROW_PINS[i], INPUT);
    for (uint8_t i = 0; i < NUM_COLS; i++) pinMode(COL_PINS[i], INPUT);

    pinMode(ROW_PINS[r], OUTPUT);
    digitalWrite(ROW_PINS[r], LOW);

    for (uint8_t c = 0; c < NUM_COLS; c++) {
      if (frame[r][c]) {
        pinMode(COL_PINS[c], OUTPUT);
        digitalWrite(COL_PINS[c], HIGH);
      }
    }

    delayMicroseconds(ROW_ON_TIME_US);

    for (uint8_t c = 0; c < NUM_COLS; c++) {
      pinMode(COL_PINS[c], INPUT);
    }
    pinMode(ROW_PINS[r], INPUT);
  }
}

void runForMillis(uint16_t durationMs, void (*effectStep)(unsigned long)) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    effectStep(millis() - start);
    unsigned long stepStart = millis();
    while (millis() - stepStart < 60) {
      refreshMatrix();
    }
  }
}

void clearAll() {
  for (uint8_t r = 0; r < NUM_ROWS; r++)
    for (uint8_t c = 0; c < NUM_COLS; c++)
      frame[r][c] = false;
}

void setPixel(uint8_t row, uint8_t col, bool state) {
  if (row < NUM_ROWS && col < NUM_COLS) frame[row][col] = state;
}

void effectChaseColumns(unsigned long elapsedMs) {
  uint8_t active = (elapsedMs / 150) % NUM_COLS;
  clearAll();
  for (uint8_t r = 0; r < NUM_ROWS; r++) setPixel(r, active, true);
}

void effectChaseRows(unsigned long elapsedMs) {
  uint8_t active = (elapsedMs / 150) % NUM_ROWS;
  clearAll();
  for (uint8_t c = 0; c < NUM_COLS; c++) setPixel(active, c, true);
}

void effectWaveFill(unsigned long elapsedMs) {
  uint8_t litCols = (elapsedMs / 100) % (NUM_COLS + 1);
  clearAll();
  for (uint8_t c = 0; c < litCols; c++)
    for (uint8_t r = 0; r < NUM_ROWS; r++)
      setPixel(r, c, true);
}

void effectSparkle(unsigned long elapsedMs) {
  static unsigned long lastUpdate = 0;
  if (elapsedMs - lastUpdate > 80) {
    lastUpdate = elapsedMs;
    clearAll();
    for (uint8_t i = 0; i < 8; i++) {
      setPixel(random(NUM_ROWS), random(NUM_COLS), true);
    }
  }
}

void effectBlinkAll(unsigned long elapsedMs) {
  bool on = (elapsedMs / 300) % 2 == 0;
  for (uint8_t r = 0; r < NUM_ROWS; r++)
    for (uint8_t c = 0; c < NUM_COLS; c++)
      setPixel(r, c, on);
}
