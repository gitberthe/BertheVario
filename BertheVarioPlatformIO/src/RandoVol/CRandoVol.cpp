////////////////////////////////////////////////////////////////////////////////
/// \file CRandoVol.cpp
///
/// \brief
///
/// \date creation     : 13/01/2025
/// \date modification : 14/01/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation rando-vol et lancement des taches
void CRandoVol::InitRandoVol()
{
// tache gps
LanceTacheRandoVol() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage rando-vol à l'ecran
void CRandoVol::AfficheRandoVol()
{
g_GlobalVar.EcranRandoVol() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance les tache de gestion du Gps : acquisition serie
void CRandoVol::LanceTacheRandoVol()
{
#ifdef _LG_DEBUG_
 Serial.println("lancement tache gps");
#endif

// tache de port serie
xTaskCreatePinnedToCore(CGps::TacheGpsSerial, "GpsSerial", SERIAL_GPS_STACK_SIZE, this, SERIAL_GPS_PRIORITY,NULL, SERIAL_GPS_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction de tri de vector gpx.
bool MySortFunctionGpx( CFileGpx *p1, CFileGpx *p2)
{
return ((*p1)<(*p2));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri distance croissante pour determination position dans menu
void CRandoVol::SortGpxFromCurPos()
{
std::sort(m_VecGpx.begin(), m_VecGpx.end(),MySortFunctionGpx);
//std::reverse(VecZones.begin(),VecZones.end());
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi les nom des traces
const char * CRandoVol::GetTrackName( int i ) const
{
if ( i < m_VecGpx.size() )
    return m_VecGpx[i]->m_TrackName.c_str() ;
return "--" ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief lit les fichiers Gpx, positionne la distance au point courant,
/// tri en distance et ne garde que le plus proche
void CRandoVol::LireFichiersGpx()
{
// ouverture directorie histo
File GpxDir = SD.open(GPX_DIR) ;
if ( !GpxDir )
    {
    Serial.println("Failed to open gpx dir");
    return;
    }

// point courant
CFileGpx::StPoint CurPts ;
CurPts.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
CurPts.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon ;
CurPts.m_Alt = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
// pour toute la directorie lecture fichier
while( true )
    {
    // ouverture fichier
    File SdFileGpx = GpxDir.openNextFile() ;

    if ( ! SdFileGpx )
        break ;

    char TmpChar[50] = GPX_DIR ;
    strcat( TmpChar , "/" ) ;
    strcat( TmpChar , SdFileGpx.name() ) ;

    // nom fichier
    CFileGpx * pFileGpxMem = new CFileGpx ;
    pFileGpxMem->m_FileName = TmpChar ;

    // lecture fichier
    pFileGpxMem->LireFichier() ;
    pFileGpxMem->SetDistanceFrom( CurPts ) ;
    // destruction trace
    pFileGpxMem->m_VecTrack.clear() ;
    pFileGpxMem->m_VecTrack.shrink_to_fit() ;
    // ajout du fichier
    m_VecGpx.push_back( pFileGpxMem ) ;
    // fermeture fichier
    SdFileGpx.close() ;
    Serial.println( pFileGpxMem->m_FileName.c_str() ) ;
    }

GpxDir.close() ;

// tri des Gps en distance
SortGpxFromCurPos() ;

// relecture premier Gps
if ( m_VecGpx.size() )
    m_VecGpx[0]->LireFichier() ;

Serial.println( "lecture Gpx ok" ) ;
}
