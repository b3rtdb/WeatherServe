#ifndef MCP320X_H
#define MCP320X_H

enum Type {
  MCP3204,
  MCP3208
};

enum InputType {
  SINGLE,
  DIFFERENTIAL
};

void mcp320xInit(InputType readType, Type mcpType, uint16_t slavePin);
int16_t mcp320xRead(uint8_t channel);


#endif
