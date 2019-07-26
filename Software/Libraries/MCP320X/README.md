# MCP320X
Arduino library for **MCP3204** and **MCP3208**

## List of Function
### mcp320xInit(readType, mcpType, slavePin);
* readType : chose if you want Single-ended or differential value 
 <br>You have to chose between SINGLE or DIFFERENTIAL
* mcpType : chose the device you use between MCP3204 and MCP3208.
* slavePin : chose your SPI CS pin

### mcp320xRead(channel)
* channel : chose the channel for the MCP to read the 12bit analogic value
