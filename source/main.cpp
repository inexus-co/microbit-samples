/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBit.h"
#include "MicroBitUARTService.h"

MicroBit uBit;
MicroBitUARTService *uart;
MicroBitThermometer thermometer;

int connected = 0;

const char *space = " ";

ManagedString DISPLAY("DISPLAY");
ManagedString TEMP("TEMP");


void onConnected(MicroBitEvent)
{
    connected = 1;

    ManagedString eom(";");

    while(connected == 1) {
        ManagedString msg = uart->readUntil(eom);
        int length = msg.length();
        for(int i = 0; i < length; ++i) {
            if(msg.charAt(i) == *space){
                ManagedString command = msg.substring(0, i);
                ManagedString data = msg.substring(i, length - 1);

                if(command == DISPLAY) {
                    uBit.display.scroll(data);
                } else if (command == TEMP) {
                    int temperature = thermometer.getTemperature();
                    uBit.display.scroll(temperature);
                } else {
                    uBit.display.scroll(msg);
                }

                break;
            }
        }
    }
}

void onDisconnected(MicroBitEvent)
{
    uBit.display.print("D");
    connected = 0;
}

int main()
{
    uBit.init();

    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);

    uart = new MicroBitUARTService(*uBit.ble, 32, 32);
    uBit.display.print("O");

    release_fiber();
}

