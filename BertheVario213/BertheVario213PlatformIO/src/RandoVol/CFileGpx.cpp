////////////////////////////////////////////////////////////////////////////////
/// \file CFileGpx.cpp
///
/// \brief
///
/// \date creation     : 14/01/2025
/// \date modification : 18/01/2025
///

#include "../BertheVario213.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
CFileGpx::~CFileGpx()
{
if ( m_pVecTrack != NULL )
    delete m_pVecTrack ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief lit un fichier et remplit la trace
void CFileGpx::LireFichier(bool AvecAlt)
{
char TmpChar[1000] = "" ;

// raz precedent
m_TrackName = "" ;

// ouverture fichier
File file = SD.open(m_FileName.c_str());
if ( !file )
    {
    Serial.println("Failed to open file for reading");
    return;
    }

if ( m_pVecTrack == NULL )
    m_pVecTrack = new std::vector<StPoint> ;
else
    m_pVecTrack->clear() ;
m_pVecTrack->reserve( 100 ) ;

// lecture fichier
int ic = 0 ;
while(file.available())
    {
    char c = file.read() ;
    TmpChar[ic++] = c ;
    if ( c == '\n' )
        {
        TmpChar[ic++] = 0 ;
        ic = 0 ;
        TraiteLigne( TmpChar , AvecAlt ) ;
        }
    }
TmpChar[ic++] = 0 ;
TraiteLigne( TmpChar , AvecAlt ) ;

// fermeture fichier
file.close() ;

m_pVecTrack->shrink_to_fit() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul de la loupe de depart et de la distance minimale a un point de
/// la trace.
void CFileGpx::SetDistanceFrom( const StPoint PtCur )
{
m_DistFrom = 500.*500. ;
float DeltaLL = 0 ;

m_Barycenter.m_Lat = 0 ;
m_Barycenter.m_Lon = 0 ;
for ( int i = 0 ; i < m_pVecTrack->size() ; i++ )
    {
    m_Barycenter.m_Lat += (*m_pVecTrack)[i].m_Lat ;
    m_Barycenter.m_Lon += (*m_pVecTrack)[i].m_Lon ;

    float DistFrom = sqrtf( powf(PtCur.m_Lat-(*m_pVecTrack)[i].m_Lat,2.0) + powf(PtCur.m_Lon-(*m_pVecTrack)[i].m_Lon,2.0) ) ;
    if ( m_DistFrom > DistFrom )
        m_DistFrom = DistFrom ;
    }
m_Barycenter.m_Lat /= m_pVecTrack->size() ;
m_Barycenter.m_Lon /= m_pVecTrack->size() ;

// delta de lat/lon
for ( int i = 0 ; i < (*m_pVecTrack).size() ; i++ )
    {
    float Dist = fabsf(m_Barycenter.m_Lat - (*m_pVecTrack)[i].m_Lat) ;
    if ( DeltaLL < Dist )
        DeltaLL = Dist ;
    Dist = fabsf(m_Barycenter.m_Lon - (*m_pVecTrack)[i].m_Lon) ;
    if ( DeltaLL < Dist )
        DeltaLL = Dist ;
    }
m_SlopeMax = 3.*DeltaLL/SLOPE_MAX_DIV ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite une ligne de *.gpx
void CFileGpx::TraiteLigne( char * Ligne , bool AvecAlt )
{
// nom piste
if ( m_TrackName == "" && strstr( Ligne , "<name>" ) != NULL )
    {
    strtok( Ligne , "<>" ) ;
    strtok( NULL , "<>" ) ;
    m_TrackName = strtok( NULL , "<>" ) ;
    }
// point de piste
else if ( strstr( Ligne , "<trkpt " ) != NULL )
    {
    StPoint Pts ;
    strtok( Ligne , "\"" ) ;
    Pts.m_Lat = atof( strtok( NULL , "\"" ) ) ;
    strtok( NULL , "\"" ) ;
    Pts.m_Lon = atof( strtok( NULL , "\"" ) ) ;
    m_pVecTrack->push_back( Pts ) ;
    //Serial.print( Pts.m_Lat ) ;
    //Serial.print( " " ) ;
    //Serial.println( Pts.m_Lon ) ;
    }
// altitude du dernier point
else if ( AvecAlt && strstr( Ligne , "<ele>" ) != NULL && m_pVecTrack->size() )
    {
    StPoint & Pts = (*m_pVecTrack)[m_pVecTrack->size()-1] ;
    strtok( Ligne , "<>" ) ;
    strtok( NULL , "<>" ) ;
    Pts.m_Alt = atof( strtok( NULL , "<>" ) ) ;
    //Serial.println( Pts.m_Alt ) ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la distance et l'altitude restante en metres.
void CFileGpx::GetInfo( const StPoint PtCur , float & AltitudeRest , float & DistanceRest , float & AltitudeFait , float & DistanceFait )
{
// determination du pt proche
int iPtProche = 0 ;
float DistProche = 360. ;
for ( int i = 0 ; i < m_pVecTrack->size() ; i++ )
    {
    float Dist = powf(PtCur.m_Lat-(*m_pVecTrack)[i].m_Lat,2.0) + powf(PtCur.m_Lon-(*m_pVecTrack)[i].m_Lon,2.0) ;
    if ( DistProche > Dist )
        {
        DistProche = Dist ;
        iPtProche = i ;
        }
    }

// init altitude distance restante
DistanceRest = DistanceMetre( PtCur , (*m_pVecTrack)[iPtProche] ) ;
AltitudeRest = (*m_pVecTrack)[iPtProche].m_Alt - PtCur.m_Alt ;
if ( AltitudeRest < 0 )
    AltitudeRest = 0 ;
for ( int i = iPtProche + 1 ; i < m_pVecTrack->size() ; i++ )
    {
    DistanceRest += DistanceMetre( (*m_pVecTrack)[i-1] , (*m_pVecTrack)[i] ) ;
    float AltiPos = (*m_pVecTrack)[i].m_Alt - (*m_pVecTrack)[i-1].m_Alt ;
    if ( AltiPos > 0. )
        AltitudeRest += AltiPos ;
    }

// init altitude distance fait
DistanceFait = DistanceMetre( PtCur , (*m_pVecTrack)[iPtProche] ) ;
AltitudeFait = PtCur.m_Alt - (*m_pVecTrack)[iPtProche].m_Alt ;
if ( AltitudeFait < 0 )
    AltitudeFait = 0 ;
for ( int i = 0 ; i < iPtProche && i < m_pVecTrack->size() ; i++ )
    {
    DistanceFait += DistanceMetre( (*m_pVecTrack)[i+1] , (*m_pVecTrack)[i] ) ;
    float AltiPos = (*m_pVecTrack)[i+1].m_Alt - (*m_pVecTrack)[i].m_Alt ;
    if ( AltiPos > 0. )
        AltitudeFait += AltiPos ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la distance entre points en metres
float CFileGpx::DistanceMetre( StPoint PtA , StPoint PtB ) const
{
float DelLat = PtA.m_Lat-PtB.m_Lat ;
float DelLon = PtA.m_Lon-PtB.m_Lon ;
float DelAlt = PtA.m_Alt-PtB.m_Alt ;
float Dist = powf(DelLat*MilesParDegres*UnMileEnMetres,2.) + powf(DelLon*MilesParDegres*UnMileEnMetres,2.) + powf(DelAlt,2.) ;
return sqrtf(Dist) ;
}
