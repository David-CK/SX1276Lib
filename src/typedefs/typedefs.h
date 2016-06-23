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
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__
class SX1276;
/*!
 * Hardware IO IRQ callback function definition
 */
typedef void ( SX1276::*DioIrqHandler )( void );
#endif //__TYPEDEFS_H__

