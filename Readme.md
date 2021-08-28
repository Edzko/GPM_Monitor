# GPM Applications

Companion Applications for Magna RTK-GPS UBlox & Vibration module.

## GPM Monitor Windows User Interface

![GUI](https://github.com/Edzko/GPM_Monitor/blob/master/GPM_Monitor/hlp/images/gpmgui.png)

[Help file](GPM_Monitor/hlp/GPM_Monitor.chm)

## GPM Update Lite

Console-based firmware upgrade utility. Run this utility on the same [Wifi] network as the UBlox module. The utility can discover the module on the network, enter update firmware based on a firmware hex file provided in the command line.

	GPM_UpdateLite [<ip-address>] <firmware.bin>

Note: without an IP address provided, the App will connect to the first available module it finds.

## GPM Vibration App

![GUI](https://github.com/Edzko/GPM_Monitor/blob/master/GPM_Vibration/hlp/images/gpmgui.png)

## GPM Matlab

The Matlab MEX function `acc.mexw64` can be used on the command line or in a Simulink model to capture vibration data.

![GUI](https://github.com/Edzko/GPM_Monitor/blob/master/GPM_Matlab/acc/img/Simulink.png)

![GUI](https://github.com/Edzko/GPM_Monitor/blob/master/GPM_Matlab/acc/img/AccChart.png)


## GPM Command

Console-based joystick control utility to command a wireless vehicle using the UBlox module. Run this utility on the same [Wifi] network as the UBlox module. A Windows-based USB joystick or game controller must be connected to the host computer.

The utility can discover the module on the network. Or the IP address can be specified on the command line.

	GPM_Command [<ip-address>]

*This utility is not yet available.*


----------
For more information contact [Edzko.Smid@magna.com ](Edzko.Smid@magna.com)