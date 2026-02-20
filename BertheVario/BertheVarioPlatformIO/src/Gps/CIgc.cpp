////////////////////////////////////////////////////////////////////////////////
/// \file CIgc.cpp
///
/// \brief
///
/// \date creation   : 14/03/2024
/// \date 22/10/2025 : ordre dans PushLoc2Igc
/// \date 01/01/2026 : buffer d'ecriture igc toutes les 15 secondes
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

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialise le fichier IGC sur la carte SD
void CIgc::InitCurentIgc()
{
int secondes_date = g_GlobalVar.m_HeureSec ;

// nom de fichier igc
sprintf( m_NomFchCourant ,"/%02d%02d%02d%02d.IGC" ,
    g_GlobalVar.m_Mois ,
    g_GlobalVar.m_Jour ,
    (int) (secondes_date/3600) ,   // heure
    (int)((secondes_date/60)%60)   // minutes
    ) ;

// ouverture fichier
m_File = SD.open(m_NomFchCourant, FILE_WRITE , true );
if (!m_File)
    {
    #ifdef _LG_DEBUG_
     Serial.print( "erreur creation fichier " ) ;
     Serial.println( m_NomFchCourant ) ;
    #endif
    return ;
    }

// debut de fichier
m_File.seek(0) ;

// elaboration date
char TmpDate[50] ;
int Annee = g_GlobalVar.m_Annee - 2000 ;
if ( Annee < 0 )
    Annee = 0 ;
sprintf(TmpDate,"HFDTEDATE:%02d%02d%02d\n", g_GlobalVar.m_Jour, g_GlobalVar.m_Mois, Annee ) ;

// ecriture entete de fichier IGC
m_File.print("AXXXABC GnuVario-E\n") ;
m_File.print(TmpDate) ;     // date
m_File.print("HFPLTPILOTINCHARGE: ") ;
m_File.print(g_GlobalVar.m_Config.m_Pilote ) ;  // pilote
m_File.print("\nHFGTYGLIDERTYPE: " ) ;
m_File.print( g_GlobalVar.m_Config.m_Voile ) ; //glider
m_File.print("\nHFDTM100GPSDATUM: WGS-1984\n") ; // repere
m_File.print("HFFTYFRTYPE: GnuVario-E\n") ; // vario

m_File.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ajoute la position courante dans le fichier igc
void CIgc::PushLoc2Igc(bool write)
{
// signe de position
float SigneLat = (g_GlobalVar.m_TerrainPosDeco.m_Lat>=0.) ? 1. : -1 ;
float SigneLon = (g_GlobalVar.m_TerrainPosDeco.m_Lon>=0.) ? 1. : -1 ;

int secondes_date = g_GlobalVar.m_HeureSec ;

// mutex lat lon
g_GlobalVar.m_MutexLatLonGps.PrendreMutex() ;

// local lat/lon char
char LocalLat[10] ;
char LocalLon[10] ;
strcpy( LocalLat , g_GlobalVar.m_LatChar ) ;
strcpy( LocalLon , g_GlobalVar.m_LonChar ) ;

// mutex lat lon
g_GlobalVar.m_MutexLatLonGps.RelacherMutex() ;

// remise a zero du ponteur de buffer
if ( m_pBuff == NULL )
    {
    m_pBuff = m_BufferEcriture ;
    *m_pBuff = 0 ;
    }

// si lat ou lon negatif
char * pLat = LocalLat ;
char * pLon = LocalLon ;
if ( SigneLat < 0. ) pLat++ ;
if ( SigneLon < 0. ) pLon++ ;

// si pas fin d'enregistrement de fin de tache
if ( !write )
    {
    // champ                 hh   mm   ss  LLlllll
    sprintf( m_pBuff , "B%02d%02d%02d%s%c%s%cA%05d%05d\n" ,
    (int) (secondes_date/3600) ,    // heure
    (int)((secondes_date/60)%60) ,  // minutes
    (int)(secondes_date%60) ,       // secondes
    LocalLat , ((SigneLat>=0.) ? 'N' : 'S') ,  // lat
    LocalLon , ((SigneLon>=0.) ? 'E' : 'W') ,  // lon
    (int)g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ,
    //(int)g_GlobalVar.m_AltiGps
    (int)0
    ) ;
    }

// on avance le pointeur
while( (*m_pBuff) != 0 )
    m_pBuff++ ;

// longueur ligne a ecrire
int LongLigneFch = m_pBuff - m_BufferEcriture  ;

#ifdef IGC_DEBUG
    Serial.println( g_GlobalVar.m_LatChar ) ;
    Serial.println( g_GlobalVar.m_LatFloat ) ;
    Serial.println( g_GlobalVar.m_LonChar ) ;
    Serial.println( g_GlobalVar.m_LonFloat ) ;
#endif

// si write demande ou buffer plain moins 2 messages
if ( write || LongLigneFch > ( TAILLE_BUFFER_FCH - (TAILLE_MESSAGE_COURANT*MARGE_BUFFER) ) )
    {
    // ouverture fichier
    m_File = SD.open(m_NomFchCourant, FILE_APPEND );
    if (!m_File)
        {
        #ifdef _LG_DEBUG_
        Serial.print( "erreur creation fichier " ) ;
        Serial.println( m_NomFchCourant ) ;
        #endif
        return ;
        }

    // ecriture record dans fichier
    m_File.write( (const uint8_t*) m_BufferEcriture , LongLigneFch ) ;

    // raz du buffer
    m_pBuff = NULL ;

    // fermeture fichier
    m_File.close() ;
    }
}
