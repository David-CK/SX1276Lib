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
#include "sx1276/sx1276.h"

SX1276::SX1276( void ( *txDone )( ), void ( *txTimeout ) ( ), void ( *rxDone ) ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ), 
                void ( *rxTimeout ) ( ), void ( *rxError ) ( ), void ( *fhssChangeChannel ) ( uint8_t channelIndex ), void ( *cadDone ) ( bool channelActivityDetected ),
                PinName mosi, PinName miso, PinName sclk, PinName nss, PinName reset,
                PinName dio0, PinName dio1, PinName dio2, PinName dio3, PinName dio4, PinName dio5 )
            :   Radio( txDone, txTimeout, rxDone, rxTimeout, rxError, fhssChangeChannel, cadDone )
{
    this->rxBuffer = new uint8_t[RX_BUFFER_SIZE];
    this->dioIrq = new DioIrqHandler[6];

    this->dioIrq[0] = &SX1276::OnDio0Irq;
}
void SX1276::Send( uint8_t *buffer, uint8_t size )
{
    switch( this->settings.Modem )
    {
    case MODEM_FSK:
        {
        }
        break;
    case MODEM_LORA:
        {
        }
        break;
    }
}
void SX1276::Rx( uint32_t timeout )
{
    //bool rxContinuous = false;

    switch( this->settings.Modem )
    {
    case MODEM_FSK:
        {
        }
        break;
    case MODEM_LORA:
        {
            if( this->settings.LoRa.IqInverted == true )
            {
            }
            else
            {
            }         
            //rxContinuous = this->settings.LoRa.RxContinuous;
            if( this->settings.LoRa.FreqHopOn == true )
            {
            }
            else
            {
            }
        }
        break;
    }

    memset( rxBuffer, 0, ( size_t )RX_BUFFER_SIZE );

    this->settings.State = RX;
    if( timeout != 0 )
    {
    }
    if( this->settings.Modem == MODEM_FSK )
    {
    }
    else
    {
    }
}

void SX1276::Tx( uint32_t timeout )
{ 
    switch( this->settings.Modem )
    {
    case MODEM_FSK:
        {
        }
        break;
    case MODEM_LORA:
        {
        }
        break;
    }
}
void SX1276::OnDio0Irq( void )
{
    switch( this->settings.State )
    {                
        case RX:
            switch( this->settings.Modem )
            {
            case MODEM_FSK:
                break;
            case MODEM_LORA:
                {
                }
                break;
            default:
                break;
            }
            break;
        case TX:
            break;
        default:
            break;
    }
}
