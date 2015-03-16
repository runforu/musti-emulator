On Symbian platform, the RDebug class is designed to print debug information, but the debug information is output to default serial communication.
This project has three parts:
1. A driver to capture the RDebug information in Symbian OS.
2. A Symbian application read debug information from driver.
3. A PC client application show the debug information through USB or Bluetooth.