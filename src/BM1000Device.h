//
// Created by Andri Yadi on 26/07/21.
//

#ifndef PULSEOXIRELAY_BM1000DEVICE_H
#define PULSEOXIRELAY_BM1000DEVICE_H

#include "OximeterBLEClient.h"
#include "BM1000Payload.h"
#include <functional>

// Settings
#define DEVICE_AUTO_ON_OFF_ENABLED          1
#define DEVICE_AUTO_ON_AFTER_INTERVAL       (1000*0.5*60)   // 0.5 minutes
#define DEVICE_AUTO_OFF_AFTER_INTERVAL      (1000*1*60)     // 1 minutes
#define DEVICE_AUTO_OFF_AFTER_MIN_READINGS  10

enum BM1000DeviceEvent_e {
    BM1000DeviceEvent_Unknown = 0,
    BM1000DeviceEvent_TurnOn,
    BM1000DeviceEvent_TurnOff,
    BM1000DeviceEvent_FingerIn,
    BM1000DeviceEvent_FingerOut
};

struct BM1000DeviceStateTimeTracker_t {
    bool noFinger = true;
    bool deviceOn = false;
    bool busy = false;
    uint32_t lastOnOffChanged = 0;
    uint16_t readingCountWhenOn = 0;
};

class BM1000Device {
public:
    typedef std::function<void(BM1000DeviceEvent_e event)> BM1000DeviceEventCallback;

    explicit BM1000Device(OximeterIdentifiers &ids);
    ~BM1000Device();

    bool begin(int8_t powerControlPin, int8_t onControlPin=-1, bool autoOn = false);
    bool run();

    void turnOn();
    void turnOff();
    inline bool isOn() const {return stateTimeTracker_.deviceOn;}

    inline void onEvent(BM1000DeviceEventCallback cb) { eventCallback_ = std::move(cb); }

    inline BM1000PayloadCollection& getPayloadCollection() { return payloadCollection_; }

    inline void setPayloadEqualityType(OximeterPayloadEqualityType ty) { payloadCheckEqualityType_ = ty; }

private:
    OximeterIdentifiers &identifiers_;
    int8_t powerControlPin_ = -1;
    int8_t onControlPin_ = -1;

    OximeterBLEClient *oxiClient_ = nullptr;
    BM1000PayloadCollection payloadCollection_ = {};
    OximeterPayloadEqualityType payloadCheckEqualityType_ = OximeterPayloadEqualityType_BarGraph;

    BM1000DeviceEventCallback eventCallback_ = nullptr;

    // For tracking event
    BM1000DeviceStateTimeTracker_t stateTimeTracker_ = {};

    void handlePayloadNotification(
            BLERemoteCharacteristic* pBLERemoteCharacteristic,
            uint8_t* pData,
            size_t length,
            bool isNotify);
};


#endif //PULSEOXIRELAY_BM1000DEVICE_H
