//
// Created by Andri Yadi on 26/07/21.
//

#include "BM1000Payload.h"

BM1000Payload::BM1000Payload() {
    spo2 = BM1000_INVALID_SPO2;
    pleth = BM1000_INVALID_PLETH; //PLETHYSMOGRAM
    pulseRate = BM1000_INVALID_PULSE_RATE;
};

BM1000Payload::~BM1000Payload() = default;

bool BM1000Payload::parse(const uint8_t *pData, size_t length) {
    if (length < 5) {
        return false;
    }

    spo2 = pData[4];
    pleth = pData[1];
    pulseRate = pData[3] | ((pData[2] & 0x40) << 1); //Bit 6 (0x40) of Byte 2 is bit 7 of the Pulse Rate

    BM1000PayloadSystemFlags1_t  sFlag1;
    sFlag1.SystemFlags1 = pData[0];

    BM1000PayloadSystemFlags2_t  sFlag2;
    sFlag2.SystemFlags2 = pData[2];

    //printf("No finger: %d || ", sFlag2.noFinger);

    noFinger = sFlag2.noFinger == 1;
    notCalibrating = sFlag2.notCalib == 1;
    barGraph = sFlag2.bargraph;

    sensorSignalStrength = sFlag1.signalStrength;
    hasSensorSignal = sFlag1.noSignal == 0;

    time_t _curTime;
    time(&_curTime);
    timestamp = (long)_curTime;

    return true;
}

std::string BM1000Payload::toString() const {

    char retc[96];
    auto retl = snprintf(retc, 96, "[%ld] SpO2: %d, BPM: %d, PPG: %d, Finger? %d, Calib? %d, Sig: %d, Graph: %d",
                         timestamp, spo2, pulseRate, pleth, !noFinger, !notCalibrating, sensorSignalStrength, barGraph);
    retc[retl] = 0;

    return std::string(retc);
}

bool operator==(const BM1000Payload &p1, const BM1000Payload &p2) {
    if (p1.equalityType == OximeterPayloadEqualityType_PPG) {
        return (p1.spo2 == p2.spo2
                && p1.pulseRate == p2.pulseRate
                && p1.pleth == p2.pleth);
    }
    else if (p1.equalityType == OximeterPayloadEqualityType_BarGraph) {
        return (p1.spo2 == p2.spo2
                && p1.pulseRate == p2.pulseRate
                && p1.barGraph == p2.barGraph);
    }
    else if (p1.equalityType == OximeterPayloadEqualityType_PulseRate) {
        return (p1.spo2 == p2.spo2
                && p1.pulseRate == p2.pulseRate);
    }
    else {
        return false;
    }
}

bool operator!=(const BM1000Payload &p1, const BM1000Payload &p2) {
    return !(p1 == p2);
}



BM1000PayloadCollection::BM1000PayloadCollection() {
}

BM1000PayloadCollection::~BM1000PayloadCollection() {
    payloads_.clear();
}

bool BM1000PayloadCollection::enqueuePayload(const BM1000Payload &payload, bool noDuplicate, bool onlyValidVal) {

    bool retVal = false;

    // Check validity
    if (onlyValidVal) {
        if (payload.pulseRate == BM1000_INVALID_PULSE_RATE || payload.spo2 >= BM1000_INVALID_SPO2) {
            return retVal;
        }
    }

    if (!payloadSemaphore_.take(2, "payload")) {
        //printf("In jail\r\n");
        return retVal;
    }

#if PAYLOAD_COL_USE_RINGBUF

    // Check equality with last added payload
    if (noDuplicate) {
        //printf("last pleth: %d, payload pleth: %d\r\n", lastPayload_.pleth, payload.pleth);
        if (payload == lastPayload_) {
            //printf("SAME\r\n");
            payloadSemaphore_.give();
            return retVal;
        }
    }

    // If above not return, payload is different from lastPayload
    lastPayload_ = payload; // Copy

    retVal = payloads_.push(payload);
    payloadSemaphore_.give();
#else

    if (payloads_.size() > BM1000_MAX_PAYLOAD_COLLECTION_SIZE - 1) {
        //printf("FULL\r\n");
        payloads_.pop_front();
    }

    // Check equality with last added payload
    if (!payloads_.empty() && noDuplicate) {
        auto &_lastPayload = payloads_.back();
        //printf("last: %d, now: %d\r\n", _lastPayload.pleth, payload.pleth);
        if (payload == _lastPayload) {
            //printf("SAME\r\n");
            payloadSemaphore_.give();
            return retVal;
        }
    }

    payloads_.push_back(payload);

    payloadSemaphore_.give();
    retVal = true;
#endif

    bpmAvg_ = (uint8_t) roundf((1.0f/100) * bpmFilter_(payload.pulseRate*100));
    spo2Avg_ = (uint8_t) roundf((1.0f/100) * spo2Filter_(payload.spo2*100));

    return retVal;
}

BM1000Payload *BM1000PayloadCollection::dequeOne() {

    if (!payloadSemaphore_.take(10, "payload")) {
        //printf("In jail\r\n");
        return nullptr;
    }

#if PAYLOAD_COL_USE_RINGBUF
    if (payloads_.isEmpty()) {
        payloadSemaphore_.give();
        return nullptr;
    }

    BM1000Payload *_retVal = nullptr;
    if (payloads_.pop(firstElement)) {
        _retVal = &firstElement;
    }

    payloadSemaphore_.give();
    return _retVal;
#else
    if (payloads_.empty()) {
        payloadSemaphore_.give(); // important
        return nullptr;
    }

//    auto *_ret = &payloads_.front();
//    payloadSemaphore_.give();
//    return _ret;

    firstElement = std::move(payloads_.front());
    payloads_.pop_front();

    payloadSemaphore_.give();

    return &firstElement;
#endif

}

