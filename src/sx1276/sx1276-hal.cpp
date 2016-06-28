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

const RadioRegisters_t SX1276MB1xAS::RadioRegsInit[] = 
{                                                 
    { MODEM_FSK , REG_LNA                , 0x23 },
    { MODEM_FSK , REG_RXCONFIG           , 0x1E },
    { MODEM_FSK , REG_RSSICONFIG         , 0xD2 },
    { MODEM_FSK , REG_PREAMBLEDETECT     , 0xAA },
    { MODEM_FSK , REG_OSC                , 0x07 },
    { MODEM_FSK , REG_SYNCCONFIG         , 0x12 },
    { MODEM_FSK , REG_SYNCVALUE1         , 0xC1 },
    { MODEM_FSK , REG_SYNCVALUE2         , 0x94 },
    { MODEM_FSK , REG_SYNCVALUE3         , 0xC1 },
    { MODEM_FSK , REG_PACKETCONFIG1      , 0xD8 },
    { MODEM_FSK , REG_FIFOTHRESH         , 0x8F },
    { MODEM_FSK , REG_IMAGECAL           , 0x02 },
    { MODEM_FSK , REG_DIOMAPPING1        , 0x00 },
    { MODEM_FSK , REG_DIOMAPPING2        , 0x30 },
    { MODEM_LORA, REG_LR_PAYLOADMAXLENGTH, 0x40 },  
};

SX1276MB1xAS::SX1276MB1xAS( void ( *txDone )( ), void ( *txTimeout ) ( ), void ( *rxDone ) ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ), 
                            void ( *rxTimeout ) ( ), void ( *rxError ) ( ), void ( *fhssChangeChannel ) ( uint8_t channelIndex ), void ( *cadDone ) ( bool ChannelActivityDetected ) ) 
                        :   SX1276( txDone, txTimeout, rxDone, rxTimeout, rxError, fhssChangeChannel, cadDone, 11, 12, 13, 10, A5, 0, 0, 0, 0, 0, 0 )
{
    Reset( );
    
    boardConnected = UNKNOWN;
    
    DetectBoardType( );
    
    RxChainCalibration( );
    
    IoInit( );
    
    SetOpMode( RF_OPMODE_SLEEP );
	/**ck**/
    IoIrqInit( dioIrq );
    
    RadioRegistersInit( );

    SetModem( MODEM_FSK );

    this->settings.State = IDLE ;
	os_init();
}

//-------------------------------------------------------------------------
//                      Board relative functions
//-------------------------------------------------------------------------
uint8_t SX1276MB1xAS::DetectBoardType( void )
{
    if( boardConnected == UNKNOWN )
    {
            boardConnected = SX1276MB1LAS;
    }
    return ( boardConnected );
}

void SX1276MB1xAS::IoInit( void )
{
    SpiInit( );
}

void SX1276MB1xAS::RadioRegistersInit( ){
    uint8_t i = 0;
    for( i = 0; i < sizeof( RadioRegsInit ) / sizeof( RadioRegisters_t ); i++ )
    {
        SetModem( RadioRegsInit[i].Modem );
        Write( RadioRegsInit[i].Addr, RadioRegsInit[i].Value );
    }    
}

void SX1276MB1xAS::SpiInit( void )
{/*????*/
	SPI.begin();
}
static const SPISettings settings(10E6, MSBFIRST, SPI_MODE0);
void handler(void);
void SX1276MB1xAS::IoIrqInit( DioIrqHandler *irqHandlers )
{
Serial.begin(115200);
debug("irqinit\n");
//attachInterrupt(A4, handler, RISING);
attachInterrupt(0, handler, CHANGE );

}

void SX1276MB1xAS::IoDeInit( void )
{
    //nothing
}

uint8_t SX1276MB1xAS::GetPaSelect( uint32_t channel )
{
    if( channel > RF_MID_BAND_THRESH )
    {
    }
    else
    {
	/**ck**/
        return RF_PACONFIG_PASELECT_RFO;
    }
}

void SX1276MB1xAS::SetAntSwLowPower( bool status )
{
    if( isRadioActive != status )
    {
        isRadioActive = status;
    }
}

void SX1276MB1xAS::AntSwInit( void )
{
}

void SX1276MB1xAS::AntSwDeInit( void )
{
}

void SX1276MB1xAS::SetAntSw( uint8_t rxTx )
{
    if( this->rxTx == rxTx )
    {
        //no need to go further
        return;
    }

    this->rxTx = rxTx;

}

static void lora_pin_nss(uint8_t val)
{
	if (!val)
	{
		SPI.beginTransaction(settings);
	}
	else
	{
		SPI.endTransaction();
	}

	digitalWrite(10, val);
}

uint8_t lora_spi(uint8_t out)
{
	uint8_t res = SPI.transfer(out);
	return res;
}

static void spi_write(uint8_t addr, uint8_t data)
{
	lora_pin_nss(0);
	lora_spi(addr | 0x80);
	lora_spi(data);
	lora_pin_nss(1);
}

static uint8_t spi_read(uint8_t addr)
{
	lora_pin_nss(0);
	lora_spi(addr & 0x7F);
	uint8_t val = lora_spi(0x00);
	lora_pin_nss(1);
	return val;
}

bool SX1276MB1xAS::CheckRfFrequency( uint32_t frequency )
{
    //TODO: Implement check, currently all frequencies are supported
    return true;
}


void SX1276MB1xAS::Reset( void )
{
	pinMode( reset, OUTPUT );
	digitalWrite( reset, LOW );
	//delay( 1 );
	pinMode( reset, INPUT );
	//delay( 6 );
}
    
void SX1276MB1xAS::Write( uint8_t addr, uint8_t data )
{
	spi_write(addr, data);
	/*CK*/
}

uint8_t SX1276MB1xAS::Read( uint8_t addr )
{
    uint8_t data;
    data = spi_read( addr );
    return data;
	/*CK*/
}

void SX1276MB1xAS::Write( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    lora_pin_nss(0);
    lora_spi(addr | 0x80);
    for (uint8_t i = 0; i < size; i++) {
        lora_spi(buffer[i]);
    }
    lora_pin_nss(1);
	/*CK*/
}

void SX1276MB1xAS::Read( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    lora_pin_nss(0);
    lora_spi(addr & 0x7F);
    for (uint8_t i = 0; i < size; i++) {
        buffer[i] = lora_spi(0x00);
    }
    lora_pin_nss(1);
	/*CK*/
}

void SX1276MB1xAS::WriteFifo( uint8_t *buffer, uint8_t size )
{
    Write( 0, buffer, size );
	/*CK*/
}

void SX1276MB1xAS::ReadFifo( uint8_t *buffer, uint8_t size )
{
    Read( 0, buffer, size );
	/*CK*/
}
