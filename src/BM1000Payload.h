//
// Created by Andri Yadi on 21/07/21.
//

#ifndef PULSEOXIRELAY_BM1000PAYLOAD_H
#define PULSEOXIRELAY_BM1000PAYLOAD_H

/**
 * BCI protocol ref: https://raw.githubusercontent.com/zh2x/BCI_Protocol/master/BCI%20Protocol%20V1.2.pdf
 */

#define PAYLOAD_COL_USE_RINGBUF         1

#include "Arduino.h"
#include "OximeterPayload.h"
#include "utils/SMA.h"
#include "FreeRTOS.h"

#if PAYLOAD_COL_USE_RINGBUF
#include "PayloadRingBuffer.h"
#endif

#undef MIN
#undef MAX
#include <deque>

#define BM1000_PACKET_LENGTH                    5
#define BM1000_INVALID_PLETH                    0
#define BM1000_INVALID_BARGRAPH                 0
#define BM1000_INVALID_PULSE_RATE               0xFF
#define BM1000_INVALID_SPO2                     0x7F

#define BM1000_MAX_PAYLOAD_COLLECTION_SIZE      30

typedef union
{
    // Bitfield for System Flags 1
    struct
    {
        uint8_t signalStrength      :4;     // 15 is no finger, other is calibration in progress
        uint8_t noSignal	        :1;     // Always zero, means has signal
        uint8_t probeUnplugged	    :1;     // Set to 1 when a new peak pulsation is counted
        uint8_t pulseBeep     	    :1;
        uint8_t syncBit             :1;     // Package Head. Must be 1
    };
    uint8_t SystemFlags1;
} BM1000PayloadSystemFlags1_t;

typedef union
{
    // Bitfield for System Flags 2
    struct
    {
        uint8_t bargraph            :4;     // Pulse strenght value used in device display for bargraph height on right of screen
        uint8_t noFinger	        :1;     // 0 means sensor is trying measurement, 1 no finger
        uint8_t notCalib	        :1;     // 0 means calibration still running, or 1 nothing inserted under the sensor
        uint8_t probeUnplugged     	:1;     // Bit 6 of Byte 3 is bit 7 of the Pulse Rate
        uint8_t syncBit             :1;     // Must be 0
    };
    uint8_t SystemFlags2;
} BM1000PayloadSystemFlags2_t;

class BM1000Payload: public OximeterPayload {
public:


    BM1000Payload();
    ~BM1000Payload();

    long timestamp = -1;

//    bool noFinger = true;
//    uint8_t spo2 = 0;
//    uint8_t pleth = 0; //PLETHYSMOGRAM
//    uint8_t pulseRate = 0;

    uint8_t sensorSignalStrength = 15;
    bool hasSensorSignal = true;
    uint8_t barGraph = BM1000_INVALID_BARGRAPH;
    bool notCalibrating  = true;

    // How to check a payload is the same as previous payload, to avoid too many duplicate data
    OximeterPayloadEqualityType equalityType = OximeterPayloadEqualityType_BarGraph;

    // Important method to parse from raw data
    bool parse(const uint8_t *pData, size_t length);

    std::string toString() const;

    friend bool operator== (const BM1000Payload &p1, const BM1000Payload &p2);
    friend bool operator!= (const BM1000Payload &p1, const BM1000Payload &p2);


private:
};

class BM1000PayloadCollection {
public:
    BM1000PayloadCollection();
    ~BM1000PayloadCollection();

    bool enqueuePayload(const BM1000Payload& payload, bool noDuplicate=true, bool onlyValidValue=true);
    BM1000Payload *dequeOne();

    inline uint8_t getAvgSpO2() const { return spo2Avg_; }
    inline uint8_t getAvgBPM() const { return bpmAvg_; }

#if PAYLOAD_COL_USE_RINGBUF
    inline bool empty() { return payloads_.isEmpty(); }
    inline size_t size() { return payloads_.elements(); }
#else
    inline bool empty() { return payloads_.empty(); }
    inline size_t size() { return payloads_.size(); }
#endif

private:

    FreeRTOS::Semaphore payloadSemaphore_ = FreeRTOS::Semaphore("payload");

#if PAYLOAD_COL_USE_RINGBUF
    PayloadRingBuffer<BM1000Payload, BM1000_MAX_PAYLOAD_COLLECTION_SIZE> payloads_;
    BM1000Payload lastPayload_ = {}, firstElement = {}; //To store last payload for comparison
#else
    std::deque<BM1000Payload> payloads_; // Use deque as ring buffer
    BM1000Payload firstElement = {};
#endif

    SMA<10, uint16_t, uint32_t> spo2Filter_;
    uint8_t spo2Avg_ = BM1000_INVALID_SPO2;

    SMA<10, uint16_t, uint32_t> bpmFilter_;
    uint8_t bpmAvg_ = BM1000_INVALID_PULSE_RATE;
};

#endif //PULSEOXIRELAY_BM1000PAYLOAD_H
