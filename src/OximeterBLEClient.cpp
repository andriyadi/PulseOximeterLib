//
// Created by Andri Yadi on 26/07/21.
//

#include "OximeterBLEClient.h"
//#include "esp_log.h"
#include "esp32-hal-log.h"

//static const char *TAG = "OXIC";

OximeterIdentifiers::OximeterIdentifiers(const std::string &servUUID, const std::string &charUUID,
                                         const std::string &fallbackAddr) {
    serviceUUID_ = new BLEUUID(servUUID);
    characteristicsUUID_ = new BLEUUID(charUUID);
    if (!fallbackAddr.empty()) {
        fallbackDeviceAddress_ = new BLEAddress(fallbackAddr);
    }
}

OximeterIdentifiers::~OximeterIdentifiers() {
    delete serviceUUID_;
    delete characteristicsUUID_;
    delete fallbackDeviceAddress_;
}



OximeterBLEClient::OximeterBLEClient(OximeterIdentifiers &ids):identifiers_(ids) {
}

OximeterBLEClient::~OximeterBLEClient() {

}

bool OximeterBLEClient::begin() {

    log_i("Starting Oximeter BLE Client...");
    BLEDevice::init("");
    log_d(" - Device initialized");

    underlyingClient_ = BLEDevice::createClient();
    log_d(" - Created client");

    underlyingClient_->setClientCallbacks(new OximeterClientCallbacks(this));
    log_d(" - Client callbacks set");

    setCurrentState(OximeterBLEClientState_Began);

    return true;
}

void OximeterBLEClient::createDeviceFrom(BLEAdvertisedDevice &advertisedDevice) {
    if (device_ != nullptr) {
        delete device_;
        device_ = nullptr;
    }

    device_ = new BLEAdvertisedDevice(advertisedDevice);
    setCurrentState(OximeterBLEClientState_ReadyToConnect);
}

void OximeterBLEClient::setCurrentState(OximeterBLEClientState_e state) {
    currentState_ = state;
    if (currentState_ == OximeterBLEClientState_Began) {
        log_i("State: BEGAN");
    }
    else if (currentState_ == OximeterBLEClientState_NotYetFound) {
        log_i("State: NOT FOUND YET");
    }
    else if (currentState_ == OximeterBLEClientState_ReadyToConnect) {
        //log_i("State: READY");
    }
    else if (currentState_ == OximeterBLEClientState_Connecting) {
        //log_i("State: CONNECTING..");
    }
    else if (currentState_ == OximeterBLEClientState_Connected) {
        timeTracker_.lastConnected = millis();
        log_i("State: CONNECTED");
    }
    else if (currentState_ == OximeterBLEClientState_Disconnected) {
        timeTracker_.lastDisconnected = millis();
        log_i("State: DISCONNECTED");
    }
    else {
        log_i("Current state: %d", currentState_);
    }
}

bool OximeterBLEClient::connect() {
    if (device_ == nullptr) {
        return false;
    }
    if (underlyingClient_ == nullptr) {
        return false;
    }

    log_i("Forming a connection to %s", device_->getAddress().toString().c_str());
    setCurrentState(OximeterBLEClientState_Connecting);

    bool _connected = underlyingClient_->connect(device_);
    if (!_connected) {
        setCurrentState(OximeterBLEClientState_Disconnected);
        log_e(" - Disconnected before completing the connection or connection was failed");
        return false;
    }
    log_i(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = underlyingClient_->getService(*identifiers_.getServiceUUID());
    if (pRemoteService == nullptr) {
        log_e("Failed to find our service UUID: %s", identifiers_.getServiceUUID()->toString().c_str());
        underlyingClient_->disconnect();
        setCurrentState(OximeterBLEClientState_Disconnected);
        return false;
    }
    log_i(" - Found requested service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    oxiRemoteCharacteristic_ = pRemoteService->getCharacteristic(*identifiers_.getCharacteristicsUUID_());
    if (oxiRemoteCharacteristic_ == nullptr) {
        log_e("Failed to find our characteristic UUID: %s", identifiers_.getCharacteristicsUUID_()->toString().c_str());
        underlyingClient_->disconnect();
        return false;
    }
    log_i(" - Found requested characteristic %s", identifiers_.getCharacteristicsUUID_()->toString().c_str());

    // Read the value of the characteristic.
    if (oxiRemoteCharacteristic_->canRead()) {
        log_d(" - Requested characteristic can be read.");
        std::string value = oxiRemoteCharacteristic_->readValue();
#if OXI_BLE_LIB_DEBUG
        byte buf[64]= {0};
        memcpy(buf,value.c_str(),value.length());
        printf("The characteristic value was: ");
        for (int i = 0; i < value.length(); i++) {
          printf("0x%X ", buf[i]);
        }
        printf("\r\n");
#endif
    }
    else {
        log_e(" - Requested characteristic cannot be read.");
    }

    if (oxiRemoteCharacteristic_->canNotify()) {
        log_i(" - Requested characteristic can notify, registering callback.");

        using namespace std::placeholders;
        oxiRemoteCharacteristic_->registerForNotify(std::bind(&OximeterBLEClient::handleCharacteristicsNotification, this, _1, _2, _3,_4), true);

        // needed to actually start the notifications for the BerryMed oximeter:
        const uint8_t notificationOn[] = {0x1, 0x0};
        oxiRemoteCharacteristic_->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
    }
    else {
        log_e(" - Requested characteristic cannot notify us.");
    }

    if (oxiRemoteCharacteristic_->canIndicate()) {
        log_d(" - Requested characteristic can indicate.");
    } else {
        log_d(" - Requested characteristic cannot indicate.");
    }

    setCurrentState(OximeterBLEClientState_Connected);

    return true;
}

bool OximeterBLEClient::run() {
    if (currentState_ == OximeterBLEClientState_ReadyToConnect) {
        connect();
    }
    else if (currentState_ == OximeterBLEClientState_Disconnected) {
        if (millis() - timeTracker_.lastDisconnected > 1000) {
            log_i("Retry connecting...");
            connect();
        }
    }
    else if (currentState_ == OximeterBLEClientState_NotYetFound) {
        if (millis() - timeTracker_.lastScan > 10000) {
            scanForTargetDevice();
        }
    }

    return true;
}


void
OximeterBLEClient::handleCharacteristicsNotification(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                                     size_t length, bool isNotify) {

    if (!pBLERemoteCharacteristic->getUUID().equals(oxiRemoteCharacteristic_->getUUID())) {
        log_e("Got different char: %s", pBLERemoteCharacteristic->getUUID().toString().c_str());
        return;
    }

#if OXI_BLE_LIB_DEBUG
    //printf("Notify callback for char: %s (%d length)\r\n", pBLERemoteCharacteristic->getUUID().toString().c_str(), length);
    printf("Got raw data (HEX): ");
    for (int i = 0; i < length; i++) {
        printf("0x%X ", pData[i]);
        if (i != 0 && ((i+1) % 5 == 0)) {
            printf(" | ");
        }
    }
    printf("\r\n");
#endif

    if (payloadNotifyCallback_) {
        payloadNotifyCallback_(pBLERemoteCharacteristic, pData, length, isNotify);
    }
}

BLEScan *OximeterBLEClient::getScanner() {
    if (bleScanner_ == nullptr) {
        bleScanner_ = BLEDevice::getScan();
        bleScanner_->setAdvertisedDeviceCallbacks(new OximeterAdvertisedDeviceCallbacks(this));
        bleScanner_->setInterval(1349);
        bleScanner_->setWindow(449);
        bleScanner_->setActiveScan(true);

        setCurrentState(OximeterBLEClientState_NotYetFound);
    }

    return bleScanner_;
}

BLEScanResults OximeterBLEClient::scanForTargetDevice() {
    log_i("Scanning requested device...");

    auto _res = getScanner()->start(5, false);
    timeTracker_.lastScan = millis();

    return _res;
}


void OximeterAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
    log_d("Found BLE device found: %s", advertisedDevice.toString().c_str());
    //log_i("Found BLE device address: %s", advertisedDevice.getAddress().toString().c_str());
    log_d("Found BLE device address: %s", advertisedDevice.getAddress().toString().c_str());

    if (client_ == nullptr) {
        return;
    }

    if (advertisedDevice.haveServiceUUID()) {
        log_d("Device has Service UUID");
        if (advertisedDevice.isAdvertisingService(*client_->getIdentifiers().getServiceUUID())) {
            log_d("Found requested Service UUID");
        }
        else {
            log_d("NOT found target our Service UUID");
        }
    }
    else {
        log_d("Device does not have Service UUID");
    }

    // We have found a device, let us now see if it contains the service we are looking for.
    if ((advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(*client_->getIdentifiers().getServiceUUID()))
        || (advertisedDevice.getAddress().equals(*client_->getIdentifiers().getFallbackDeviceAddress()))) {

        log_i("Found device with requested service. Scan will stop!");
        BLEDevice::getScan()->stop();

        client_->createDeviceFrom(advertisedDevice);

    } // Found our server
}




void OximeterClientCallbacks::onConnect(BLEClient *pclient) {
    log_i("BLE Client is connected");
}

void OximeterClientCallbacks::onDisconnect(BLEClient *pclient) {
    log_i("BLE Client is disconnected");
    client_->setCurrentState(OximeterBLEClientState_NotYetFound);
}
