#include "sx1276/sx1276-hal.h"
#include "registers/sx1276Regs-Fsk.h"
#include "debug-git/debug.h"

#if 0
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
#endif
