//
// Created by mawood on 01/11/2020.
//

#ifndef VERSION_H
#define VERSION_H
#include <stdint.h>

typedef struct
{
    uint8_t letter;
    uint8_t major;
    uint8_t minor;
    uint8_t subMinor;
} version_t;
version_t VERSION __attribute__((section(".versionInfo"))) =
        {
        .letter = 'A',
        .major = 1U,
        .minor = 0U,
        .subMinor = 0U
        };
#endif //VERSION_H