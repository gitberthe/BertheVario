////////////////////////////////////////////////////////////////////////////////
/// \file CSDCard.cpp
///
/// \brief
///
/// \date creation     : 04/03/2024
/// \date modification : 09/08/2024
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
        VecTempsIgc.push_back(file.size()/37) ;
        }
    file = root.openNextFile();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Archive tous les fichiers IGC à la racine de la carte ayant un historique
/// d'au moins une minute.
void CSDCard::ArchiveIgc()
{
// lecture des fichiers historiques
g_GlobalVar.m_HistoVol.LectureFichiers() ;

const std::vector<CHistoVol> & VecHisto = g_GlobalVar.m_HistoVol.m_HistoDir ;

for ( int iv = 0 ; iv < VecHisto.size() ; iv++ )
    {
    const CHistoVol & HistoVol =  VecHisto[iv] ;

    // nouveau nom de fichier
    char NomFchArch[50] = "/arch" ;
    sprintf( NomFchArch ,"/arch/%04d/%s", (int)HistoVol.m_AnneeDuVol, HistoVol.m_NomIgc ) ;

    // creation nouvelle directorie annee
    char NomDirArch[50] ;
    sprintf( NomDirArch , "/arch/%04d" , (int)HistoVol.m_AnneeDuVol ) ;
    SD.mkdir( "/arch" ) ;
    SD.mkdir( NomDirArch ) ;

    // deplacement/destruction fichier
    if ( HistoVol.m_TempsDeVol < 1 )
        SD.remove( HistoVol.m_NomIgc ) ;
    else
        SD.rename( HistoVol.m_NomIgc , NomFchArch ) ;
    }


// destruction historique
g_GlobalVar.m_HistoVol.DeleteHisto() ;

// ouverture carte
/*
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
*/
}
