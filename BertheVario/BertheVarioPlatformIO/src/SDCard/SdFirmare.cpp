////////////////////////////////////////////////////////////////////////////////
/// \file SdFirmware.cpp
///
/// \brief
///
/// \date creation   : 24/01/2026
/// \date 24/01/2026 : mofification
///

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif

// Update progress status
void progresscallback(size_t currSize, size_t totalSize)
{
  //Serial.printf("CALLBACK:  Update process at %d of %d bytes...n", currSize, totalSize);
  //g_GlobalVar.DoChar( 0 , 50 ,)
}

////////////////////////////////////////////////////////////////////////////////
/// \brief fonction qui update le firmware depuis la SD card.
/// Puis efface le fichier firmware.bin.
void UpdateFirmwareFromSdCard()
{
// si ouverture du fichier ok
Serial.print("nSearching for firmware update..");
File firmware =  SD.open("/firmware.bin");
if (firmware)
    {
    // screen
    g_GlobalVar.ScreenRaz() ;
    g_GlobalVar.DoChar( 0 , 50 , "Update firmware" ) ;

    // flash le firmware
    Serial.println("Found!");
    Serial.println("Try to update!");
    Update.onProgress(progresscallback);
    Update.begin(firmware.size(), U_FLASH);
    Update.writeStream(firmware);
    if (Update.end())
        {
        Serial.println("Update finished!");
        g_GlobalVar.DoChar( 0 , 50 , "Update firmware : Ok" ) ;
        delay( 500 ) ;
        }
    else

        {
        g_GlobalVar.DoChar( 0 , 50 , "Update firmware : error" ) ;
        Serial.println("Update error!");
        Serial.println(Update.getError());
        delay( 500 ) ;
        }

    // fermeture du firmware
    firmware.close();

    // destruction du firmware
    if (SD.remove("/firmware.bin"))
        {
        Serial.println("Firmware remove successfully!");
        }
    else
        {
        Serial.println("Firmware remove error!");
        }

    // redemmarrage
    delay(200);

    ESP.restart();
    }
else
    {
    Serial.println("Firmware update not found!");
    }
}

