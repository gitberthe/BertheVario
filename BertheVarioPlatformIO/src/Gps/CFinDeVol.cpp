////////////////////////////////////////////////////////////////////////////////
/// \file CFinDeVol.h
///
/// \brief
///
/// \date creation     : 24/08/2024
/// \date modification : 25/03/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour redemmarrage de fichier IGC.
void CFinDeVol::InitFinDeVol()
{
m_ADejaEteLointOuHaut = false ;
m_ipile = 0 ;
m_pile_full = false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Determine si le vol est finit. Par petite vitesse verticale et
/// horizontale et distance XY / Altitude.
bool CFinDeVol::IsFlightEnd()
{
// si pile pas pleine
if ( !m_pile_full)
    return false ;

// si au dessus du deco
float AltiDessusDeco = g_GlobalVar.m_TerrainPosCur.m_AltiBaro - g_GlobalVar.m_TerrainPosDeco.m_AltiBaro ;
if ( AltiDessusDeco > 5. )
    return false ;

// calcul barycentre pile
float BaryAlti = 0 ;
float BaryLon  = 0 ;
float BaryLat  = 0 ;
for ( int ip = 0 ; ip < TAILLE_PILE_FE ; ip++ )
    {
    BaryAlti += m_PosArr[ip].m_Alti ;
    BaryLon += m_PosArr[ip].m_Lon ;
    BaryLat += m_PosArr[ip].m_Lat ;
    }
BaryAlti /= TAILLE_PILE_FE ;
BaryLon  /= TAILLE_PILE_FE ;
BaryLat  /= TAILLE_PILE_FE ;

// calcul stationnarite
float DistAlti = 0 ;
float DistXY = 0 ;
for ( int ip = 0 ; ip < TAILLE_PILE_FE ; ip++ )
    {
    DistAlti += fabsf( BaryAlti - m_PosArr[ip].m_Alti ) ;
    DistXY += GetDistance( BaryLon - m_PosArr[ip].m_Lon , BaryLat - m_PosArr[ip].m_Lat ) ;
    }
DistAlti /= TAILLE_PILE_FE ;
DistXY   /= TAILLE_PILE_FE ;

// calcul petite vitesse horizontale
bool VitesseHorPetite = true ;
for ( int ip = 0 ; ip < TAILLE_PILE_FE ; ip++ )
    if ( m_PosArr[ip].m_VitHor > VITESSE_FAIBLE )
        {
        VitesseHorPetite = false ;
        break ;
        }

// calcul petite vitesse verticale
bool VitesseVerPetite = true ;
for ( int ip = 0 ; ip < TAILLE_PILE_FE ; ip++ )
    if ( fabsf(m_PosArr[ip].m_VitVer) > g_GlobalVar.m_Config.m_vz_igc_ms )
        {
        VitesseVerPetite = false ;
        break ;
        }

// si petite distance, altitude, vitesse
if ( DistXY < 30. && DistAlti < 3. && VitesseHorPetite && VitesseVerPetite )
    {
    // bip fin de vol
    CGlobalVar::BeepOk() ;
    CGlobalVar::beeper( SOUND_DELAY_ONLY , 200 );
    CGlobalVar::BeepOk() ;
    return true ;
    }

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Si en vol .
bool CFinDeVol::IsInFlight() const
{
if ( g_GlobalVar.m_DureeVolMin >= 0. )
    return true ;

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Si a deja ete haut ou loin.
bool CFinDeVol::IsFlightLocked() const
{
return m_ADejaEteLointOuHaut || (g_GlobalVar.m_VitesseKmh > VITESSE_FAIBLE) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Empile la position/vitesse.
void CFinDeVol::PushPos4FlihgtEnd()
{
// push de la position dans la pile
struct st_pos pos ;
pos.m_Lat  = g_GlobalVar.m_TerrainPosCur.m_Lat ;
pos.m_Lon  = g_GlobalVar.m_TerrainPosCur.m_Lon ;
pos.m_Alti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
pos.m_VitHor = g_GlobalVar.m_VitesseKmh ;
pos.m_VitVer = g_GlobalVar.m_VitVertMS ;

m_PosArr[ m_ipile++ ] = pos ;

if ( m_ipile >= TAILLE_PILE_FE )
    {
    m_ipile = 0 ;
    m_pile_full = true ;
    }

// detarminattion si a deja ete haut ou loin
if ( !m_ADejaEteLointOuHaut )
    {
    // distance au pt de decollage
    float DistanceMetresLat = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lat - g_GlobalVar.m_TerrainPosDeco.m_Lat ) * 60. * UnMileEnMetres ;
    float DistanceMetresLon = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lon - g_GlobalVar.m_TerrainPosDeco.m_Lon ) * 60. * UnMileEnMetres ;
    float DistanceDecoMetres = GetDistance( DistanceMetresLat , DistanceMetresLon ) ;

    float HauteurAltiBaroMetres = fabsf( g_GlobalVar.m_TerrainPosCur.m_AltiBaro - g_GlobalVar.m_TerrainPosDeco.m_AltiBaro ) ;

    // si deja aller loint ou haut on verrouille
    // erreur de 200m constate sur fichier IGC
    if ( DistanceDecoMetres > 400 || HauteurAltiBaroMetres > 3 )
        m_ADejaEteLointOuHaut = true ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Donne la distance 3D cartesienne
float CFinDeVol::GetDistance( float x , float y )
{
return sqrtf( powf(x,2) + powf(y,2) ) ;
}
