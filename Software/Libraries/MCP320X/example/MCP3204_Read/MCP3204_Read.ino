#include <SPI.h>
#include "MCP320X.h"

uint8_t channel = 0;
const int PINSLAVE = 8;

void setup() {
  Serial.begin(115200);
  mcp320xInit(SINGLE, MCP3204, PINSLAVE);
}

void loop() {
  delay(100);
  uint16_t value = mcp320xRead(channel);
  Serial.println(value);
}

