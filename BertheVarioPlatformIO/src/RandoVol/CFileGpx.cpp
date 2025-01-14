////////////////////////////////////////////////////////////////////////////////
/// \file CFileGpx.cpp
///
/// \brief
///
/// \date creation     : 14/01/2025
/// \date modification : 14/01/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CFileGpx::LireFichier()
{
char TmpChar[1000] = "" ;
//m_TrackName = m_FileName ;

m_VecTrack.clear() ;

// ouverture fichier
File file = SD.open(m_FileName.c_str());
if ( !file )
    {
    Serial.println("Failed to open file for reading");
    return;
    }

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
        TraiteLigne( TmpChar ) ;
        }
    }
TmpChar[ic++] = 0 ;
TraiteLigne( TmpChar ) ;

// fermeture fichier
file.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul de la loupe de depart et de la distance minimale a un point de
/// la trace.
void CFileGpx::SetDistanceFrom( const StPoint PtCur )
{
m_DistFrom = 500. ;
float DeltaLL = 0 ;

m_Barycenter.m_Lat = 0 ;
m_Barycenter.m_Lon = 0 ;
for ( int i = 0 ; i < m_VecTrack.size() ; i++ )
    {
    m_Barycenter.m_Lat += m_VecTrack[i].m_Lat ;
    m_Barycenter.m_Lon += m_VecTrack[i].m_Lon ;

    float DistFrom = sqrtf( powf(PtCur.m_Lat-m_VecTrack[i].m_Lat,2.0) + powf(PtCur.m_Lon-m_VecTrack[i].m_Lon,2.0) ) ;
    if ( m_DistFrom > DistFrom )
        m_DistFrom = DistFrom ;
    }
m_Barycenter.m_Lat /= m_VecTrack.size() ;
m_Barycenter.m_Lon /= m_VecTrack.size() ;

// delta de lat/lon
for ( int i = 0 ; i < m_VecTrack.size() ; i++ )
    {
    if ( DeltaLL < fabsf(m_Barycenter.m_Lat - m_VecTrack[i].m_Lat ) )
        DeltaLL = fabsf(m_Barycenter.m_Lat - m_VecTrack[i].m_Lat ) ;
    if ( DeltaLL < fabsf(m_Barycenter.m_Lon - m_VecTrack[i].m_Lon ) )
        DeltaLL = fabsf(m_Barycenter.m_Lon - m_VecTrack[i].m_Lon ) ;
    }
m_SlopeMax = DeltaLL/200./2. ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CFileGpx::TraiteLigne( char * Ligne )
{
StPoint Pts ;
// nom piste
if ( strstr( Ligne , "<name>" ) != NULL )
    {
    strtok( Ligne , "<>" ) ;
    strtok( NULL , "<>" ) ;
    m_TrackName = strtok( NULL , "<>" ) ;
    }
// point de piste
else if ( strstr( Ligne , "<trkpt " ) != NULL )
    {
    strtok( Ligne , "\"" ) ;
    Pts.m_Lat = atof( strtok( NULL , "\"" ) ) ;
    strtok( NULL , "\"" ) ;
    Pts.m_Lon = atof( strtok( NULL , "\"" ) ) ;
    m_VecTrack.push_back( Pts ) ;
    Serial.print( Pts.m_Lat ) ;
    Serial.print( " " ) ;
    Serial.println( Pts.m_Lon ) ;
    }
}
