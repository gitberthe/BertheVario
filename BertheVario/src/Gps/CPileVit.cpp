////////////////////////////////////////////////////////////////////////////////
/// \file CPileVit.cpp
///
/// \brief
///
/// \date creation     : 02/09/2024
/// \date modification : 02/09/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
CPileVit::~CPileVit()
{
if ( m_PosArr != NULL )
    delete [] m_PosArr ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief On empile une vitesse Gps. Sauf si changement de satellites alors
/// tempo de 10.
void CPileVit::PusGpsVit()
{

// cas de changement de nombre de gps, vitesse fausse
if ( m_ipile < 0 )
    {
    m_pile_full = false ;
    m_ipile++ ;
    return ;
    }

// si vitesse insuffisante ou trop grande
if ( g_GlobalVar.m_VitesseKmh < g_GlobalVar.m_Config.m_vitesse_igc_kmh ||
     g_GlobalVar.m_VitesseKmh > 60. )
    {
    m_pile_full = false ;
    m_ipile = 0 ;
    return ;
    }

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
    m_PosArr = new float[m_TaillePile] ;
    }

// empilement position
m_PosArr[ m_ipile++ ] = g_GlobalVar.m_VitesseKmh ;

// rebouclage position cpurante
if ( m_ipile >= m_TaillePile )
    {
    m_ipile = 0 ;
    m_pile_full = true ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Raz de la pile lors de reboot
void CPileVit::ResetVit()
{
m_ipile = 0 ;
m_pile_full = false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Configuration non memorisation vitesse si satellites modifiés.
void CPileVit::SatChange()
{
m_ipile = -7 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Indique si on est en vol pour declenchement debut de vol vitesse.
/// Si la moyenne des vitesse est assez haute
bool CPileVit::IsInFlight() const
{
//g_GlobalVar.m_Config.m_vitesse_igc_kmh = 1.5 ;

if ( !m_pile_full )
    return false ;

// calcul moyenne
float moyenne = 0. ;
for ( int ip = 0 ; ip < m_TaillePile ; ip++ )
    moyenne += m_PosArr[ip] ;
moyenne /= m_TaillePile ;

if ( moyenne >= g_GlobalVar.m_Config.m_vitesse_igc_kmh )
    return true ;

return false ;
}
