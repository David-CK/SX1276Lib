/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    ( C )2014 Semtech

Description: SX1276 LoRa modem registers and bits definitions

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __SX1276_REGS_LORA_H__
#define __SX1276_REGS_LORA_H__

/*!
 * ============================================================================
 * SX1276 Internal registers Address
 * ============================================================================
 */
// Common settings
// Tx settings
// Rx settings
// LoRa registers
#define REG_LR_FIFOADDRPTR                          0x0D 
#define REG_LR_FIFOTXBASEADDR                       0x0E 
#define REG_LR_FIFORXBASEADDR                       0x0F 
#define REG_LR_IRQFLAGSMASK                         0x11 
#define REG_LR_MODEMCONFIG1                         0x1D 
#define REG_LR_MODEMCONFIG2                         0x1E 
#define REG_LR_SYMBTIMEOUTLSB                       0x1F 
#define REG_LR_PREAMBLEMSB                          0x20 
#define REG_LR_PREAMBLELSB                          0x21 
#define REG_LR_PAYLOADLENGTH                        0x22 
#define REG_LR_PAYLOADMAXLENGTH                     0x23 
#define REG_LR_HOPPERIOD                            0x24 
#define REG_LR_MODEMCONFIG3                         0x26
#define REG_LR_DETECTOPTIMIZE                       0x31
#define REG_LR_INVERTIQ                             0x33
#define REG_LR_DETECTIONTHRESHOLD                   0x37
// end of documented register in datasheet
// I/O settings
// Version
// Additional settings
#define REG_LR_PLLHOP                               0x44
/*!
 * ============================================================================
 * SX1276 LoRa bits control definition
 * ============================================================================
 */
/*!
 * RegFifo
 */
/*!
 * RegOpMode
 */
#define RFLR_OPMODE_LONGRANGEMODE_MASK              0x7F 
#define RFLR_OPMODE_LONGRANGEMODE_OFF               0x00 // Default
#define RFLR_OPMODE_LONGRANGEMODE_ON                0x80 
#define RFLR_OPMODE_TRANSMITTER                     0x03 
#define RFLR_OPMODE_RECEIVER                        0x05 
// LoRa specific modes
#define RFLR_OPMODE_RECEIVER_SINGLE                 0x06 
/*!
 * RegFrf ( MHz )
 */
/*!
 * RegPaConfig
 */
/*!
 * RegPaRamp
 */
/*!
 * RegOcp
 */
/*!
 * RegLna
 */
/*!
 * RegFifoAddrPtr
 */
/*!
 * RegFifoTxBaseAddr
 */
/*!
 * RegFifoTxBaseAddr
 */
/*!
 * RegFifoRxCurrentAddr ( Read Only )
 */
/*!
 * RegIrqFlagsMask
 */
/*!
 * RegIrqFlags
 */
#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80 
#define RFLR_IRQFLAGS_RXDONE                        0x40 
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20 
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10 
#define RFLR_IRQFLAGS_TXDONE                        0x08 
#define RFLR_IRQFLAGS_CADDONE                       0x04 
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02 
#define RFLR_IRQFLAGS_CADDETECTED                   0x01 
/*!
 * RegFifoRxNbBytes ( Read Only )
 */
 
/*!
 * RegRxHeaderCntValueMsb ( Read Only )
 */
 
/*!
 * RegRxHeaderCntValueLsb ( Read Only )
 */
 
 
/*!
 * RegRxPacketCntValueMsb ( Read Only )
 */
 
 
/*!
 * RegRxPacketCntValueLsb ( Read Only )
 */
/*!
 * RegModemStat ( Read Only )
 */
/*!
 * RegPktSnrValue ( Read Only )
 */

 
 /*!
 * RegPktRssiValue ( Read Only )
 */
 
 
/*!
 * RegRssiValue ( Read Only )
 */

/*!
 * RegHopChannel ( Read Only )
 */
/*!
 * RegModemConfig1
 */
#define RFLR_MODEMCONFIG1_BW_MASK                   0x0F 
#define RFLR_MODEMCONFIG1_CODINGRATE_MASK           0xF1 
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK       0xFE 
/*!
 * RegModemConfig2
 */
#define RFLR_MODEMCONFIG2_SF_MASK                   0x0F 
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK         0xFB 
#define RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK       0xFC 
/*!
 * RegSymbTimeoutLsb
 */
/*!
 * RegPreambleLengthMsb
 */
/*!
 * RegPreambleLengthLsb
 */
/*!
 * RegPayloadLength
 */
/*!
 * RegPayloadMaxLength
 */
/*!
 * RegHopPeriod
 */
/*!
 * RegFifoRxByteAddr ( Read Only )
 */
/*!
 * RegModemConfig3
 */
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK  0xF7 
/*!
 * RegFeiMsb ( Read Only )
 */

/*!
 * RegFeiMid ( Read Only )
 */

/*!
 * RegFeiLsb ( Read Only )
 */

/*!
 * RegRssiWideband ( Read Only )
 */

/*!
 * RegDetectOptimize
 */
#define RFLR_DETECTIONOPTIMIZE_MASK                 0xF8
#define RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12          0x03 // Default
#define RFLR_DETECTIONOPTIMIZE_SF6                  0x05

/*!
 * RegInvertIQ
 */
#define RFLR_INVERTIQ_RX_MASK                       0xBF
#define RFLR_INVERTIQ_RX_OFF                        0x00
#define RFLR_INVERTIQ_RX_ON                         0x40
#define RFLR_INVERTIQ_TX_MASK                       0xFE
#define RFLR_INVERTIQ_TX_OFF                        0x01
#define RFLR_INVERTIQ_TX_ON                         0x00

/*!
 * RegDetectionThreshold
 */
#define RFLR_DETECTIONTHRESH_SF7_TO_SF12            0x0A // Default
#define RFLR_DETECTIONTHRESH_SF6                    0x0C

/*!
 * RegDioMapping1
 */
#define RFLR_DIOMAPPING1_DIO0_MASK                  0x3F
#define RFLR_DIOMAPPING1_DIO0_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO0_01                    0x40
#define RFLR_DIOMAPPING1_DIO2_MASK                  0xF3
#define RFLR_DIOMAPPING1_DIO2_00                    0x00  // Default
/*!
 * RegDioMapping2
 */
/*!
 * RegVersion ( Read Only )
 */

/*!
 * RegPllHop
 */
#define RFLR_PLLHOP_FASTHOP_MASK                    0x7F
#define RFLR_PLLHOP_FASTHOP_ON                      0x80
/*!
 * RegTcxo
 */
/*!
 * RegPaDac
 */
/*!
 * RegFormerTemp
 */

/*!
 * RegBitrateFrac
 */
/*!
 * RegAgcRef
 */

/*!
 * RegAgcThresh1
 */

/*!
 * RegAgcThresh2
 */

/*!
 * RegAgcThresh3
 */

/*!
 * RegPll
 */
#endif // __SX1276_REGS_LORA_H__

