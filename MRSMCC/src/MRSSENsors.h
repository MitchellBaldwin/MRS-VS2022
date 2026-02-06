/*	MRSSENsors.h
*	MRSSENsorsClass - Base class for controlling and accessing MRS Sensors module sensors
*
*	Mitchell Baldwin copyright 2026
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _MRSSENSORS_h
#define _MRSSENSORS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

constexpr uint8_t defaultMRSSENAddress = 0x08;			// I2C address of MRS Sensors module on MCC I2C bus

constexpr uint8_t MRSSEN_ODOSPosXAddress = 0x01;		// SF ODOS X position address
constexpr uint8_t MRSSEN_ODOSPosYAddress = 0x05;		// SF ODOS Y position address
constexpr uint8_t MRSSEN_ODOSHdgAddress = 0x09;			// SF ODOS heading address
constexpr uint8_t MRSSEN_FWDLIDARRangeAddress = 0x0D;	// FWDLIDAR range address
constexpr uint8_t MRSSEN_TurretPositionAddress = 0x11;	// Sensor turret position address

#include <Wire.h>

class MRSSENsorsClass
{
protected:
	char buf[64];
	uint8_t _i2caddress = defaultMRSSENAddress;
	int STPosition = 0;									// Current sensor turret position in steps

    template <typename T>
    uint8_t& getData(const uint8_t addr, T& value) const {
        /*!
          @brief     Template for reading from I2C using any data type
          @details   As a template it can support compile-time data type definitions
          @param[in] addr Memory address
          @param[in] value Data Type "T" to read
          @return    Size of data read in bytes
        */
        uint8_t* bytePtr = (uint8_t*)&value;            // Pointer to structure beginning
        static uint8_t structSize = sizeof(T);          // Number of bytes in structure
        if (_i2caddress)                                // Using I2C if address is non-zero
        {                                               //
            Wire.beginTransmission(_i2caddress);        // Address the I2C device
            Wire.write(addr);                           // Send register address to read
            Wire.endTransmission();                     // Close transmission
            Wire.requestFrom(_i2caddress, sizeof(T));   // Request 1 byte of data
            structSize = Wire.available();              // Use the actual number of bytes
            for (uint8_t i = 0; i < structSize; i++)
                *bytePtr++ = Wire.read();               // loop for each byte to be read
        }
		return structSize;                              // Return number of bytes read
    }

public:
	MRSSENsorsClass();
	bool Init();
	bool Init(uint8_t i2cAddress);
    bool TestI2CConnection() const {
        /*!
          @brief     Test I2C connection to device
          @details   Attempts to begin and end transmission to I2C device
          @return    True if device acknowledges, false otherwise
        */
        Wire.beginTransmission(_i2caddress);
        if (Wire.endTransmission() == 0x00) {
            return true;
        } else {
            return false;
        }
	}

    uint8_t getByte(const uint8_t addr) const {
        /*!
          @brief     Read a single byte from I2C
          @details   Wrapper for getData to read a single byte
          @param[in] addr Memory address
          @return    Byte read from device
        */
        uint8_t value = 0;
        getData<uint8_t>(addr, value);
        return value;
	}

    uint16_t getWord(const uint8_t addr) const {
        /*!
          @brief     Read a single word (2 bytes) from I2C
          @details   Wrapper for getData to read a single word
          @param[in] addr Memory address
          @return    Word read from device
        */
        uint16_t value = 0;
        getData<uint16_t>(addr, value);
        return value;
    }

    int32_t getInt(const uint8_t addr) const {
        /*!
          @brief     Read a single long (4 bytes) from I2C
          @details   Wrapper for getData to read a single long
          @param[in] addr Memory address
          @return    Long read from device
        */
        int32_t value = 0;
        getData<int32_t>(addr, value);
        return value;
	}

    float getFloat(const uint8_t addr) const {
        /*!
          @brief     Read a single float (4 bytes) from I2C
          @details   Wrapper for getData to read a single float
          @param[in] addr Memory address
          @return    Float read from device
        */
        float value = 0.0f;
        getData<float>(addr, value);
        return value;
    }

	int GetFWDLIDARRangeMM();
};

//extern MRSSENsorsClass MRSSENsors;

#endif

