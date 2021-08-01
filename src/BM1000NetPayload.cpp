//
// Created by Andri Yadi on 25/07/21.
//

#include "BM1000NetPayload.h"

// setup the field sizes
static const uint8_t BM1000NetPayloadDataFieldSizes[] = { BM1000_NET_PAYLOAD_DATA_FIELD_SIZES };

BM1000NetPayload::BM1000NetPayload() {

}

BM1000NetPayload::~BM1000NetPayload() {

}

void BM1000NetPayload::init() {
    printf("Buf size: %lu, field count: %d\r\n", BM1000_NET_PAYLOAD_DATA_BUFFER_SIZE, BM1000_NET_PAYLOAD_DATA_FIELD_COUNT);
    memset(buffer_, 0, BM1000_NET_PAYLOAD_DATA_BUFFER_SIZE);
}

bool BM1000NetPayload::isValid() const {
    return true;
}

void BM1000NetPayload::printRecordLn(const char *separator) const {
    if (sensorData_ == nullptr) {
        return;
    }
    PRINT_FIELD_NUMBER_INT_WITH_SEPARATOR(separator, (int)sensorData_->timestamp);
    PRINT_FIELD_NUMBER_INT_WITH_SEPARATOR(separator, sensorData_->noFinger);
    PRINT_FIELD_NUMBER_INT_WITH_SEPARATOR(separator, sensorData_->spo2);
    PRINT_FIELD_NUMBER_INT_WITH_SEPARATOR(separator, sensorData_->pleth);
    PRINT_FIELD_NUMBER_INT_WITH_SEPARATOR(separator, sensorData_->pulseRate);
    PRINT_FIELD_NUMBER_INT_WITH_SEPARATOR(separator, sensorData_->barGraph);

    printf("\r\n");
}

void BM1000NetPayload::serialize() {
    if (sensorData_ == nullptr) {
        return;
    }
    setFieldValue<uint32_t>(Timestamp, (uint32_t )(sensorData_->timestamp));
    setFieldValue<uint8_t>(NoFinger, (uint8_t )(sensorData_->noFinger));
    setFieldValue<uint8_t>(SpO2, (uint8_t )(sensorData_->spo2));
    setFieldValue<uint8_t>(Pleth, (uint8_t )(sensorData_->pleth));
    setFieldValue<uint8_t>(BPM, (uint8_t )(sensorData_->pulseRate));
    setFieldValue<uint8_t>(Graph, (uint8_t )(sensorData_->barGraph));
}

void BM1000NetPayload::deserialize() {
    sensorData_->timestamp = getFieldValue<uint32_t>(Timestamp);
    sensorData_->noFinger = getFieldValue<uint8_t>(NoFinger);
    sensorData_->spo2 = getFieldValue<uint8_t>(SpO2);
    sensorData_->pleth = getFieldValue<uint8_t>(Pleth);
    sensorData_->pulseRate = getFieldValue<uint8_t>(BPM);
    sensorData_->barGraph = getFieldValue<uint8_t>(Graph);
}

uint8_t BM1000NetPayload::getFieldSize(uint8_t fieldIndex) const {
    if (fieldIndex > getFieldCount()) {
        // TODO sanity check fail
        return 0;
    }

    return BM1000NetPayloadDataFieldSizes[fieldIndex];
}

void BM1000NetPayload::printBuffer() {
    for (int i = 0; i < getSize(); i++) {
        printf("0x%X ", *(getBuffer() + i));
    }
    printf("\r\n");
}
