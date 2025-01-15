////////////////////////////////////////////////////////////////////////////////
/// \file CFileGpx.cpp
///
/// \brief
///
/// \date creation     : 14/01/2025
/// \date modification : 15/01/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief lit un fichier et remplit la trace
void CFileGpx::LireFichier(bool AvecAlt)
{
char TmpChar[1000] = "" ;

// raz precedent
m_VecTrack.clear() ;
m_VecTrack.reserve( 300 ) ;
m_TrackName = "" ;

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
        TraiteLigne( TmpChar , AvecAlt ) ;
        }
    }
TmpChar[ic++] = 0 ;
TraiteLigne( TmpChar , AvecAlt ) ;

// fermeture fichier
file.close() ;

m_VecTrack.shrink_to_fit() ;
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
for ( int i = 0 ; i < m_VecTrack.size() ; i++ )
    {
    m_Barycenter.m_Lat += m_VecTrack[i].m_Lat ;
    m_Barycenter.m_Lon += m_VecTrack[i].m_Lon ;

    float DistFrom = powf(PtCur.m_Lat-m_VecTrack[i].m_Lat,2.0) + powf(PtCur.m_Lon-m_VecTrack[i].m_Lon,2.0) ;
    if ( m_DistFrom > DistFrom )
        m_DistFrom = DistFrom ;
    }
m_Barycenter.m_Lat /= m_VecTrack.size() ;
m_Barycenter.m_Lon /= m_VecTrack.size() ;

// delta de lat/lon
for ( int i = 0 ; i < m_VecTrack.size() ; i++ )
    {
    float Dist = fabsf(m_Barycenter.m_Lat - m_VecTrack[i].m_Lat) ;
    if ( DeltaLL < Dist )
        DeltaLL = Dist ;
    Dist = fabsf(m_Barycenter.m_Lon - m_VecTrack[i].m_Lon) ;
    if ( DeltaLL < Dist )
        DeltaLL = Dist ;
    }
m_SlopeMax = DeltaLL/200./2. ;
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
    m_VecTrack.push_back( Pts ) ;
    //Serial.print( Pts.m_Lat ) ;
    //Serial.print( " " ) ;
    //Serial.println( Pts.m_Lon ) ;
    }
// altitude du dernier point
else if ( AvecAlt && strstr( Ligne , "<ele>" ) != NULL && m_VecTrack.size() )
    {
    StPoint & Pts = m_VecTrack[m_VecTrack.size()-1] ;
    strtok( Ligne , "<>" ) ;
    strtok( NULL , "<>" ) ;
    Pts.m_Alt = atof( strtok( NULL , "<>" ) ) ;
    //Serial.println( Pts.m_Alt ) ;
    }
}
