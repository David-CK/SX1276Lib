/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    ( C )2014 Semtech

Description: Actual implementation of a SX1276 radio, inherits Radio

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainers: Miguel Luis, Gregory Cristian and Nicolas Huguenin
*/
#ifndef __SX1276_H__
#define __SX1276_H__

#include "radio/radio.h"
#include "typedefs/typedefs.h"  // //

#define RX_BUFFER_SIZE                              256
class SX1276 : public Radio
{
    uint8_t *rxBuffer;
    /*!
     * Hardware DIO IRQ functions
     */
    DioIrqHandler *dioIrq;
    RadioSettings_t settings;
public:
    SX1276( void ( *txDone )( ), void ( *txTimeout ) ( ), void ( *rxDone ) ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ), 
            void ( *rxTimeout ) ( ), void ( *rxError ) ( ), void ( *fhssChangeChannel ) ( uint8_t channelIndex ), void ( *cadDone ) ( bool channelActivityDetected ),
            PinName mosi, PinName miso, PinName sclk, PinName nss, PinName reset,
            PinName dio0, PinName dio1, PinName dio2, PinName dio3, PinName dio4, PinName dio5 ); 
    //virtual ~SX1276( );
    /*!
     * @brief Sends the buffer of size. Prepares the packet to be sent and sets
     *        the radio in transmission
     *
     * @param [IN]: buffer     Buffer pointer
     * @param [IN]: size       Buffer size
     */
    virtual void Send( uint8_t *buffer, uint8_t size );
    /*!
     * @brief Sets the radio in reception mode for the given time
     * @param [IN] timeout Reception timeout [us]
     *                     [0: continuous, others timeout]
     */
    virtual void Rx( uint32_t timeout );
    
    /*!
     * @brief Sets the radio in transmission mode for the given time
     * @param [IN] timeout Transmission timeout [us]
     *                     [0: continuous, others timeout]
     */
    virtual void Tx( uint32_t timeout );
    
    /*!
     * @brief Reads the radio register at the specified address
     *
     * @param [IN]: addr Register address
     * @retval data Register value
     */
    virtual uint8_t Read ( uint8_t addr ) = 0;
    /*!
     * @brief DIO 0 IRQ callback
     */
    virtual void OnDio0Irq( void );
};

#endif //__SX1276_H__
