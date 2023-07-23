#include "LPS22HB.h"
#include "Wire.h"

LPS22HB::LPS22HB(TwoWire& wire) :
    _wire(&wire),
    _initialized(false) 
{
}

int LPS22HB::begin() {
    _wire->begin();

    if (i2cRead(LPS22HB_WHO_AM_I_REG) != 0xb1) {
        end();
        return 0;
    }

    i2cWrite(LPS22HB_CTRL_REG1, 0x0);

    _initialized = true;
    return 1;
}

void LPS22HB::end() {
    #if defined(WIRE_HAS_END) && WIRE_HAS_END
    _wire->end();
    #endif
    _initialized = false;
}

float LPS22HB::readPressure(int units) {
    if (_initialized == true) {
        float reading = readRawPressure() / 4096.0;

        if (units == MILLIBAR) { // 1 hPa = 1 millibar
            return reading;
        } else if (units == PSI) {  // 1 hPa = 0.145038 PSI
            return reading * 0.0145038;
        } else {
            return reading * 0.1;
        }
    }
    return 0;
}

float LPS22HB::readTemperature(void) {
    return readRawTemperature() / 100.0;
}

int32_t LPS22HB::readRawPressure(void) {
    // trigger one shot
    i2cWrite(LPS22HB_CTRL_REG2, 0x01);

    // wait for ONE_SHOT bit to be cleared by the hardware
    while ((i2cRead(LPS22HB_CTRL_REG2) & 0x01) != 0) {
        yield();
    }

    int32_t pressure =  (i2cRead(LPS22HB_PRESS_OUT_XL_REG) |
                        (i2cRead(LPS22HB_PRESS_OUT_L_REG) << 8) |
                        (i2cRead(LPS22HB_PRESS_OUT_H_REG) << 16));

    /* convert the 2's complement 24 bit to 2's complement 32 bit */
    if(pressure & 0x00800000) {
        pressure |= 0xFF000000;
    }
            
    return pressure;
}

int16_t LPS22HB::readRawTemperature(void) {
    return  (i2cRead(LPS22HB_TEMP_OUT_L_REG) << 0) | 
            (i2cRead(LPS22HB_TEMP_OUT_H_REG) << 8);
}

int LPS22HB::i2cRead(uint8_t reg) {
    _wire->beginTransmission(LPS22HB_ADDRESS);
    _wire->write(reg);
    if (_wire->endTransmission(false) != 0) {
        return -1;
    }

    if (_wire->requestFrom(LPS22HB_ADDRESS, 1) != 1) {
        return -1;
    }
    
    return _wire->read();
}

int LPS22HB::i2cWrite(uint8_t reg, uint8_t val)
{
    _wire->beginTransmission(LPS22HB_ADDRESS);
    _wire->write(reg);
    _wire->write(val);
    if (_wire->endTransmission() != 0) {
        return 0;
    }

    return 1;
}
