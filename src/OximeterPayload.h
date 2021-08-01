//
// Created by Andri Yadi on 25/07/21.
//

#ifndef PULSEOXIRELAY_OXIMETERPAYLOAD_H
#define PULSEOXIRELAY_OXIMETERPAYLOAD_H

enum OximeterPayloadEqualityType { OximeterPayloadEqualityType_PPG, OximeterPayloadEqualityType_BarGraph, OximeterPayloadEqualityType_PulseRate };

class OximeterPayload {
public:
    OximeterPayload() = default;
    virtual ~OximeterPayload() = default;

    long timestamp = -1;

    bool noFinger = true;
    uint8_t spo2 = 0;
    uint8_t pleth = 0; //PLETHYSMOGRAM
    uint8_t pulseRate = 0;

protected:
};
#endif //PULSEOXIRELAY_OXIMETERPAYLOAD_H
