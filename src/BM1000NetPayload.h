//
// Created by Andri Yadi on 25/07/21.
// Just an example how to serialize and deserialize payload using binary format for transmitting over the network.
//

#ifndef PULSEOXIRELAY_BM1000NETPAYLOAD_H
#define PULSEOXIRELAY_BM1000NETPAYLOAD_H

#include "netpayload/BinaryNetPayloadBase.h"
#include "OximeterPayload.h"
#include "BM1000Payload.h"

#define BM1000_NET_PAYLOAD_DATA_HEADER "Timestamp, NoFinger, SpO2, Pleth, BPM, Graph\r\n====================================================\r\n"

#define BM1000_NET_PAYLOAD_DATA_FIELD_SIZES     sizeof(uint32_t),   \
                                                sizeof(uint8_t),    \
                                                sizeof(uint8_t),    \
                                                sizeof(uint8_t),    \
                                                sizeof(uint8_t),    \
                                                sizeof(uint8_t)

#define BM1000_NET_PAYLOAD_DATA_BUFFER_SIZE (SUM(BM1000_NET_PAYLOAD_DATA_FIELD_SIZES))
#define BM1000_NET_PAYLOAD_DATA_FIELD_COUNT (COUNT(BM1000_NET_PAYLOAD_DATA_FIELD_SIZES))

class BM1000NetPayload: public BinaryNetPayloadBase {
public:
    BM1000NetPayload();
    ~BM1000NetPayload() override;

    void init() override;
    bool isValid() const override;
    uint16_t getSize() const override { return BM1000_NET_PAYLOAD_DATA_BUFFER_SIZE; }
    uint8_t getFieldCount() const override{ return BM1000_NET_PAYLOAD_DATA_FIELD_COUNT; }
    uint8_t* getBuffer() const override { return (uint8_t*)buffer_; }

    void printHeaderLn() const override { printf(BM1000_NET_PAYLOAD_DATA_HEADER); }
    void printRecordLn(const char* separator = DATA_RECORD_SEPARATOR) const override;

    void printBuffer();

    void serialize();
    void deserialize();

    inline void setDataSource(OximeterPayload *payload) { sensorData_ = (BM1000Payload *)payload; }
    //inline void setDataSource(BM1000Payload *payload) { sensorData_ = payload; }

protected:
    uint8_t getFieldSize(uint8_t fieldIndex) const;

private:
    uint8_t buffer_[BM1000_NET_PAYLOAD_DATA_BUFFER_SIZE];

    BM1000Payload *sensorData_ = nullptr;

enum Fields { Timestamp, NoFinger, SpO2, Pleth, BPM, Graph };
};


#endif //PULSEOXIRELAY_BM1000NETPAYLOAD_H
