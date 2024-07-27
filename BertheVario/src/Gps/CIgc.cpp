////////////////////////////////////////////////////////////////////////////////
/// \file CIgc.cpp
///
/// \brief
///
/// \date creation     : 14/03/2024
/// \date modification : 26/07/2024
///

#include "../BertheVario.h"

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
sprintf(TmpDate,"HFDTEDATE:%02d%02d%02d", g_GlobalVar.m_Jour, g_GlobalVar.m_Mois, Annee ) ;

// ecriture entete de fichier IGC
m_File.println("AXXX GnuVario-E") ;
m_File.println(TmpDate) ;     // date
m_File.print("HFPLTPILOTINCHARGE: ") ;
m_File.println(g_GlobalVar.m_Config.m_Pilote.c_str() ) ;  // pilote
m_File.print("HFGTYGLIDERTYPE: " ) ;
m_File.println( g_GlobalVar.m_Config.m_Voile.c_str() ) ; //glider
m_File.println("HFDTM100GPSDATUM: WGS-1984") ; // repere
m_File.println("HFFTYFRTYPE: GnuVario-E") ; // vario

m_File.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ajoute la position courante dans le fichier igc
void CIgc::PushLoc2Igc()
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

g_GlobalVar.m_MutexVariable.PrendreMutex() ;

// variable local pour minimiser le temps mutex du sprintf qui est assez long je pense
char CharRecord[100] ;

// signe de position
float SigneLat = (g_GlobalVar.m_TerrainPosDeco.m_Lat>=0.) ? 1. : -1 ;
float SigneLon = (g_GlobalVar.m_TerrainPosDeco.m_Lon>=0.) ? 1. : -1 ;

int secondes_date = g_GlobalVar.m_HeureSec ;

// local lat/lon char
char LocalLat[10] ;
char LocalLon[10] ;
strcpy( LocalLat , g_GlobalVar.m_LatChar ) ;
strcpy( LocalLon , g_GlobalVar.m_LonChar ) ;

g_GlobalVar.m_MutexVariable.RelacherMutex() ;

#ifdef IGC_DEBUG
    Serial.println( g_GlobalVar.m_LatChar ) ;
    Serial.println( g_GlobalVar.m_LatFloat ) ;
    Serial.println( g_GlobalVar.m_LonChar ) ;
    Serial.println( g_GlobalVar.m_LonFloat ) ;
#endif

/*// si lat ou lon negatif
char * pLat = LocalLat ;
char * pLon = LocalLon ;
if ( SigneLat < 0. ) pLat++ ;
if ( SigneLon < 0. ) pLon++ ;*/

// champ                 hh   mm   ss  LLlllll
sprintf( CharRecord , "B%02d%02d%02d%s%c%s%cA%05d%05d" ,
    (int) (secondes_date/3600) ,    // heure
    (int)((secondes_date/60)%60) ,  // minutes
    (int)(secondes_date%60) ,       // secondes
    LocalLat , ((SigneLat>=0.) ? 'N' : 'S') ,  // lat
    LocalLon , ((SigneLon>=0.) ? 'E' : 'W') ,  // lon
    (int)g_GlobalVar.m_TerrainPosCur.m_AltiBaro ,
    //(int)g_GlobalVar.m_AltiGps
    (int)0
    ) ;


// ecriture record dans fichier
m_File.println(CharRecord) ;

m_File.close() ;
}
