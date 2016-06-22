#include "sx1276/sx1276.h"

SX1276::SX1276( void ( *txDone )( ), void ( *txTimeout ) ( ), void ( *rxDone ) ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ), 
                void ( *rxTimeout ) ( ), void ( *rxError ) ( ), void ( *fhssChangeChannel ) ( uint8_t channelIndex ), void ( *cadDone ) ( bool channelActivityDetected ),
                PinName mosi, PinName miso, PinName sclk, PinName nss, PinName reset,
                PinName dio0, PinName dio1, PinName dio2, PinName dio3, PinName dio4, PinName dio5 )
            :   Radio( txDone, txTimeout, rxDone, rxTimeout, rxError, fhssChangeChannel, cadDone )
{
}
