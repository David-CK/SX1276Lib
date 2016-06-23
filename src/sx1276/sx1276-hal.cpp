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
#include "sx1276/sx1276-hal.h"

SX1276MB1xAS::SX1276MB1xAS( void ( *txDone )( ), void ( *txTimeout ) ( ), void ( *rxDone ) ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ), 
                            void ( *rxTimeout ) ( ), void ( *rxError ) ( ), void ( *fhssChangeChannel ) ( uint8_t channelIndex ), void ( *cadDone ) ( bool ChannelActivityDetected ) ) 
                        :   SX1276( txDone, txTimeout, rxDone, rxTimeout, rxError, fhssChangeChannel, cadDone, 11, 12, 13, 10, 0, 0, 0, 0, 0, 0, 0 )
{
}
uint8_t SX1276MB1xAS::Read( uint8_t addr )
{
	/*CK*/
}
