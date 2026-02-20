////////////////////////////////////////////////////////////////////////////////
/// \file CRectIn.cpp
///
/// \brief
///
/// \date 27/10/2025 : creation
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
/// \brief Donne la
float CRectIn::GetDistanceMaxDegres( const CZoneAer::st_coord_poly * PtsArr , int NbPts , const CZoneAer::st_coord_poly & BaryCenter ) const
{
float DistMaxLL = -1 ;
for ( int ip = 0 ; ip < NbPts ; ip++ )
    {
    const CZoneAer::st_coord_poly & Pts = PtsArr [ip] ;
    float DistLL = fabsf( BaryCenter.m_Lat - Pts.m_Lat ) ;
    if ( DistLL > DistMaxLL )
        DistMaxLL = DistLL ;
    DistLL = fabsf( BaryCenter.m_Lon - Pts.m_Lon ) ;
    if ( DistLL > DistMaxLL )
        DistMaxLL = DistLL ;
    }

return DistMaxLL ; // * MilesParDegres * UnMileEnMetres ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne les bornes min/max
void CRectIn::SetLatLonMinMax( const CZoneAer::st_coord_poly * PtsArr , int NbPts )
{
m_LatMax = -100 ;
m_LatMin =  100 ;
m_LonMax = -360 ;
m_LonMin =  360 ;
for ( int ip = 0 ; ip < NbPts ; ip++ )
    {
    const CZoneAer::st_coord_poly & Pts = PtsArr [ip] ;
    m_LatMin = MIN( Pts.m_Lat , m_LatMin ) ;
    m_LonMin = MIN( Pts.m_Lon , m_LonMin ) ;
    m_LatMax = MAX( Pts.m_Lat , m_LatMax ) ;
    m_LonMax = MAX( Pts.m_Lon , m_LonMax ) ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Determine si l'on est dans le rectangle
bool CRectIn::IsInRect( const st_coord_poly & PtsVol ) const
{
if ( PtsVol.m_Lat >= m_LatMin &&
     PtsVol.m_Lat <= m_LatMax &&
     PtsVol.m_Lon <= m_LonMax &&
     PtsVol.m_Lon >= m_LonMin )
    return true ;
return false ;
}
