/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : File management header
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#ifndef FILE_HEADER_FILE
#define FILE_HEADER_FILE

#include <Arduino.h>
#include <pico/time.h>
#include "board.h"
#include "parameters.h"
#include <LittleFS.h>
// #include "hardware/sync.h"
#include "hardware/flash.h"

// #define FS_FLASH_START_OFF 1*1024*1024 // Sketch size: 1 MB
// #define FS_FLASH_SIZE 15*1024*1024 // FS size: 15 MB

typedef struct {
    uint64_t rocketSts; // Time + status
    int32_t gnssLat;
    int32_t gnssLon;
    int32_t gnssAlt;
    int32_t pressure;
    int16_t temperature;
    int16_t accX;
    int16_t accY;
    int16_t accZ;
    int16_t gyrX;
    int16_t gyrY;
    int16_t gyrZ;
    // Be careful to be align with 64 bits 
} DataFile_t;
// ==> sizeof(DataFile_t) = 42

void setupFileSystem();
void writeDataToBufferFile(DataFile_t* df);
void writeBufferToFile(void *buffer, uint32_t bufSize);

#endif
