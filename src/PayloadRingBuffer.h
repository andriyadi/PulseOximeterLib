//
// Created by Andri Yadi on 26/07/21.
// Adapter from here: https://github.com/Romkabouter/ESP32-Rhasspy-Satellite/blob/master/PlatformIO/src/Esp32RingBuffer.h
//

#ifndef PULSEOXIRELAY_PAYLOADRINGBUFFER_H
#define PULSEOXIRELAY_PAYLOADRINGBUFFER_H

#include <freertos/ringbuf.h>

template <typename IT, size_t S>
class PayloadRingBuffer {
public:
    PayloadRingBuffer() {
        rbh = xRingbufferCreate(S * sizeof(IT), RINGBUF_TYPE_BYTEBUF);
    }

    /* Push an input item to the end of the buffer */
    //bool push(const IT inElement)
    bool push(const IT &inElement)
    {
        return pdTRUE == xRingbufferSend(rbh, &inElement, sizeof(IT), pdMS_TO_TICKS(10));
    }

    /* Push an item array to the end of the buffer */
    bool push(const IT *const inElement_p, size_t len = 1)
    {
        return pdTRUE == xRingbufferSend(rbh, inElement_p, sizeof(IT)*len, pdMS_TO_TICKS(10));
    }

    /* Pop the data at the beginning of the buffer */
    bool pop(IT &outElement)
    {
        bool retval = false;
        size_t item_size;
        if (size() >= sizeof(IT))
        {
            IT *item_p = static_cast<IT*>(xRingbufferReceiveUpTo(rbh, &item_size, pdMS_TO_TICKS(10), sizeof(IT)));
            if (item_p != NULL)
            {
                if (item_size != sizeof(IT))
                {
                    log_e("Did not receive enough data, this should not happen");
                }
                else
                {
                    outElement = *item_p;
                    retval = true;
                }
                vRingbufferReturnItem(rbh, item_p);
            }
        }
        return retval;
    }

    /* Push an input item to the end of the buffer from within an interrupt service routine */
    bool pushFromISR(const IT inElement)
    {
        return pdTRUE == xRingbufferSendFromISR(rbh, &inElement, sizeof(IT), pdMS_TO_TICKS(10));
    }

    /* Push an input item array to the end of the buffer from within an interrupt service routine */
    bool pushFromISR(const IT *const inElement_p, size_t len = 1)
    {
        return pdTRUE == xRingbufferSendFromISR(rbh, inElement_p, sizeof(IT)*len, pdMS_TO_TICKS(10));
    }

    /* Pop the data from the beginning of the buffer from within an interrupt service routine */
    bool popFromISR(IT &outElement)
    {
        bool retval = false;
        size_t item_size;
        if (size() >= sizeof(IT))
        {
            IT *item_p = static_cast<IT *>(xRingbufferReceiveUpToFromISR(rbh, &item_size, sizeof(IT)));
            if (item_p != NULL)
            {
                if (item_size != sizeof(IT))
                {
                    Serial.println("Did not receive enough data, this should not happen");
                }
                else
                {
                    outElement = *item_p;
                    retval = true;
                }
                vRingbufferReturnItemFromISR(rbh, item_p);
            }
        }
        return retval;
    }

    /* Return true if the buffer is full */
    bool isFull() { return xRingbufferGetCurFreeSize(rbh) == 0; }

    /* Return true if the buffer is empty */
    bool isEmpty() { return xRingbufferGetCurFreeSize(rbh) == xRingbufferGetMaxItemSize(rbh); }

    /* Reset the buffer  to an empty state */
    void clear()
    {
        // we try to get to a state where we are not getting any more memory back
        // this requires at least 2 calls to xRingbufferReceiveUpTo
        void* item_p;

        do
        {
            size_t freed_bytes;

            item_p = xRingbufferReceiveUpTo(rbh, &freed_bytes, 0, size());
            if (item_p != NULL)
            {
                vRingbufferReturnItem(rbh, item_p);
            }
        }
        while (item_p != NULL);
    }
    /* return the element numbers */
    size_t size() { return (xRingbufferGetMaxItemSize(rbh) - xRingbufferGetCurFreeSize(rbh)); }

    /* return the used size of the buffer in bytes */
    size_t elements() { return (xRingbufferGetMaxItemSize(rbh) - xRingbufferGetCurFreeSize(rbh))/sizeof(IT); }

    /* return the maximum size of the buffer in bytes*/
    size_t maxSize() { return xRingbufferGetMaxItemSize(rbh); }

    /* return the free size of the buffer in bytes*/
    size_t freeSize() { return xRingbufferGetMaxItemSize(rbh); }

private:
    RingbufHandle_t rbh = nullptr;
};
#endif //PULSEOXIRELAY_PAYLOADRINGBUFFER_H
