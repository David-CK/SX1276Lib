/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    ( C )2014 Semtech

Description: SX1276 FSK modem registers and bits definitions

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __SX1276_REGS_FSK_H__
#define __SX1276_REGS_FSK_H__
#define REG_OPMODE                                  0x01
#define REG_BITRATEMSB                              0x02
#define REG_BITRATELSB                              0x03
#define REG_FDEVMSB                                 0x04 
#define REG_FDEVLSB                                 0x05
#define REG_FRFMSB                                  0x06
#define REG_FRFMID                                  0x07
#define REG_FRFLSB                                  0x08
#define REG_PACONFIG                                0x09
#define REG_LNA                                     0x0C
#define REG_RXCONFIG                                0x0D
#define REG_RSSICONFIG                              0x0E
#define REG_RSSIVALUE                               0x11
#define REG_RXBW                                    0x12 
#define REG_AFCBW                                   0x13
#define REG_PREAMBLEDETECT                          0x1F
#define REG_OSC                                     0x24
#define REG_PREAMBLEMSB                             0x25
#define REG_PREAMBLELSB                             0x26
#define REG_SYNCCONFIG                              0x27
#define REG_SYNCVALUE1                              0x28
#define REG_SYNCVALUE2                              0x29
#define REG_SYNCVALUE3                              0x2A
#define REG_PACKETCONFIG1                           0x30
#define REG_PAYLOADLENGTH                           0x32
#define REG_FIFOTHRESH                              0x35
#define REG_IMAGECAL                                0x3B
#define REG_IRQFLAGS1                               0x3E
#define REG_IRQFLAGS2                               0x3F
#define REG_DIOMAPPING1                             0x40
#define REG_DIOMAPPING2                             0x41
// Version
#define REG_VERSION                                 0x42
#define REG_PADAC                                   0x4D
#define RF_OPMODE_MASK                              0xF8
#define RF_OPMODE_SLEEP                             0x00
#define RF_OPMODE_STANDBY                           0x01  // Default
#define RF_OPMODE_TRANSMITTER                       0x03
#define RF_OPMODE_RECEIVER                          0x05
#define RF_PACONFIG_PASELECT_MASK                   0x7F
#define RF_PACONFIG_PASELECT_PABOOST                0x80
#define RF_PACONFIG_PASELECT_RFO                    0x00 // Default
#define RF_PACONFIG_MAX_POWER_MASK                  0x8F
#define RF_PACONFIG_OUTPUTPOWER_MASK                0xF0
#define RF_RXCONFIG_RESTARTRXWITHOUTPLLLOCK         0x40 // Write only
#define RF_SYNCCONFIG_SYNCSIZE_MASK                 0xF8
#define RF_PACKETCONFIG1_PACKETFORMAT_MASK          0x7F
#define RF_PACKETCONFIG1_PACKETFORMAT_FIXED         0x00
#define RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE      0x80  // Default
#define RF_PACKETCONFIG1_CRC_MASK                   0xEF
#define RF_PACKETCONFIG1_ADDRSFILTERING_MASK         0xF9
#define RF_IMAGECAL_IMAGECAL_MASK                   0xBF
#define RF_IMAGECAL_IMAGECAL_START                  0x40
#define RF_IMAGECAL_IMAGECAL_RUNNING                0x20
#define RF_IRQFLAGS1_RSSI                           0x08
#define RF_IRQFLAGS1_PREAMBLEDETECT                 0x02
#define RF_IRQFLAGS1_SYNCADDRESSMATCH               0x01
#define RF_IRQFLAGS2_FIFOOVERRUN                    0x10
#define RF_DIOMAPPING1_DIO0_MASK                    0x3F
#define RF_DIOMAPPING1_DIO0_00                      0x00  // Default
#define RF_DIOMAPPING1_DIO1_MASK                    0xCF
#define RF_DIOMAPPING1_DIO2_MASK                    0xF3
#define RF_DIOMAPPING1_DIO2_11                      0x0C
#define RF_DIOMAPPING2_DIO4_MASK                    0x3F
#define RF_DIOMAPPING2_DIO4_11                      0xC0
#define RF_DIOMAPPING2_MAP_MASK                     0xFE
#define RF_DIOMAPPING2_MAP_PREAMBLEDETECT           0x01
#define RF_PADAC_20DBM_MASK                         0xF8
#define RF_PADAC_20DBM_ON                           0x07
#define RF_PADAC_20DBM_OFF                          0x04  // Default
#endif // __SX1276_REGS_FSK_H__

