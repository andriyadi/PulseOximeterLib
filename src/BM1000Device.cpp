//
// Created by Andri Yadi on 26/07/21.
//

#include "BM1000Device.h"

BM1000Device::BM1000Device(OximeterIdentifiers &ids):identifiers_(ids) {

}

BM1000Device::~BM1000Device() {
    delete oxiClient_;
    oxiClient_ = nullptr;
}

bool BM1000Device::begin(int8_t powerControlPin, int8_t onControlPin, bool autoOn) {
    if (powerControlPin >= 0) {
        powerControlPin_ = powerControlPin;
    }
    if (onControlPin >= 0) {
        onControlPin_ = onControlPin;
    }

    stateTimeTracker_.deviceOn = false;

    // Create Oxi BLE Client
    oxiClient_ = new OximeterBLEClient(identifiers_);

    using namespace std::placeholders;
    oxiClient_->onPayloadNotified(std::bind(&BM1000Device::handlePayloadNotification, this, _1, _2, _3,_4));
    auto _oxiBegan = oxiClient_->begin();

    if (powerControlPin_ >= 0) {
        pinMode(powerControlPin_, OUTPUT);
        digitalWrite(powerControlPin_, LOW);
    }
    if (onControlPin_ >= 0) {
        pinMode(onControlPin_, OUTPUT);
        digitalWrite(powerControlPin_, HIGH);

        if (autoOn) {
            turnOn();
        }
    }

    // Start scan
    if (oxiClient_ != nullptr) {
        delay(100);
        oxiClient_->scanForTargetDevice();
    }

    return _oxiBegan;
}

void BM1000Device::turnOn() {

    if (powerControlPin_ >= 0) {
        digitalWrite(powerControlPin_, HIGH);
    }

    if (onControlPin_ >= 0) {
        digitalWrite(onControlPin_, LOW);
        delay(100);
        digitalWrite(onControlPin_, HIGH);
        delay(500);
    }

    stateTimeTracker_.deviceOn = true;
    stateTimeTracker_.lastOnOffChanged = millis();

    if (eventCallback_) {
        eventCallback_(BM1000DeviceEvent_TurnOn);
    }
}

void BM1000Device::turnOff() {
    if (powerControlPin_ >= 0) {
        digitalWrite(powerControlPin_, LOW);
    }

    stateTimeTracker_.deviceOn = false;
    stateTimeTracker_.lastOnOffChanged = millis();

    if (eventCallback_) {
        if (stateTimeTracker_.noFinger != true) {
            stateTimeTracker_.noFinger = true;
            eventCallback_(BM1000DeviceEvent_FingerOut); //Must be no finger
        }

        stateTimeTracker_.readingCountWhenOn = 0;
        eventCallback_(BM1000DeviceEvent_TurnOff);
    }
}

void BM1000Device::handlePayloadNotification(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                              size_t length, bool isNotify) {


    stateTimeTracker_.busy = true;
    size_t _packetCount = length / BM1000_PACKET_LENGTH;

    if (_packetCount != 4) {
        log_e("Incorrect packet length!!!");
        stateTimeTracker_.busy = false;
        return;
    }

    uint8_t _aPayloadBytes[BM1000_PACKET_LENGTH];
    BM1000Payload _bmc1000Payload = {};
    _bmc1000Payload.equalityType = payloadCheckEqualityType_;

    //for (int i = _packetCount-1; i < _packetCount; i++) {
    for (int i = 0; i < _packetCount; i++) {
        memcpy(_aPayloadBytes, pData + i*BM1000_PACKET_LENGTH, BM1000_PACKET_LENGTH);
        auto _res = _bmc1000Payload.parse(_aPayloadBytes, BM1000_PACKET_LENGTH);
        if (_res) {

//            if (i == _packetCount-1) {
//                printf("%s\r\n", _bmc1000Payload.toString().c_str());
//            }

            auto _added = payloadCollection_.enqueuePayload(_bmc1000Payload);
            if (_added) {
                //printf("%s\r\n", _bmc1000Payload.toString().c_str());
#if DEVICE_AUTO_ON_OFF_ENABLED
                stateTimeTracker_.readingCountWhenOn++;
#endif
            }
            //printf("\r\nAvg SpO2 = %.2f, BPM = %.2f\r\n", payloadCollection_.getAvgSpO2(), payloadCollection_.getAvgBPM());
        }
        else {
            log_e("Failed to parse!");
        }
    }

    // Get last payload
    if (stateTimeTracker_.noFinger != _bmc1000Payload.noFinger) {
        stateTimeTracker_.noFinger = _bmc1000Payload.noFinger;

        if (eventCallback_) {
            eventCallback_(stateTimeTracker_.noFinger? BM1000DeviceEvent_FingerOut: BM1000DeviceEvent_FingerIn);
        }
    }

    stateTimeTracker_.busy = false;
}

bool BM1000Device::run() {

    if (DEVICE_AUTO_ON_OFF_ENABLED &&
        stateTimeTracker_.lastOnOffChanged > 0 // Udah pernah ON sekali
        && !stateTimeTracker_.busy) {
        if (!isOn()) {
            if (millis() - stateTimeTracker_.lastOnOffChanged >= DEVICE_AUTO_ON_AFTER_INTERVAL) {
                turnOn();
            }
        }
        else {
            if (millis() - stateTimeTracker_.lastOnOffChanged >= DEVICE_AUTO_OFF_AFTER_INTERVAL
                && (stateTimeTracker_.noFinger || stateTimeTracker_.readingCountWhenOn >= DEVICE_AUTO_OFF_AFTER_MIN_READINGS)
                ) {
                log_i("Turning off with readings: %d", stateTimeTracker_.readingCountWhenOn);
                turnOff();
            }
        }
    }

    if (oxiClient_ != nullptr && isOn()) {
        return oxiClient_->run();
    }

    return true;
}

