////////////////////////////////////////////////////////////////////////////////
/// \file CStabGps.cpp
///
/// \brief
///
/// \date creation     : 03/04/2024
/// \date modification : 06/09/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
CStabGps::~CStabGps()
{
if ( m_PosArr != NULL )
    {
    delete [] m_PosArr ;
    m_PosArr = NULL ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour une relance Fichier Igc sans reboot
void CStabGps::RazGpsPos()
{
m_ipile = 0 ;
m_pile_full = false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief On empile une position Gps.
void CStabGps::PushGpPos4Stab()
{
struct st_pos pos ;
pos.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
pos.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon ;

// si reconfig fichier config
if ( m_TaillePile != g_GlobalVar.m_Config.m_stab_gps_sec )
    {
    if ( m_PosArr != NULL )
        delete [] m_PosArr ;
    m_PosArr = NULL ;
    m_ipile = 0 ;
    m_pile_full = false ;
    }

// allocation tableau
if ( m_PosArr == NULL )
    {
    m_TaillePile = g_GlobalVar.m_Config.m_stab_gps_sec ;
    m_PosArr = new st_pos[m_TaillePile] ;
    }

m_PosArr[ m_ipile++ ] = pos ;

if ( m_ipile >= m_TaillePile )
    {
    m_ipile = 0 ;
    m_pile_full = true ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie true la la position est dans un rayon stable.
bool CStabGps::IsGpsStable() const
{
if ( !m_pile_full )
    return false ;

// calcul de la position moyenne
struct st_pos pos_moy ;    // position moyenne

for ( int ip = 0 ; ip < m_TaillePile ; ip++ )
    {
    pos_moy.m_Lat += m_PosArr[ip].m_Lat ;
    pos_moy.m_Lon += m_PosArr[ip].m_Lon ;
    }
pos_moy.m_Lat /= m_TaillePile ;
pos_moy.m_Lon /= m_TaillePile ;

// calcul des ecarts de position
for ( int ip = 0 ; ip < m_TaillePile ; ip++ )
    {
    float DeltaLat = m_PosArr[ip].m_Lat - pos_moy.m_Lat ;
    float DeltaLon = m_PosArr[ip].m_Lon - pos_moy.m_Lon ;
    float Dist = sqrtf( powf(DeltaLat,2) + powf(DeltaLon,2) ) * 60 * UnMileEnMetres ;

    if ( Dist > g_GlobalVar.m_Config.m_stab_gps_metre )
        return false ;
    }

return true ;
}

