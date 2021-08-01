//
// Created by Andri Yadi on 26/07/21.
// Inspired from this project: https://github.com/tobiasisenberg/OxiVis
//

#ifndef PULSEOXIRELAY_OXIMETERBLECLIENT_H
#define PULSEOXIRELAY_OXIMETERBLECLIENT_H

#include <utility>

#include "Arduino.h"
#include "BLEDevice.h"

#define OXI_BLE_LIB_DEBUG       0

class OximeterIdentifiers {
public:
    OximeterIdentifiers(const std::string &servUUID, const std::string &charUUID, const std::string& fallbackAddr="");
    virtual ~OximeterIdentifiers();

    BLEUUID *getServiceUUID() { return serviceUUID_; }
    BLEUUID *getCharacteristicsUUID_() { return characteristicsUUID_; }
    BLEAddress *getFallbackDeviceAddress() { return fallbackDeviceAddress_; }
private:
    BLEUUID *serviceUUID_ = nullptr;
    BLEUUID *characteristicsUUID_ = nullptr;
    BLEAddress *fallbackDeviceAddress_ = nullptr;
};

enum OximeterBLEClientState_e {
    OximeterBLEClientState_Unknown = 0,
    OximeterBLEClientState_Began,
    OximeterBLEClientState_NotYetFound,
    OximeterBLEClientState_ReadyToConnect,
    OximeterBLEClientState_Disconnected,
    OximeterBLEClientState_Connecting,
    OximeterBLEClientState_Connected
};

struct OximeterBLEClientTimeTracker {
    uint32_t lastScan = 0;
    uint32_t lastConnected = 0;
    uint32_t lastDisconnected = 0;
};

class OximeterBLEClient {
public:
    explicit OximeterBLEClient(OximeterIdentifiers &ids);
    virtual ~OximeterBLEClient();

    bool begin();
    bool connect();
    bool run();

    void createDeviceFrom(BLEAdvertisedDevice &advertisedDevice);

    OximeterIdentifiers &getIdentifiers() {
        return identifiers_;
    }

    BLEScan* getScanner();
    BLEScanResults scanForTargetDevice();

    void setCurrentState(OximeterBLEClientState_e state);
    inline void onPayloadNotified(notify_callback cb) { payloadNotifyCallback_ = std::move(cb); }
    //inline const BMC1000PayloadCollection& getPayloadCollection() { return payloadCollection_; }

private:
    OximeterIdentifiers &identifiers_;

    BLEAdvertisedDevice *device_ = nullptr;
    BLEClient *underlyingClient_ = nullptr;
    BLERemoteCharacteristic* oxiRemoteCharacteristic_ = nullptr;
    BLEScan *bleScanner_ = nullptr;

    OximeterBLEClientState_e currentState_ = OximeterBLEClientState_Unknown;
    OximeterBLEClientTimeTracker timeTracker_ = {};

    //BMC1000PayloadCollection payloadCollection_ = {};
    notify_callback		 payloadNotifyCallback_;

    void handleCharacteristicsNotification(
            BLERemoteCharacteristic* pBLERemoteCharacteristic,
            uint8_t* pData,
            size_t length,
            bool isNotify);
};


class OximeterAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
public:
    explicit OximeterAdvertisedDeviceCallbacks(OximeterBLEClient *cl): client_(cl) {}

    void setClient(OximeterBLEClient *cl) {
        client_ = cl;
    }

    void onResult(BLEAdvertisedDevice advertisedDevice) override;

private:
    OximeterBLEClient *client_ = nullptr;
};


class OximeterClientCallbacks: public BLEClientCallbacks {
public:
    explicit OximeterClientCallbacks(OximeterBLEClient *cl): client_(cl) {}
    void onConnect(BLEClient* pclient) override;
    void onDisconnect(BLEClient* pclient) override;

private:
    OximeterBLEClient *client_ = nullptr;
};
#endif //PULSEOXIRELAY_OXIMETERBLECLIENT_H
