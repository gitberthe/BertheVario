////////////////////////////////////////////////////////////////////////////////
/// \file CDisPoints.cpp
///
/// \brief
///
/// \date creation     : 12/08/2024
/// \date modification : 13/08/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
CDisPoints::~CDisPoints()
{
if ( m_PosArr != NULL )
    delete [] m_PosArr ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief On empile une position Gps si vitesse suffisante.
void CDisPoints::PusGpsPos4Disp()
{
struct st_pos pos ;
pos.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
pos.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon ;

// si vitesse insuffisante
if ( g_GlobalVar.m_VitesseKmh < g_GlobalVar.m_Config.m_vitesse_igc_kmh )
    return ;

// si reconfig fichier config
if ( m_TaillePile != g_GlobalVar.m_Config.m_temps_igc_sec )
    {
    delete [] m_PosArr ;
    m_PosArr = NULL ;
    m_ipile = 0 ;
    m_pile_full = false ;
    }

// allocation tableau
if ( m_PosArr == NULL )
    {
    m_TaillePile = g_GlobalVar.m_Config.m_temps_igc_sec ;
    m_PosArr = new st_pos[m_TaillePile] ;
    }


// empilement position
m_PosArr[ m_ipile++ ] = pos ;

// rebouclage position cpurante
if ( m_ipile >= m_TaillePile )
    {
    m_ipile = 0 ;
    m_pile_full = true ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Raz de la pile lors de reboot
void CDisPoints::ResetXY()
{
m_ipile = 0 ;
m_pile_full = false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Indique si on est en vol pour declenchement debut de vol XY. Suivant
/// l'importance de la moyennes des dispersion des points autour du barycentre.
bool CDisPoints::IsInFlight() const
{
if ( !m_pile_full )
    return false ;

// calcul barycentre
struct st_pos pos_bary ;
pos_bary.m_Lat = 0 ;
pos_bary.m_Lon = 0 ;
for ( int ip = 0 ; ip < m_TaillePile ; ip++ )
    {
    pos_bary.m_Lat += m_PosArr[ip].m_Lat ;
    pos_bary.m_Lon += m_PosArr[ip].m_Lon ;
    }
pos_bary.m_Lat /= m_TaillePile ;
pos_bary.m_Lon /= m_TaillePile ;

// calcul moyenne dispersion
float Disp = 0 ;
for ( int ip = 0 ; ip < m_TaillePile ; ip++ )
    Disp += sqrtf( powf( g_GlobalVar.m_TerrainPosCur.m_Lat - pos_bary.m_Lat , 2. ) +
                   powf( g_GlobalVar.m_TerrainPosCur.m_Lon - pos_bary.m_Lon , 2. ) ) ;
Disp /= m_TaillePile ;
Disp *= 60. * UnMileEnMetres ;

// si distance moyenne des points au barycentre
if ( Disp >= g_GlobalVar.m_Config.m_dispersion_igc_metres )
    return true ;

return false ;
}
