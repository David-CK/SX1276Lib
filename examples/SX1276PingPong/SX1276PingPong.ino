#include <SX1276Lib.h>

SX1276MB1xAS Radio( OnTxDone, OnTxTimeout, OnRxDone, OnRxTimeout, OnRxError, NULL, NULL );

void setup()
{
    debug( "\n\n\r     SX1276 Ping Pong Demo Application \n\n\r" );
    
    // verify the connection with the board
    while( Radio.Read( REG_VERSION ) == 0x00  )
    {
        debug( "Radio could not be detected!\n\r" );
    }
}

void loop()
{
  
}

void OnTxDone( void )
{
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
}

void OnTxTimeout( void )
{
}

void OnRxTimeout( void )
{
}

void OnRxError( void )
{
}
