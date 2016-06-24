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

const FskBandwidth_t SX1276::FskBandwidths[] =
{       
    { 2600  , 0x17 },   
    { 3100  , 0x0F },
    { 3900  , 0x07 },
    { 5200  , 0x16 },
    { 6300  , 0x0E },
    { 7800  , 0x06 },
    { 10400 , 0x15 },
    { 12500 , 0x0D },
    { 15600 , 0x05 },
    { 20800 , 0x14 },
    { 25000 , 0x0C },
    { 31300 , 0x04 },
    { 41700 , 0x13 },
    { 50000 , 0x0B },
    { 62500 , 0x03 },
    { 83333 , 0x12 },
    { 100000, 0x0A },
    { 125000, 0x02 },
    { 166700, 0x11 },
    { 200000, 0x09 },
    { 250000, 0x01 },
    { 300000, 0x00 }, // Invalid Badwidth
};


SX1276::SX1276( void ( *txDone )( ), void ( *txTimeout ) ( ), void ( *rxDone ) ( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ), 
                void ( *rxTimeout ) ( ), void ( *rxError ) ( ), void ( *fhssChangeChannel ) ( uint8_t channelIndex ), void ( *cadDone ) ( bool channelActivityDetected ),
                PinName mosi, PinName miso, PinName sclk, PinName nss, PinName reset,
                PinName dio0, PinName dio1, PinName dio2, PinName dio3, PinName dio4, PinName dio5 )
            :   Radio( txDone, txTimeout, rxDone, rxTimeout, rxError, fhssChangeChannel, cadDone ),
                reset( reset ),
                isRadioActive( false )
{
    this->rxTx = 0;
    this->rxBuffer = new uint8_t[RX_BUFFER_SIZE];
    previousOpMode = RF_OPMODE_STANDBY;
    
    this->dioIrq = new DioIrqHandler[6];

    this->dioIrq[0] = &SX1276::OnDio0Irq;
    this->settings.State = IDLE;
}

SX1276::~SX1276( )
{
    delete this->rxBuffer;
    delete this->dioIrq;
}

void SX1276::RxChainCalibration( void )
{
    uint8_t regPaConfigInitVal;
    uint32_t initialFreq;

    // Save context
    regPaConfigInitVal = this->Read( REG_PACONFIG );
    initialFreq = ( double )( ( ( uint32_t )this->Read( REG_FRFMSB ) << 16 ) |
                              ( ( uint32_t )this->Read( REG_FRFMID ) << 8 ) |
                              ( ( uint32_t )this->Read( REG_FRFLSB ) ) ) * ( double )FREQ_STEP;

    // Cut the PA just in case, RFO output, power = -1 dBm
    this->Write( REG_PACONFIG, 0x00 );

    // Launch Rx chain calibration for LF band
    Write ( REG_IMAGECAL, ( Read( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    while( ( Read( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    {
	/*error  ck*/
	break;
    }

    // Sets a Frequency in HF band
    settings.Channel=  868000000 ;

    // Launch Rx chain calibration for HF band 
    Write ( REG_IMAGECAL, ( Read( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    while( ( Read( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    {
		/*error ck*/
		break;
    }

    // Restore context
    this->Write( REG_PACONFIG, regPaConfigInitVal );
    SetChannel( initialFreq );
}
void SX1276::SetChannel( uint32_t freq )
{
	debug(reset);

    this->settings.Channel = freq;
    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
    Write( REG_FRFMSB, ( uint8_t )( ( freq >> 16 ) & 0xFF ) );
    Write( REG_FRFMID, ( uint8_t )( ( freq >> 8 ) & 0xFF ) );
    Write( REG_FRFLSB, ( uint8_t )( freq & 0xFF ) );
#if 0
	debug((5));
	debug((Read(REG_FRFMSB)));
	delay(1000);
	Write( REG_FRFMSB, ( uint8_t ) ( 10 ) );
    //Write( REG_FRFMSB, ( uint8_t )( ( freq >> 16 ) & 0xFF ) );
	delay(1000);
	debug((Read(REG_FRFMSB)));
#endif
}

/*!
 * Returns the known FSK bandwidth registers value
 *
 * \param [IN] bandwidth Bandwidth value in Hz
 * \retval regValue Bandwidth register value.
 */
uint8_t SX1276::GetFskBandwidthRegValue( uint32_t bandwidth )
{
    uint8_t i;

    for( i = 0; i < ( sizeof( FskBandwidths ) / sizeof( FskBandwidth_t ) ) - 1; i++ )
    {
        if( ( bandwidth >= FskBandwidths[i].bandwidth ) && ( bandwidth < FskBandwidths[i + 1].bandwidth ) )
        {
            return FskBandwidths[i].RegValue;
        }
    }
    // ERROR: Value not found
    while( 1 );
}

void SX1276::SetRxConfig( ModemType modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, bool fixLen,
                         uint8_t payloadLen,
                         bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                         bool iqInverted, bool rxContinuous )
{
    SetModem( modem );

    switch( modem )
    {
    case MODEM_FSK:
        {
            this->settings.Fsk.Bandwidth = bandwidth;
            this->settings.Fsk.Datarate = datarate;
            this->settings.Fsk.BandwidthAfc = bandwidthAfc;
            this->settings.Fsk.FixLen = fixLen;
            this->settings.Fsk.PayloadLen = payloadLen;
            this->settings.Fsk.CrcOn = crcOn;
            this->settings.Fsk.IqInverted = iqInverted;
            this->settings.Fsk.RxContinuous = rxContinuous;
            this->settings.Fsk.PreambleLen = preambleLen;
            
            datarate = ( uint16_t )( ( double )XTAL_FREQ / ( double )datarate );
            Write( REG_BITRATEMSB, ( uint8_t )( datarate >> 8 ) );
            Write( REG_BITRATELSB, ( uint8_t )( datarate & 0xFF ) );

            Write( REG_RXBW, GetFskBandwidthRegValue( bandwidth ) );
            Write( REG_AFCBW, GetFskBandwidthRegValue( bandwidthAfc ) );

            Write( REG_PREAMBLEMSB, ( uint8_t )( ( preambleLen >> 8 ) & 0xFF ) );
            Write( REG_PREAMBLELSB, ( uint8_t )( preambleLen & 0xFF ) );

            Write( REG_PACKETCONFIG1,
                         ( Read( REG_PACKETCONFIG1 ) & 
                           RF_PACKETCONFIG1_CRC_MASK &
                           RF_PACKETCONFIG1_PACKETFORMAT_MASK ) |
                           ( ( fixLen == 1 ) ? RF_PACKETCONFIG1_PACKETFORMAT_FIXED : RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE ) |
                           ( crcOn << 4 ) );
            if( fixLen == 1 )
            {
                Write( REG_PAYLOADLENGTH, payloadLen );
            }
        }
        break;
    case MODEM_LORA:
        {
            if( bandwidth > 2 )
            {
                // Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
                while( 1 );
            }
            bandwidth += 7;
            this->settings.LoRa.Bandwidth = bandwidth;
            this->settings.LoRa.Datarate = datarate;
            this->settings.LoRa.Coderate = coderate;
            this->settings.LoRa.FixLen = fixLen;
            this->settings.LoRa.PayloadLen = payloadLen;
            this->settings.LoRa.CrcOn = crcOn;
            this->settings.LoRa.FreqHopOn = freqHopOn;
            this->settings.LoRa.HopPeriod = hopPeriod;
            this->settings.LoRa.IqInverted = iqInverted;
            this->settings.LoRa.RxContinuous = rxContinuous;
            
            if( datarate > 12 )
            {
                datarate = 12;
            }
            else if( datarate < 6 )
            {
                datarate = 6;
            }
        
            if( ( ( bandwidth == 7 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
                ( ( bandwidth == 8 ) && ( datarate == 12 ) ) )
            {
                this->settings.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                this->settings.LoRa.LowDatarateOptimize = 0x00;
            }

            Write( REG_LR_MODEMCONFIG1, 
                         ( Read( REG_LR_MODEMCONFIG1 ) &
                           RFLR_MODEMCONFIG1_BW_MASK &
                           RFLR_MODEMCONFIG1_CODINGRATE_MASK &
                           RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) |
                           ( bandwidth << 4 ) | ( coderate << 1 ) | 
                           fixLen );
                        
            Write( REG_LR_MODEMCONFIG2,
                         ( Read( REG_LR_MODEMCONFIG2 ) &
                           RFLR_MODEMCONFIG2_SF_MASK &
                           RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK &
                           RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) |
                           ( datarate << 4 ) | ( crcOn << 2 ) |
                           ( ( symbTimeout >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) );

            Write( REG_LR_MODEMCONFIG3, 
                         ( Read( REG_LR_MODEMCONFIG3 ) &
                           RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) |
                           ( this->settings.LoRa.LowDatarateOptimize << 3 ) );

            Write( REG_LR_SYMBTIMEOUTLSB, ( uint8_t )( symbTimeout & 0xFF ) );
            
            Write( REG_LR_PREAMBLEMSB, ( uint8_t )( ( preambleLen >> 8 ) & 0xFF ) );
            Write( REG_LR_PREAMBLELSB, ( uint8_t )( preambleLen & 0xFF ) );

            if( fixLen == 1 )
            {
                Write( REG_LR_PAYLOADLENGTH, payloadLen );
            }

            if( this->settings.LoRa.FreqHopOn == true )
            {
                Write( REG_LR_PLLHOP, ( Read( REG_LR_PLLHOP ) & RFLR_PLLHOP_FASTHOP_MASK ) | RFLR_PLLHOP_FASTHOP_ON );
                Write( REG_LR_HOPPERIOD, this->settings.LoRa.HopPeriod );
            }

            if( datarate == 6 )
            {
                Write( REG_LR_DETECTOPTIMIZE, 
                             ( Read( REG_LR_DETECTOPTIMIZE ) &
                               RFLR_DETECTIONOPTIMIZE_MASK ) |
                               RFLR_DETECTIONOPTIMIZE_SF6 );
                Write( REG_LR_DETECTIONTHRESHOLD, 
                             RFLR_DETECTIONTHRESH_SF6 );
            }
            else
            {
                Write( REG_LR_DETECTOPTIMIZE,
                             ( Read( REG_LR_DETECTOPTIMIZE ) &
                             RFLR_DETECTIONOPTIMIZE_MASK ) |
                             RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12 );
                Write( REG_LR_DETECTIONTHRESHOLD, 
                             RFLR_DETECTIONTHRESH_SF7_TO_SF12 );
            }
        }
        break;
    }
}

void SX1276::SetTxConfig( ModemType modem, int8_t power, uint32_t fdev, 
                        uint32_t bandwidth, uint32_t datarate,
                        uint8_t coderate, uint16_t preambleLen,
                        bool fixLen, bool crcOn, bool freqHopOn, 
                        uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
{
    uint8_t paConfig = 0;
    uint8_t paDac = 0;

    SetModem( modem );
    
    paConfig = Read( REG_PACONFIG );
    paDac = Read( REG_PADAC );

    paConfig = ( paConfig & RF_PACONFIG_PASELECT_MASK ) | GetPaSelect( this->settings.Channel );
    paConfig = ( paConfig & RF_PACONFIG_MAX_POWER_MASK ) | 0x70;

    if( ( paConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if( power > 17 )
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_ON;
        }
        else
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_OFF;
        }
        if( ( paDac & RF_PADAC_20DBM_ON ) == RF_PADAC_20DBM_ON )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )
            {
                power = 2;
            }
            if( power > 17 )
            {
                power = 17;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if( power < -1 )
        {
            power = -1;
        }
        if( power > 14 )
        {
            power = 14;
        }
        paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    Write( REG_PACONFIG, paConfig );
    Write( REG_PADAC, paDac );

    switch( modem )
    {
    case MODEM_FSK:
        {
            this->settings.Fsk.Power = power;
            this->settings.Fsk.Fdev = fdev;
            this->settings.Fsk.Bandwidth = bandwidth;
            this->settings.Fsk.Datarate = datarate;
            this->settings.Fsk.PreambleLen = preambleLen;
            this->settings.Fsk.FixLen = fixLen;
            this->settings.Fsk.CrcOn = crcOn;
            this->settings.Fsk.IqInverted = iqInverted;
            this->settings.Fsk.TxTimeout = timeout;
            
            fdev = ( uint16_t )( ( double )fdev / ( double )FREQ_STEP );
            Write( REG_FDEVMSB, ( uint8_t )( fdev >> 8 ) );
            Write( REG_FDEVLSB, ( uint8_t )( fdev & 0xFF ) );

            datarate = ( uint16_t )( ( double )XTAL_FREQ / ( double )datarate );
            Write( REG_BITRATEMSB, ( uint8_t )( datarate >> 8 ) );
            Write( REG_BITRATELSB, ( uint8_t )( datarate & 0xFF ) );

            Write( REG_PREAMBLEMSB, ( preambleLen >> 8 ) & 0x00FF );
            Write( REG_PREAMBLELSB, preambleLen & 0xFF );

            Write( REG_PACKETCONFIG1,
                         ( Read( REG_PACKETCONFIG1 ) & 
                           RF_PACKETCONFIG1_CRC_MASK &
                           RF_PACKETCONFIG1_PACKETFORMAT_MASK ) |
                           ( ( fixLen == 1 ) ? RF_PACKETCONFIG1_PACKETFORMAT_FIXED : RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE ) |
                           ( crcOn << 4 ) );
        }
        break;
    case MODEM_LORA:
        {
            this->settings.LoRa.Power = power;
            if( bandwidth > 2 )
            {
                // Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
                while( 1 );
            }
            bandwidth += 7;
            this->settings.LoRa.Bandwidth = bandwidth;
            this->settings.LoRa.Datarate = datarate;
            this->settings.LoRa.Coderate = coderate;
            this->settings.LoRa.PreambleLen = preambleLen;
            this->settings.LoRa.FixLen = fixLen;
            this->settings.LoRa.CrcOn = crcOn;
            this->settings.LoRa.FreqHopOn = freqHopOn;
            this->settings.LoRa.HopPeriod = hopPeriod;
            this->settings.LoRa.IqInverted = iqInverted;
            this->settings.LoRa.TxTimeout = timeout;

            if( datarate > 12 )
            {
                datarate = 12;
            }
            else if( datarate < 6 )
            {
                datarate = 6;
            }
            if( ( ( bandwidth == 7 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
                ( ( bandwidth == 8 ) && ( datarate == 12 ) ) )
            {
                this->settings.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                this->settings.LoRa.LowDatarateOptimize = 0x00;
            }
            
            if( this->settings.LoRa.FreqHopOn == true )
            {
                Write( REG_LR_PLLHOP, ( Read( REG_LR_PLLHOP ) & RFLR_PLLHOP_FASTHOP_MASK ) | RFLR_PLLHOP_FASTHOP_ON );
                Write( REG_LR_HOPPERIOD, this->settings.LoRa.HopPeriod );
            }
            
            Write( REG_LR_MODEMCONFIG1, 
                         ( Read( REG_LR_MODEMCONFIG1 ) &
                           RFLR_MODEMCONFIG1_BW_MASK &
                           RFLR_MODEMCONFIG1_CODINGRATE_MASK &
                           RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) |
                           ( bandwidth << 4 ) | ( coderate << 1 ) | 
                           fixLen );

            Write( REG_LR_MODEMCONFIG2,
                         ( Read( REG_LR_MODEMCONFIG2 ) &
                           RFLR_MODEMCONFIG2_SF_MASK &
                           RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) |
                           ( datarate << 4 ) | ( crcOn << 2 ) );

            Write( REG_LR_MODEMCONFIG3, 
                         ( Read( REG_LR_MODEMCONFIG3 ) &
                           RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) |
                           ( this->settings.LoRa.LowDatarateOptimize << 3 ) );
        
            Write( REG_LR_PREAMBLEMSB, ( preambleLen >> 8 ) & 0x00FF );
            Write( REG_LR_PREAMBLELSB, preambleLen & 0xFF );
            
            if( datarate == 6 )
            {
                Write( REG_LR_DETECTOPTIMIZE, 
                             ( Read( REG_LR_DETECTOPTIMIZE ) &
                               RFLR_DETECTIONOPTIMIZE_MASK ) |
                               RFLR_DETECTIONOPTIMIZE_SF6 );
                Write( REG_LR_DETECTIONTHRESHOLD, 
                             RFLR_DETECTIONTHRESH_SF6 );
            }
            else
            {
                Write( REG_LR_DETECTOPTIMIZE,
                             ( Read( REG_LR_DETECTOPTIMIZE ) &
                             RFLR_DETECTIONOPTIMIZE_MASK ) |
                             RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12 );
                Write( REG_LR_DETECTIONTHRESHOLD, 
                             RFLR_DETECTIONTHRESH_SF7_TO_SF12 );
            }
        }
        break;
    }
}
void SX1276::Send( uint8_t *buffer, uint8_t size )
{
    uint32_t txTimeout = 0;

    this->settings.State = IDLE;

    switch( this->settings.Modem )
    {
    case MODEM_FSK:
        {
            this->settings.FskPacketHandler.NbBytes = 0;
            this->settings.FskPacketHandler.Size = size;
            if( this->settings.Fsk.FixLen == false )
            {
                WriteFifo( ( uint8_t* )&size, 1 );
            }
            else
            {
                Write( REG_PAYLOADLENGTH, size );
            }            
            if( ( size > 0 ) && ( size <= 64 ) )
            {
                this->settings.FskPacketHandler.ChunkSize = size;
            }
            else
            {
                this->settings.FskPacketHandler.ChunkSize = 32;
            }

            // Write payload buffer
            WriteFifo( buffer, this->settings.FskPacketHandler.ChunkSize );
            this->settings.FskPacketHandler.NbBytes += this->settings.FskPacketHandler.ChunkSize;
            txTimeout = this->settings.Fsk.TxTimeout;
        }
        break;
    case MODEM_LORA:
        {
            if( this->settings.LoRa.IqInverted == true )
            {
                Write( REG_LR_INVERTIQ, ( ( Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_ON ) );
            }
            else
            {
                Write( REG_LR_INVERTIQ, ( ( Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF ) );
            }      
        
            this->settings.LoRaPacketHandler.Size = size;

            // Initializes the payload size
            Write( REG_LR_PAYLOADLENGTH, size );

            // Full buffer used for Tx            
            Write( REG_LR_FIFOTXBASEADDR, 0 );
            Write( REG_LR_FIFOADDRPTR, 0 );

            // FIFO operations can not take place in Sleep mode
            if( ( Read( REG_OPMODE ) & ~RF_OPMODE_MASK ) == RF_OPMODE_SLEEP )
            {
                Standby( );
            }
            // Write payload buffer
            WriteFifo( buffer, size );
            txTimeout = this->settings.LoRa.TxTimeout;
        }
        break;
    }

    Tx( txTimeout );
}
void SX1276::Standby( void )
{
    SetOpMode( RF_OPMODE_STANDBY );
}

void SX1276::Rx( uint32_t timeout )
{
    bool rxContinuous = false;

    switch( this->settings.Modem )
    {
    case MODEM_FSK:
        {
            rxContinuous = this->settings.Fsk.RxContinuous;
            
            // DIO0=PayloadReady
            // DIO1=FifoLevel
            // DIO2=SyncAddr
            // DIO3=FifoEmpty
            // DIO4=Preamble
            // DIO5=ModeReady
            Write( REG_DIOMAPPING1, ( Read( REG_DIOMAPPING1 ) & RF_DIOMAPPING1_DIO0_MASK & RF_DIOMAPPING1_DIO1_MASK &
                                                                            RF_DIOMAPPING1_DIO2_MASK ) |
                                                                            RF_DIOMAPPING1_DIO0_00 |
                                                                            RF_DIOMAPPING1_DIO2_11 );
            
            Write( REG_DIOMAPPING2, ( Read( REG_DIOMAPPING2 ) & RF_DIOMAPPING2_DIO4_MASK &
                                                                            RF_DIOMAPPING2_MAP_MASK ) | 
                                                                            RF_DIOMAPPING2_DIO4_11 |
                                                                            RF_DIOMAPPING2_MAP_PREAMBLEDETECT );
            
            this->settings.FskPacketHandler.FifoThresh = Read( REG_FIFOTHRESH ) & 0x3F;
            
            this->settings.FskPacketHandler.PreambleDetected = false;
            this->settings.FskPacketHandler.SyncWordDetected = false;
            this->settings.FskPacketHandler.NbBytes = 0;
            this->settings.FskPacketHandler.Size = 0;
        }
        break;
    case MODEM_LORA:
        {
            if( this->settings.LoRa.IqInverted == true )
            {
                Write( REG_LR_INVERTIQ, ( ( Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_ON | RFLR_INVERTIQ_TX_OFF ) );
            }
            else
            {
                Write( REG_LR_INVERTIQ, ( ( Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF ) );
            }         
        
            rxContinuous = this->settings.LoRa.RxContinuous;
            
            if( this->settings.LoRa.FreqHopOn == true )
            {
                Write( REG_LR_IRQFLAGSMASK, //RFLR_IRQFLAGS_RXTIMEOUT |
                                              //RFLR_IRQFLAGS_RXDONE |
                                              //RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                              RFLR_IRQFLAGS_VALIDHEADER |
                                              RFLR_IRQFLAGS_TXDONE |
                                              RFLR_IRQFLAGS_CADDONE |
                                              //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                              RFLR_IRQFLAGS_CADDETECTED );
                                              
                // DIO0=RxDone, DIO2=FhssChangeChannel
                Write( REG_DIOMAPPING1, ( Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO2_MASK  ) | RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO2_00 );
            }
            else
            {
                Write( REG_LR_IRQFLAGSMASK, //RFLR_IRQFLAGS_RXTIMEOUT |
                                              //RFLR_IRQFLAGS_RXDONE |
                                              //RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                              RFLR_IRQFLAGS_VALIDHEADER |
                                              RFLR_IRQFLAGS_TXDONE |
                                              RFLR_IRQFLAGS_CADDONE |
                                              RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                              RFLR_IRQFLAGS_CADDETECTED );
                                              
                // DIO0=RxDone
                Write( REG_DIOMAPPING1, ( Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_00 );
            }
            
            Write( REG_LR_FIFORXBASEADDR, 0 );
            Write( REG_LR_FIFOADDRPTR, 0 );
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
        if( rxContinuous == true )
        {
            SetOpMode( RFLR_OPMODE_RECEIVER );
        }
        else
        {
            SetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
        }
    }
}

void SX1276::Tx( uint32_t timeout )
{ 
    switch( this->settings.Modem )
    {
    case MODEM_FSK:
        {
            // DIO0=PacketSent
            // DIO1=FifoLevel
            // DIO2=FifoFull
            // DIO3=FifoEmpty
            // DIO4=LowBat
            // DIO5=ModeReady
            Write( REG_DIOMAPPING1, ( Read( REG_DIOMAPPING1 ) & RF_DIOMAPPING1_DIO0_MASK & RF_DIOMAPPING1_DIO1_MASK &
                                                                            RF_DIOMAPPING1_DIO2_MASK ) );

            Write( REG_DIOMAPPING2, ( Read( REG_DIOMAPPING2 ) & RF_DIOMAPPING2_DIO4_MASK &
                                                                            RF_DIOMAPPING2_MAP_MASK ) );
            this->settings.FskPacketHandler.FifoThresh = Read( REG_FIFOTHRESH ) & 0x3F;
        }
        break;
    case MODEM_LORA:
        {
        
            if( this->settings.LoRa.FreqHopOn == true )
            {
                Write( REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
                                              RFLR_IRQFLAGS_RXDONE |
                                              RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                              RFLR_IRQFLAGS_VALIDHEADER |
                                              //RFLR_IRQFLAGS_TXDONE |
                                              RFLR_IRQFLAGS_CADDONE |
                                              //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                              RFLR_IRQFLAGS_CADDETECTED );
                                              
                // DIO0=TxDone
                Write( REG_DIOMAPPING1, ( Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_01 );
                // DIO2=FhssChangeChannel
                Write( REG_DIOMAPPING1, ( Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO2_MASK ) | RFLR_DIOMAPPING1_DIO2_00 );  
            }
            else
            {
                Write( REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
                                              RFLR_IRQFLAGS_RXDONE |
                                              RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                              RFLR_IRQFLAGS_VALIDHEADER |
                                              //RFLR_IRQFLAGS_TXDONE |
                                              RFLR_IRQFLAGS_CADDONE |
                                              RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                              RFLR_IRQFLAGS_CADDETECTED );
                                              
                // DIO0=TxDone
                Write( REG_DIOMAPPING1, ( Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_01 ); 
            }
        }
        break;
    }

    this->settings.State = TX;
    SetOpMode( RF_OPMODE_TRANSMITTER );
}
void SX1276::SetOpMode( uint8_t opMode )
{
    if( opMode != previousOpMode )
    {
        previousOpMode = opMode;
        if( opMode == RF_OPMODE_SLEEP )
        {
            SetAntSwLowPower( true );
        }
        else
        {
            SetAntSwLowPower( false );
            if( opMode == RF_OPMODE_TRANSMITTER )
            {
                 SetAntSw( 1 );
            }
            else
            {
                 SetAntSw( 0 );
            }
        }
        Write( REG_OPMODE, ( Read( REG_OPMODE ) & RF_OPMODE_MASK ) | opMode );
    }
}

void SX1276::SetModem( ModemType modem )
{
    if( this->settings.Modem != modem )
    {
        this->settings.Modem = modem;
        switch( this->settings.Modem )
        {
        default:
        case MODEM_FSK:
            SetOpMode( RF_OPMODE_SLEEP );
            Write( REG_OPMODE, ( Read( REG_OPMODE ) & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_OFF );
        
            Write( REG_DIOMAPPING1, 0x00 );
            Write( REG_DIOMAPPING2, 0x30 ); // DIO5=ModeReady
            break;
        case MODEM_LORA:
            SetOpMode( RF_OPMODE_SLEEP );
            Write( REG_OPMODE, ( Read( REG_OPMODE ) & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON );
            Write( 0x30, 0x00 ); //  IF = 0
            Write( REG_LR_DETECTOPTIMIZE, ( Read( REG_LR_DETECTOPTIMIZE ) & 0x7F ) ); // Manual IF
            Write( REG_DIOMAPPING1, 0x00 );
            Write( REG_DIOMAPPING2, 0x00 );
            break;
        }
    }
}

void SX1276::OnTimeoutIrq( void )
{
    switch( this->settings.State )
    {
    case RX:
        if( this->settings.Modem == MODEM_FSK )
        {
            this->settings.FskPacketHandler.PreambleDetected = false;
            this->settings.FskPacketHandler.SyncWordDetected = false;
            this->settings.FskPacketHandler.NbBytes = 0;
            this->settings.FskPacketHandler.Size = 0;

            // Clear Irqs
            Write( REG_IRQFLAGS1, RF_IRQFLAGS1_RSSI | 
                                        RF_IRQFLAGS1_PREAMBLEDETECT |
                                        RF_IRQFLAGS1_SYNCADDRESSMATCH );
            Write( REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN );

            if( this->settings.Fsk.RxContinuous == true )
            {
                // Continuous mode restart Rx chain
                Write( REG_RXCONFIG, Read( REG_RXCONFIG ) | RF_RXCONFIG_RESTARTRXWITHOUTPLLLOCK );
            }
            else
            {
                this->settings.State = IDLE;
            }
        }
        if( ( rxTimeout != NULL ) )
        {
            rxTimeout( );
        }
        break;
    case TX:
        this->settings.State = IDLE;
        if( ( txTimeout != NULL ) )
        {
            txTimeout( );
        }
        break;
    default:
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
