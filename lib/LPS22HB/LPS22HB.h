#ifndef _LPS22HB_H_
#define _LPS22HB_H_

#include <Arduino.h>
#include <Wire.h>

#define LPS22HB_ADDRESS  0x5C

#define LPS22HB_WHO_AM_I_REG        0x0F
#define LPS22HB_CTRL_REG1           0x10
#define LPS22HB_CTRL_REG2           0x11
#define LPS22HB_CTRL_REG3           0x12
#define LPS22HB_FIFO_CTRL           0x14
#define LPS22HB_STATUS_REG          0x27
#define LPS22HB_PRESS_OUT_XL_REG    0x28
#define LPS22HB_PRESS_OUT_L_REG     0x29
#define LPS22HB_PRESS_OUT_H_REG     0x2A
#define LPS22HB_TEMP_OUT_L_REG      0x2B
#define LPS22HB_TEMP_OUT_H_REG      0x2C

enum {
    PSI,
    MILLIBAR,
    KILOPASCAL
};

class LPS22HB {
    public:
        LPS22HB(TwoWire& wire);

        int begin();
        void end();

        float readPressure(int units = MILLIBAR);
        float readTemperature(void);
        int32_t readRawPressure(void);
        int16_t readRawTemperature(void);

    private:
        int i2cRead(uint8_t reg);
        int i2cWrite(uint8_t reg, uint8_t val);

    private:
        TwoWire* _wire;
        bool _initialized;
};

#endif
