////////////////////////////////////////////////////////////////////////////////
/// \file CSDCard.cpp
///
/// \brief
///
/// \date creation     : 04/03/2024
/// \date modification : 21/01/2025
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
    }
delay( 100 ) ;

// si /firmware.bin sur sd card
#ifdef _BERTHE_VARIO_TAC_
 g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
#endif
#ifndef _BERTHE_VARIO_TAC_
 UpdateFirmwareFromSdCard() ;
#endif
#ifdef _BERTHE_VARIO_TAC_
 g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// \brief liste des fichiers IGC avec leur temps respectif en secondes
void CSDCard::ListeIgc( CLGArray<CLGString> & VecNomIgc , CLGArray<float> & VecTempsIgc )
{
// raz vecteur in out
VecNomIgc.SetSize(0) ;
VecTempsIgc.SetSize(0) ;

/*// ouverture carte
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
        // nom et taille
        VecNomIgc.push_back(file.name()) ;
        VecTempsIgc.push_back(((float)file.size()/36)) ;
        }
    file = root.openNextFile();
    }*/

// lecture histo
g_GlobalVar.m_HistoVol.LectureFichiers() ;

// remplissage vecteurs
for ( int ii = 0 ; ii < g_GlobalVar.m_HistoVol.m_HistoDir.GetSize() ; ii++ )
    {
    VecNomIgc.Add( g_GlobalVar.m_HistoVol.m_HistoDir[ii].m_NomIgc ) ;
    VecTempsIgc.Add( g_GlobalVar.m_HistoVol.m_HistoDir[ii].m_TempsDeVol ) ;
    }

// delete histo dir
g_GlobalVar.m_HistoVol.m_HistoDir.SetSize(0) ;
g_GlobalVar.m_HistoVol.m_HistoDir.FreeExtra() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Archive tous les fichiers IGC à la racine de la carte ayant un historique
/// d'au moins une minute.
void CSDCard::ArchiveIgc()
{
// lecture des fichiers historiques
g_GlobalVar.m_HistoVol.LectureFichiers() ;

const CLGArray<CHistoVol> & VecHisto = g_GlobalVar.m_HistoVol.m_HistoDir ;

for ( int iv = 0 ; iv < VecHisto.GetSize() ; iv++ )
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

// destruction des petits fichiers
File RootDir = SD.open("/") ;
if ( !RootDir )
    {
    Serial.println("Failed to open root dir");
    return;
    }

// pour toute la directorie
while( true )
    {
    // ouverture fichier
    File Fichier = RootDir.openNextFile() ;

    // si fin de liste
    if ( ! Fichier )
        break ;

    // si directorie
    if( Fichier.isDirectory() )
        {
        Fichier.close() ;
        continue ;
        }

    // si petit fichier
    if ( Fichier.size() < 37*30 )
       {
       CLGString name = "/" ;
       name += Fichier.name() ;
       SD.remove( name ) ;
       }

    Fichier.close() ;
    }


RootDir.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Verifie les checksums des fichier hgt
bool CSDCard::VerifyChecksumGood(const char* NomFch)
{
short LigneShort[ NB_PT_PER_ROW ] ;

CLGString NomHgt = REP_HGT_FILE ;
NomHgt += "/" ;
NomHgt += NomFch ;

CLGString NomCourtChk = NomFch ;
NomCourtChk.SetAt(7,0) ;
CLGString NomChk = REP_CHK_FILE ;
NomChk += "/" ;
NomChk += NomCourtChk + ".chk" ;

File ChksFile = SD.open(NomChk) ;
if ( !ChksFile )
    {
    Serial.println("Failed to open ChksFile");
    Serial.println( ChksFile.name() ) ;
    return false ;
    }

// lecture de la cheksum
char TmpChar[5] ;
ChksFile.read( (uint8_t *) TmpChar , 5 ) ;
unsigned short ChksVerif ;
sscanf( TmpChar , "%X" , &ChksVerif ) ;
ChksFile.close() ;

// calcul de la checksum
unsigned short Checksum = 0 ;
const unsigned int NbCol = NB_PT_PER_ROW ;
unsigned int NbRead = 0 ;

// ouverture fichier hgt
File HgtFile = SD.open(NomHgt) ; // /config/hgtdata/N42E000.hgt") ;
if ( !HgtFile )
    {
    Serial.println("Failed to open HgtFile");
    Serial.println( HgtFile.name() ) ;
    return false ;
    }

// lecture/calcul du fichier hgt
while ( NbRead++ < NbCol )
    {
    HgtFile.read( (uint8_t *) & LigneShort , sizeof(short) * NB_PT_PER_ROW ) ;
    for ( int is = 0 ; is < NB_PT_PER_ROW ; is++ )
        {
        Checksum ^= LigneShort[is] ;
        if ( g_GlobalVar.GetWant2Reboot() )
            return false ;
        }
    }
HgtFile.close() ;

// verification egalite calcul/fichier
if ( Checksum != ChksVerif )
    {
    Serial.println( "bad" ) ;
    return false ;
    }

Serial.println( "good" ) ;
return true ;
}
