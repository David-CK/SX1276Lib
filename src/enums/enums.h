/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    ( C )2014 Semtech

Description: -

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainers: Miguel Luis, Gregory Cristian and Nicolas Huguenin
*/
#ifndef __ENUMS_H__
#define __ENUMS_H__

#include <stdint.h>
//#include <stdio.h>
#include <stddef.h>
//#include <stdbool.h>
//#include <stdlib.h>
#include <string.h>

typedef uint8_t			PinName;

/*!
 *    State of the radio:
 *    [IDLE,
 *     RX_RUNNING, RX_TIMEOUT, RX_ERROR,
 *     TX_RUNNING, TX_TIMEOUT,
       CAD]
 */
enum RadioState
{
    RX,
    TX,
};

/*!
 *    Type of the modem. [LORA / FSK]
 */
enum ModemType
{
    MODEM_FSK = 0,
    MODEM_LORA
};

typedef struct
{
    bool     FreqHopOn;
    bool     IqInverted;
}RadioLoRaSettings_t;

typedef struct
{
    RadioState               State;
    ModemType                Modem;
    RadioLoRaSettings_t      LoRa;
}RadioSettings_t;

#endif //__ENUMS_H__

