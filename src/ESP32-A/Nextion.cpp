#pragma once
#include "Nextion.h"

// ----------------------- Nextion Buffer Setup -----------------------
Nextion::Nextion(HardwareSerial& port) : port(port), bufIndex(0) {}

void Nextion::begin()
{
    port.begin(9600, SERIAL_8N1, NEXTION_RX_PIN, NEXTION_TX_PIN);
}

String Nextion::getCommand()
{
    while (port.available() > 0) {
        uint8_t b = port.read();
        if (bufIndex < NEXTION_BUF_SIZE) {
            buf[bufIndex++] = b;
        }
        if (bufIndex >= 3 &&
            buf[bufIndex - 1] == 0xFF &&
            buf[bufIndex - 2] == 0xFF &&
            buf[bufIndex - 3] == 0xFF) {
                String cmd = "";
                for (size_t i = 0; i < bufIndex - 3; i++) {
                    cmd += (char)buf[i];
                }
                cmd.trim();
                bufIndex = 0;
                Serial.println(cmd);
                return cmd;
            }
        }
    return "";
}
void Nextion::sendStatus(const bool isRunning, const float sensorFreq, const float motorFreq) 
{
    String cmd = "t4.txt=\"";
    cmd += (isRunning ? "Running" : "Standby");
    cmd += "\"";
    port.print(cmd);
    port.write(0xFF);
    port.write(0xFF);
    port.write(0xFF);
    
    cmd = "bt0.val=";
    cmd += String(isRunning);
    port.print(cmd);
    port.write(0xFF);
    port.write(0xFF);
    port.write(0xFF);

    cmd = "bt0.txt=\"";
    cmd += (isRunning ? "Stop" : "Run");
    cmd += "\"";
    port.print(cmd);
    port.write(0xFF);
    port.write(0xFF);
    port.write(0xFF);

    cmd = "x0.val=";
    cmd += String((int)(sensorFreq * 10));
    port.print(cmd);
    port.write(0xFF);
    port.write(0xFF);
    port.write(0xFF);

    cmd = "x1.val=";
    cmd += String((int)(motorFreq/69));
    port.print(cmd);
    port.write(0xFF);
    port.write(0xFF);
    port.write(0xFF);
}