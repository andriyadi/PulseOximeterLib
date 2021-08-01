//
// Created by Andri Yadi on 1/25/17.
//

//Adapted from here: https://github.com/SodaqMoja/SodaqOne-UniversalTracker/blob/master/SodaqOneTracker/DataRecord.h

#ifndef DYCODEX_DATAMODELBASE_H
#define DYCODEX_DATAMODELBASE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "NetPayloadMacros.h"

/**
 * Base class for standardizing data record definitions with fixed-size fields.
 * This makes an absolutely safe way of dealing with the records vs the unaligned structs.
 * To allow for portability it uses memcpy instead of casts for converting fields to and from the buffer.
 */

#define DATA_RECORD_SEPARATOR ", "

#define PRINT_FIELD_NUMBER_INT_WITH_SEPARATOR(separatorVar, val) { printf("%d%s", val, separatorVar); }
#define PRINT_FIELD_NUMBER_FLOAT_WITH_SEPARATOR(separatorVar, val) { printf("%.4f%s", val, separatorVar); }

//
// Derived classes should:
// * implement all virtual methods (obv.)
// * (optional) create enum Fields { FieldName1 = 0, FieldName2, FieldName3, ... } for ease of use
// * (optional but strongly suggested) create getters/setters for all fields
//

// TODO: make fieldsizes needed only in one place for each derived class

class BinaryNetPayloadBase {
public:
	virtual ~BinaryNetPayloadBase();
    virtual void init() {}

    virtual bool isValid() const = 0;
    virtual uint16_t getSize() const = 0;
    virtual uint8_t getFieldCount() const = 0;

    virtual uint8_t* getBuffer() const = 0;

    virtual void printHeaderLn() const = 0;
    virtual void printRecordLn(const char* separator = DATA_RECORD_SEPARATOR) const = 0;

    void copyTo(uint8_t* buffer, size_t size) const;
    void copyFrom(const uint8_t* buffer, size_t size);

    uint8_t getFieldValue(uint8_t fieldIndex, uint8_t* buffer, size_t size) const;

    template <typename T>
    T getFieldValue(uint8_t fieldIndex) const
    {
        // implementation has to be here
        if (fieldIndex > getFieldCount() || getFieldSize(fieldIndex) > sizeof(T)) {
            // TODO debugPrintLn("getFieldValue(): sanity check failed!");
            printf("getFieldValue(): sanity check failed! Field index: %d\r\n", fieldIndex);
            return 0;
        }

        T result;
        uint16_t fieldStart = getFieldStart(fieldIndex);
        memcpy(&result, getBuffer() + fieldStart, sizeof(T));

        return result;
    }

    void setFieldValue(uint8_t fieldIndex, const uint8_t* buffer, uint8_t size) const;

    template <typename T>
    void setFieldValue(uint8_t fieldIndex, T value) const
    {
        // implementation has to be here
        if (fieldIndex > getFieldCount() || sizeof(T) > getFieldSize(fieldIndex)) {
            printf("setFieldValue(): sanity check failed! Field index: %d\r\n", fieldIndex);
            return;
        }

        uint16_t fieldStart = getFieldStart(fieldIndex);
        memcpy(getBuffer() + fieldStart, &value, getFieldSize(fieldIndex));
    }

protected:
    virtual uint8_t getFieldSize(uint8_t fieldIndex) const = 0;
    uint16_t getFieldStart(uint8_t fieldIndex) const;
private:

};


#endif //DYCODEX_DATAMODELBASE_H
