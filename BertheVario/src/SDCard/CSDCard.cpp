////////////////////////////////////////////////////////////////////////////////
/// \file CSDCard.cpp
///
/// \brief
///
/// \date creation     : 04/03/2024
/// \date modification : 03/04/2024
///

#include "../BertheVario.h"

SPIClass SpiSdCard(HSPI);

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialise la carte SD
void CSDCard::InitSDCard()
{
//pinMode(SS, OUTPUT);
delay( 200 ) ;
SpiSdCard.begin(SDCARD_SCK_PIN, SDCARD_MISO_PIN, SDCARD_MOSI_PIN, SDCARD_CS_PIN);
delay( 200 ) ;
if ( !SD.begin(SDCARD_CS_PIN, SpiSdCard) )
    {
    #ifdef _LG_DEBUG_
     Serial.println("SDCard initialization failed!");
    #endif

    #ifndef MS5611_DEBUG
     CGlobalVar::Reboot() ;
    #endif
    }
delay( 100 ) ;
}
