# Auxiliary Saline Delivery Pump System
Modular ESP32 firmware for real-time stepper motor control with support for multiple input interfaces and feedback mechanisms.   
## Development Details
### Firmware
- **Board**: ESP32-WROOM-32  
- **Firmware Language**: C++ (Arduino framework)  
- **Firmware IDE**: PlatformIO  
- **Platform**: Espressif32 @ 3.5.0   
### GUI
- **HMI Display**: Nextion NX4832K035 (3.5" Enhanced)  
- **HMI IDE**: [Nextion Editor](https://nextion.tech/nextion-editor/)
## Software development process
1.  Install PlatformIO inside Visual Studio Code and restart Visual Studio Code to apply change
2.  Navigate to PlatformIO homepage through the home icon on the bottom toolbar   
3.  In PlatformIO, navigate to Platforms -> Embedded   
4.  Search for ‘Espressif 32’ and choose version 3.5.0 to install (Warning: installing versions other than 3.5.0 could result in performance issue of pump)   
5.	Use Git or GitHub Desktop to clone the following repository: https://github.com/uf-ippd/Team-02.git   
6.	Open project using ‘Open Project’ tab on home page of PlatformIO   
7.	Download and unzip CP210x Driver that fits your OS through CP210x USB to UART Bridge VCP Drivers Download Page. https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads    
8.	Connect the ESP32 with label ‘motor’ with your PC.   
9.	Open device manager and locate CP2102 device that may be under Ports (COM & LPT) or other devices.   
10.	Right-click on the CP2102 device and select “Update driver”.   
11.	Choose “Browse my computer for driver software” and then navigate to the folder where the CP2102 driver is unzipped.   
12.	Complete the on-screen instructions to complete driver installation.   
13.	Check Device Manager to ensure the CP2102 device now has updated driver by checking that device is now recognized with a port name ‘COM#’ and note the COM port number. (If using MacOS, run ‘ls /dev/tty.*’ to list all serial ports, note the port that may be shown as ‘/dev/tty.SLAB_USBtoUART’ or ‘/dev/tty.usbserial-0001.’)
14.	In platform.ini file of the project, make sure that the section labelled ‘[env:motor_esp32]’ is uncommented, change the upload_port variable using the serial port identified in step m. (e.g. COM3,‘/dev/tty.SLAB_USBtoUART’ or ‘/dev/tty.usbserial-0001’) then compile and upload using the icon on the top right of the screen. (Note: if terminal stuck at ‘Connecting…___...’, press RST button on esp32 once, or long-press boot button to reset flash.)   
15.	Unplug the ESP32 labelled ‘motor’(software is stored on ESP32’s non-volatile memory) and plug in the ESP32 labelled ‘sensor’.   
16.	Repeat steps m-n but comment out the [env:motor_esp32] section and uncomment the [env:sensor_ui_esp32], make suire to change upload_port.   
17.	Install the ESP32s to correspond to the labeled PCB (red protoboards)   
