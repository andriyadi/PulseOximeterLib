//
// Created by Andri Yadi on 1/25/17.
//

#include "BinaryNetPayloadBase.h"

BinaryNetPayloadBase::~BinaryNetPayloadBase() {

}

void BinaryNetPayloadBase::copyTo(uint8_t* buffer, size_t size) const
{
    if (size < getSize()) {
        // TODO failed sanity check!
        return;
    }

    memcpy(buffer, getBuffer(), size);
}

void BinaryNetPayloadBase::copyFrom(const uint8_t* buffer, size_t size)
{
    //if (size != getSize()) {
	if (size > getSize()) {
        // TODO failed sanity check!
        return;
    }

    memcpy(getBuffer(), buffer, size);
}

uint16_t BinaryNetPayloadBase::getFieldStart(uint8_t fieldIndex) const
{
    uint16_t result = 0;
    for (uint8_t i = 0; i < fieldIndex; i++) {
        result += getFieldSize(i);
    }

    return result;
}

uint8_t BinaryNetPayloadBase::getFieldValue(uint8_t fieldIndex, uint8_t* buffer, size_t size) const
{
    uint8_t fieldSize = getFieldSize(fieldIndex);

    if (fieldIndex > getFieldCount() || fieldSize > size) {
        printf("getFieldValue(): sanity check failed!\r\n");
        return 0;
    }

    uint16_t fieldStart = getFieldStart(fieldIndex);
    memcpy(buffer, getBuffer() + fieldStart, fieldSize); // already checked buffer size >= getFieldSize(fieldIndex)

    return fieldSize;
}

void BinaryNetPayloadBase::setFieldValue(uint8_t fieldIndex, const uint8_t* buffer, uint8_t size) const
{
    if (fieldIndex > getFieldCount() || size > getFieldSize(fieldIndex)) {
        printf("setFieldValue(): sanity check failed!\r\n");
        return;
    }

    uint16_t fieldStart = getFieldStart(fieldIndex);
    memcpy(getBuffer() + fieldStart, buffer, getFieldSize(fieldIndex));
}
