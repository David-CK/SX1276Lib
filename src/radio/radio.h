/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    ( C )2014 Semtech

Description: Interface for the radios, contains the main functions that a radio needs, and 5 callback functions

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainers: Miguel Luis, Gregory Cristian and Nicolas Huguenin
*/
#ifndef __RADIO_H__
#define __RADIO_H__

#include "enums/enums.h"

/*!
 *    Interface for the radios, contains the main functions that a radio needs, and 5 callback functions
 */
class Radio
{
protected:

    //-------------------------------------------------------------------------
    //                        Callback functions pointers
    //-------------------------------------------------------------------------
    
    /*!
     * @brief  Tx Done callback prototype.
     */
    void ( *txDone )( );

    /*!
     * @brief  Tx Timeout callback prototype.
     */
    void ( *txTimeout ) ( );

    /*!
     * @brief Rx Done callback prototype.
     *
     * @param [IN] payload Received buffer pointer
     * @param [IN] size    Received buffer size
     * @param [IN] rssi    RSSI value computed while receiving the frame [dBm]
     * @param [IN] snr     Raw SNR value given by the radio hardware
     *                     FSK : N/A ( set to 0 )
     *                     LoRa: SNR value in dB
     */
    void ( *rxDone ) ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

    /*!
     * @brief  Rx Timeout callback prototype.
     */
    void ( *rxTimeout ) ( );
    
    /*!
     * @brief Rx Error callback prototype.
     */
    void ( *rxError ) ( );
    
    /*!
     * \brief  FHSS Change Channel callback prototype.
     *
     * \param [IN] CurrentChannel   Index number of the current channel
     */
    void ( *fhssChangeChannel )( uint8_t CurrentChannel );

    /*!
     * @brief CAD Done callback prototype.
     *
     * @param [IN] ChannelDetected    Channel Activity detected during the CAD
     */
    void ( *cadDone ) ( bool channelActivityDetected );
    
public:
    //-------------------------------------------------------------------------
    //                        Constructor
    //-------------------------------------------------------------------------
    /*!
     * @brief Constructor of the radio object, the parameters are the callback functions described in the header.
     * @param [IN]    txDone
     * @param [IN]    txTimeout
     * @param [IN]    rxDone
     * @param [IN]    rxTimeout
     * @param [IN]    rxError
     */
    Radio( void ( *txDone )( ), void ( *txTimeout ) ( ), void ( *rxDone ) ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ), 
           void ( *rxTimeout ) ( ), void ( *rxError ) ( ), void ( *fhssChangeChannel ) ( uint8_t channelIndex ), void ( *cadDone ) ( bool channelActivityDetected ) );
    virtual ~Radio( ) {};
    /*!
     * @brief Sends the buffer of size. Prepares the packet to be sent and sets
     *        the radio in transmission
     *
     * @param [IN]: buffer     Buffer pointer
     * @param [IN]: size       Buffer size
     */
    virtual void Send( uint8_t *buffer, uint8_t size ) = 0;
    /*!
     * @brief Sets the radio in reception mode for the given time
     * @param [IN] timeout Reception timeout [us]
     *                     [0: continuous, others timeout]
     */
    virtual void Rx( uint32_t timeout ) = 0;
    
    /*!
     * @brief Sets the radio in transmission mode for the given time
     * @param [IN] timeout Transmission timeout [us]
     *                     [0: continuous, others timeout]
     */
    virtual void Tx( uint32_t timeout ) = 0;
    /*!
     * @brief Reads the radio register at the specified address
     *
     * @param [IN]: addr Register address
     * @retval data Register value
     */
    virtual uint8_t Read ( uint8_t addr ) = 0;
};

#endif // __RADIO_H__

