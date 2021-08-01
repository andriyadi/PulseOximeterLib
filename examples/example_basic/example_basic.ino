//
// Created by Andri Yadi on 26/07/21.
//

#ifndef PULSEOXIRELAY_EXP_BMC1000_HPP
#define PULSEOXIRELAY_EXP_BMC1000_HPP

#include "Arduino.h"
#include <BM1000Device.h>

#define SERVICE_UUID "49535343-fe7d-4ae5-8fa9-9fafd205e455"
#define CHARS_UUID "49535343-1e4d-4bd9-ba61-23c647249616"
#define BM1000_ADDR "00:a0:50:db:83:94"

// Pins definition, only if you solder cables on "ON" button and VCC of Oximeter, to be connected to ESP32 GPIOs
// These pins are ESP32 GPIO pins
#define PIN_BM1000_CONTROL_POWER    22
#define PIN_BM1000_CONTROL_ON       19

OximeterIdentifiers *oxiIds;
BM1000Device *bmcDevice;

void setup() {
    delay(3000);
    Serial.begin(115200);

    Serial.println("It begins");

    oxiIds = new OximeterIdentifiers (SERVICE_UUID, CHARS_UUID, BM1000_ADDR);
    bmcDevice = new BM1000Device(*oxiIds);
    log_i("Turning on the device");
    bmcDevice->begin(PIN_BM1000_CONTROL_POWER, PIN_BM1000_CONTROL_ON);
}

void loop() {

    // Do all stuffs necessary
    bmcDevice->run();

    // Consuming one measurement from queue
    auto *_aPayload = bmcDevice->getPayloadCollection().dequeOne();
    if (_aPayload != nullptr) {
        // Display the measurements to Serial, for now.
        printf("%s\r\n", _aPayload->toString().c_str());
        // Also the average, using moving average
        printf("Average SpO2: %d, BPM: %d\r\n", bmcDevice->getPayloadCollection().getAvgSpO2(), bmcDevice->getPayloadCollection().getAvgBPM());
    }

    delay(10);
}

#endif //PULSEOXIRELAY_EXP_BMC1000_HPP
