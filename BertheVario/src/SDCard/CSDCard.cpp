////////////////////////////////////////////////////////////////////////////////
/// \file CSDCard.cpp
///
/// \brief
///
/// \date creation     : 04/03/2024
/// \date modification : 26/07/2024
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

////////////////////////////////////////////////////////////////////////////////
/// \brief liste des fichiers IGC
void CSDCard::ListeIgc( std::vector<std::string> & VecNomIgc , std::vector<int> & VecTempsIgc )
{
// raz vecteur in out
VecNomIgc.clear() ;
VecTempsIgc.clear() ;

// ouverture carte
File root = SD.open("/");
if(!root)
    {
    Serial.println("Failed to open directory");
    return;
    }

if(!root.isDirectory())
    {
    Serial.println("Not a directory");
    return;
    }

// pour tous les fichier
File file = root.openNextFile();
while(file)
    {
    if(file.isDirectory())
        {
        //Serial.print("  DIR : ");
        //Serial.println(file.name());
        //if(levels){
            //listDir(fs, file.name(), levels -1);
            //}
        }
    else
        {
        /*Serial.print("  FILE: ");
        Serial.print(file.name());
        Serial.print("  SIZE: ");
        Serial.println(file.size());*/
        // nom et taille
        VecNomIgc.push_back(file.name()) ;
        VecTempsIgc.push_back(file.size()/36) ;
        }
    file = root.openNextFile();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Detruit tous les fichiers IGC
void CSDCard::DeleteIgc()
{
// ouverture carte
File root = SD.open("/");
File file = root.openNextFile();

// pour tous les fichiers
while(file)
    {
    if(file.isDirectory())
        {
        }
    else
        {
        // recherche nom de fichier igc
        std::string filename = file.name() ;
        const char * pChar = filename.c_str() ;
        // destruction fichier
        if ( strcasestr( pChar , ".igc" ) != NULL )
            {
            std::string filenamepath = "/" ;
            filenamepath += filename ;
            SD.remove( filenamepath.c_str() ) ;
            }
        }
    file = root.openNextFile();
    }
}
