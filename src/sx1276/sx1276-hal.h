#ifndef __SX1276_HAL_H__
#define __SX1276_HAL_H__

#include "sx1276/sx1276.h"

class SX1276MB1xAS : public SX1276
{
public:
            SX1276MB1xAS( void ( *txDone )( ), void ( *txTimeout ) ( ), void ( *rxDone ) ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ),
                          void ( *rxTimeout ) ( ), void ( *rxError ) ( ), void ( *fhssChangeChannel ) ( uint8_t channelIndex ), void ( *cadDone ) ( bool ChannelActivityDetected ) );
	//virtual ~SX1276MB1xAS( ) { };
    /*!
     * @brief Reads the radio register at the specified address
     *
     * @param [IN]: addr Register address
     * @retval data Register value
     */
    virtual uint8_t Read ( uint8_t addr ) ;
};

#endif // __SX1276_HAL_H__