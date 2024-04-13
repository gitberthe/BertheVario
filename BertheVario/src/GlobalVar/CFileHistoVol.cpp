////////////////////////////////////////////////////////////////////////////////
/// \file CFileHistoVol.cpp
///
/// \brief
///
/// \date creation     : 22/03/2024
/// \date modification : 07/04/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture du fichier histo au debut du programme
void CFileHistoVol::LectureFichier()
{
char * TmpChar = new char [5000] ;
int ic = 0 ;

// ouverture fichier
m_File = SD.open(NOM_FCH_HISTO_VOL);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(m_File.available())
    {
    TmpChar[ic++] = m_File.read();
    }
TmpChar[ic++] = 0 ;

m_File.close();

std::vector<char*> VecLigne ;

// decoupage en ligne
char * pChar = strtok( TmpChar , "\n" ) ;
while ( pChar != NULL )
    {
    // recopie
    char * pLigne = new char[1000] ;
    strcpy( pLigne , pChar ) ;
    VecLigne.push_back( pLigne ) ;
    pChar = strtok( NULL , "\n" ) ;
    }

// analyse des champs
char Separ[] = " #\t" ;
for ( int i = 0 ; i < VecLigne.size() ; i++ )
    {
    char * pLigne = VecLigne[i] ;
    char * pNomParam = strtok( pLigne , Separ ) ;
    char * pValParam = strtok( NULL , Separ ) ;
    if ( pNomParam == NULL || pValParam == NULL )
        continue ;
    if ( !strcmp( pNomParam , "[VzMax]" ) )
        m_VzMax = atof(pValParam) ;
    else if ( !strcmp( pNomParam , "[VzMin]" ) )
        m_VzMin = atof(pValParam) ;
    else if ( !strcmp( pNomParam , "[VsMax]" ) )
        m_VsMax = atof(pValParam) ;
    else if ( !strcmp( pNomParam , "[temps_vol]" ) )
        m_TempsDeVol = atoi(pValParam) ;
    else if ( !strcmp( pNomParam , "[distance]" ) )
        m_DistanceKm = atof(pValParam) ;
    else if ( !strcmp( pNomParam , "[z_deco]" ) )
        m_ZDeco = atoi(pValParam) ;
    else if ( !strcmp( pNomParam , "[z_max]" ) )
        m_ZMax = atoi(pValParam) ;
    }

// liberation memoire
for ( int i = 0 ; i < VecLigne.size() ; i++ )
    delete [] VecLigne[i] ;

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture toutes les 5s du fichier.
void CFileHistoVol::EcritureFichier()
{
// ouverture fichier
m_File = SD.open(NOM_FCH_HISTO_VOL, FILE_WRITE , true );
if (!m_File)
    {
    #ifdef _LG_DEBUG_
     Serial.print( "erreur creation fichier NOM_FCH_HISTO_VOL" ) ;
    #endif
    return ;
    }

// debut de fichier
m_File.seek(0) ;

// ecritude fichier
m_File.print("[VzMax] ") ; m_File.println( m_VzMax ) ;
m_File.print("[VzMin] ") ; m_File.println( m_VzMin ) ;
m_File.print("[VsMax] ") ; m_File.println( m_VsMax ) ;
m_File.print("[temps_vol] ") ; m_File.println( m_TempsDeVol ) ;
m_File.print("[distance] ") ; m_File.println( m_DistanceKm ) ;
m_File.print("[z_deco] ") ; m_File.println( m_ZDeco ) ;
m_File.print("[z_max] ") ; m_File.println( m_ZMax ) ;

// fermeture fichier
m_File.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul les VzMax, VzMin, et renvoi la distance...
float CFileHistoVol::PushHistoVol()
{
// temps de vol
m_TempsDeVol = g_GlobalVar.m_DureeVolMin ;

// altitude max
float AltiPression = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
if ( m_ZMax < AltiPression )
    m_ZMax = AltiPression ;

// Vz Max
float Vz = g_GlobalVar.m_VitVertMS ;
if ( m_VzMax < Vz )
    m_VzMax = Vz ;

// Vz Min
if ( m_VzMin > Vz )
    m_VzMin = Vz ;

// Vs Max
float Vs = g_GlobalVar.m_VitesseKmh ;
if ( m_VsMax < Vs )
    m_VsMax = Vs ;

// distance
float DistanceMetresLat = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lat - m_LastLat ) * 60. * UnMileEnMetres ;
float DistanceMetresLon = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lon - m_LastLon ) * 60. * UnMileEnMetres ;
float DistanceDeplacementMetres = sqrtf( powf(DistanceMetresLat,2) + powf(DistanceMetresLon,2) ) ;
m_DistanceKm += DistanceDeplacementMetres / 1000.  ;
m_LastLat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
m_LastLon = g_GlobalVar.m_TerrainPosCur.m_Lon ;

// pour le calcul de la finesse
return DistanceDeplacementMetres ;
}
