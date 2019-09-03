#include <Arduino.h>
#include <SPI.h>
#include "MCP320X.h"

#define START 1

#define NBCHANNELMCP3204 4
#define NBCHANNELMCP3208 8

#define SINGLEENDEDVALUE 1
#define DIFFERENTIALVALUE 0

uint16_t inputConfiguration;
uint16_t CS;
uint16_t nbChannel = 0;
bool mcpSet = 0;

void mcp320xInit(InputType readType, Type mcpType, uint16_t slavePin) {
  mcpSet = 1;
  switch (readType) {
    case SINGLE:
      inputConfiguration = SINGLEENDEDVALUE;
      break;
    case DIFFERENTIAL:
      inputConfiguration = DIFFERENTIALVALUE;
      break;
    default:
      mcpSet = 0;
      break;
  }

  switch (mcpType) {
    case MCP3204:
      nbChannel = NBCHANNELMCP3204;
      break;
    case MCP3208:
      nbChannel = NBCHANNELMCP3208;
      break;
    default:
      mcpSet = 0;
      break;
  }

  if (slavePin <= 50) {
    CS = slavePin;
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);
  } else {
    mcpSet = 0;
  }

  if (mcpSet) {
    SPI.begin();
    SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  }
}

int16_t mcp320xRead(uint8_t channel) {
  int16_t ans = 0;
  if (mcpSet) {
    if ((channel <= nbChannel) && (channel >= 0)) {
      uint8_t SEND1 = 0x00 | (START << 2) | (inputConfiguration << 1) | (channel >> 2);
      uint16_t SEND2 = 0x00 | (channel << 14);
      digitalWrite(CS, LOW);
      SPI.transfer(SEND1);
      uint16_t value = SPI.transfer16(SEND2) & 0x0FFF;
      digitalWrite(CS, HIGH);
      ans = value;
    } else {
      ans = -1;
    }
  } else {
    ans = -1;
  }
  return ans;
}


