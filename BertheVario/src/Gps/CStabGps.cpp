////////////////////////////////////////////////////////////////////////////////
/// \file CStabGps.cpp
///
/// \brief
///
/// \date creation     : 03/04/2024
/// \date modification : 13/08/2024
///

#include "../BertheVario.h"

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

m_PosArr[ m_ipile++ ] = pos ;

if ( m_ipile >= TAILLE_PILE )
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

for ( int ip = 0 ; ip < TAILLE_PILE ; ip++ )
    {
    pos_moy.m_Lat += m_PosArr[ip].m_Lat ;
    pos_moy.m_Lon += m_PosArr[ip].m_Lon ;
    }
pos_moy.m_Lat /= TAILLE_PILE ;
pos_moy.m_Lon /= TAILLE_PILE ;

// calcul des ecarts de position
for ( int ip = 0 ; ip < TAILLE_PILE ; ip++ )
    {
    float DeltaLat = m_PosArr[ip].m_Lat - pos_moy.m_Lat ;
    float DeltaLon = m_PosArr[ip].m_Lon - pos_moy.m_Lon ;
    float Dist = sqrtf( powf(DeltaLat,2) + powf(DeltaLon,2) ) * 60 * UnMileEnMetres ;

    if ( Dist > g_GlobalVar.m_Config.m_stab_gps )
        return false ;

    }

return true ;
}

