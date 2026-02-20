////////////////////////////////////////////////////////////////////////////////
/// \file CRandoVol.cpp
///
/// \brief
///
/// \date creation     : 13/01/2025
/// \date modification : 17/10/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation rando-vol et lancement des taches
void CRandoVol::InitRandoVol()
{
m_VecGpx.SetDeleteObjet(true) ;
// tache gps
LancerTacheRandoVol() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage rando-vol à l'ecran
void CRandoVol::AfficheRandoVol()
{
g_GlobalVar.EcranRandoVol() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance les tache de gestion du Gps : acquisition serie
void CRandoVol::LancerTacheRandoVol()
{
#ifdef _LG_DEBUG_
 Serial.println("lancement tache gps");
#endif

// tache de port serie
xTaskCreatePinnedToCore(CGps::TacheGpsSerial, "GpsSerial", SERIAL_GPS_STACK_SIZE, this, SERIAL_GPS_PRIORITY,
    GetTaskHandle(SERIAL_GPS_NUM_TASK), SERIAL_GPS_CORE);
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
//std::sort(m_VecGpx.begin(), m_VecGpx.end(),MySortFunctionGpx);
//std::reverse(VecZones.begin(),VecZones.end());
m_VecGpx.TriAdapte(true) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi les nom des traces
const char * CRandoVol::GetTrackName( int i ) const
{
if ( i < m_VecGpx.GetSize() )
    return m_VecGpx[i]->m_TrackName ;
return "--" ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief lit les fichiers Gpx, positionne la distance au point courant;
/// Lit aussi le fichier terconnu.txt
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
CurPts.m_Alt = g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ;
m_VecGpx.SetGrow( 500 ) ;
///////////////////////////////////////////////////
// pour toute la directorie lecture des fichier gpx
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
    delete pFileGpxMem->m_pVecTrack ;
    pFileGpxMem->m_pVecTrack = NULL ;
    // ajout du fichier
    m_VecGpx.Add( pFileGpxMem ) ;
    // fermeture fichier
    SdFileGpx.close() ;
    Serial.println( pFileGpxMem->m_FileName ) ;
    }
GpxDir.close() ;

/////////////////////////////////
// lecture fichier terrain connus
for ( int it = 0 ; it < 2 ; it++ )
    {
    CTerrainsConnu TerrainsConnu ;
    if ( it == 0 )
        TerrainsConnu.LireFichierTerrains(TERRAIN_FCH) ;
    else
        TerrainsConnu.LireFichierTerrains(PT_REMARQUABLE_FCH) ;
    for ( int it = 0 ; it < TerrainsConnu.GetSize() ; it++ )
        {
        CFileGpx * pFileGpxMem = new CFileGpx ;
        const CLocTerrain * pTerrain = TerrainsConnu.GetData(it) ;
        // nom
        pFileGpxMem->m_TrackName = "pt-" ,
        pFileGpxMem->m_TrackName += pTerrain->m_Nom ;
        Serial.println ( pFileGpxMem->m_TrackName ) ;
        // point
        CFileGpx::StPoint PtsTer ;
        PtsTer.m_Lat = pTerrain->m_Lat ;
        PtsTer.m_Lon = pTerrain->m_Lon ;
        PtsTer.m_Alt = pTerrain->m_AltiBaroRec ;
        // ajout point
        pFileGpxMem->m_pVecTrack = new CLGArray<CFileGpx::StPoint> ;
        pFileGpxMem->m_pVecTrack->Add(PtsTer) ;
        // calcul distance
        pFileGpxMem->m_DistFrom = sqrtf( powf(CurPts.m_Lat-PtsTer.m_Lat,2.0) + powf(CurPts.m_Lon-PtsTer.m_Lon,2.0) ) ;
        // point de terrain connus
        pFileGpxMem->m_PtTerConnu = true ;
        // loupe
        float DeltaLL = 0. ;
        float Dist = fabsf(CurPts.m_Lat-PtsTer.m_Lat) ;
        if ( DeltaLL < Dist )
            DeltaLL = Dist ;
        Dist = fabsf(CurPts.m_Lon-PtsTer.m_Lon) ;
        if ( DeltaLL < Dist )
        DeltaLL = Dist ;
        pFileGpxMem->m_SlopeMax = 3.*DeltaLL/SLOPE_MAX_DIV ;

        // ajout du fichier
        m_VecGpx.Add( pFileGpxMem ) ;
        }
    }


// tri des Gpx en distance
SortGpxFromCurPos() ;

Serial.println( "lecture Gpx ok" ) ;
}
