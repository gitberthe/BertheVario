////////////////////////////////////////////////////////////////////////////////
/// \file CFinDeVol.h
///
/// \brief
///
/// \date creation   : 24/08/2024
/// \date 22/10/2025 : DISTANCE_LOIN_XY_METRE passe a 25m avec le HDOP
/// \date 23/10/2025 : fabsf distance.
/// \date 06/10/2025 : optimisation de IsFlightEnd().
/// \date 02/01/2026 : stationnarite en metre fonction de m_vitesse_igc_kmh
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
/// \brief Pour redemmarrage de fichier IGC.
CFinDeVol::CFinDeVol()
{
InitFinDeVol() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour redemmarrage de fichier IGC.
void CFinDeVol::InitFinDeVol()
{
//m_ADejaEteLointOuHaut = false ;
m_ipile = 0 ;
m_pile_full = false ;

m_AuDessusDuDeco  = false ;
m_LoinDuSol = false ;
m_FaibleVitesseHor = true ;
m_FaibleVitesseVert = true ;

}

////////////////////////////////////////////////////////////////////////////////
/// \brief Determine si le vol est finit. Par petite vitesse verticale et
/// horizontale et distance XY / Altitude.
bool CFinDeVol::IsFlightEnd()
{

// si au dessus du deco pas de fin de vol
float AltiDessusDeco = g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec - g_GlobalVar.m_TerrainPosDeco.m_AltiBaroRec ;
if ( AltiDessusDeco > HAUTEUR_LOIN_DECO_METRE )
    {
    m_AuDessusDuDeco = true ;
    return false ;
    }
m_AuDessusDuDeco = false ;

// si loin du sol pas de fin de vol
if ( g_GlobalVar.GetHauteurSol() > HAUTEUR_LOIN_SOL_METRE_ATTERO )
    {
    m_LoinDuSol = true ;
    return false ;
    }
m_LoinDuSol = false ;

// si pile pas pleine
if ( !m_pile_full)
    return false ;

// calcul petite vitesse horizontale
for ( int ip = 0 ; ip < TAILLE_PILE_FE ; ip++ )
    if ( m_PosArr[ip].m_VitHor > VITESSE_FAIBLE_HOR_KMH )
        {
        m_FaibleVitesseHor = false ;
        return false ;
        }
m_FaibleVitesseHor = true ;

// calcul petite vitesse verticale config
for ( int ip = 0 ; ip < TAILLE_PILE_FE ; ip++ )
    if ( fabsf(m_PosArr[ip].m_VitVer) > g_GlobalVar.m_Config.m_vz_igc_ms )
        {
        m_FaibleVitesseVert = false ;
        return false ;
        }
m_FaibleVitesseVert = true ;

// calcul barycentre pile
float BaryAlti = 0 ;
float BaryLon  = 0 ;
float BaryLat  = 0 ;
for ( int ip = 0 ; ip < TAILLE_PILE_FE ; ip++ )
    {
    BaryAlti += m_PosArr[ip].m_Alti ;
    BaryLon  += m_PosArr[ip].m_Lon ;
    BaryLat  += m_PosArr[ip].m_Lat ;
    }
BaryAlti /= TAILLE_PILE_FE ;
BaryLon  /= TAILLE_PILE_FE ;
BaryLat  /= TAILLE_PILE_FE ;

// calcul stationnarite XY et Z
float DistAlti = 0 ;
float DistXY = 0 ;
for ( int ip = 0 ; ip < TAILLE_PILE_FE ; ip++ )
    {
    DistAlti += fabsf( BaryAlti - m_PosArr[ip].m_Alti ) ;
    DistXY   += GetDistanceLatLon( BaryLon - m_PosArr[ip].m_Lon , BaryLat - m_PosArr[ip].m_Lat ) ;
    }
DistAlti /= TAILLE_PILE_FE ;
DistXY   /= TAILLE_PILE_FE ;

// si petite distance, altitude, vitesse
const float StationnariteEnMetres = g_GlobalVar.m_Config.m_vitesse_igc_kmh / 3.6 ;
if ( DistXY < StationnariteEnMetres &&
     DistAlti < STATIONNARITE_Z_METRE )
    {
    return true ;
    }

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour la neutralisation des boutons
bool CFinDeVol::IsAtGround() const
{
// atention a la routine d'interruption, pas trop d'appel de fonction

if ( m_AuDessusDuDeco )
    return false ;

if ( m_LoinDuSol )
    return false ;

if ( m_FaibleVitesseHor && m_FaibleVitesseVert )
    return true ;

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief hdop en dessous de HDOP_BON 3.0 sur toute la pile
bool CFinDeVol::IsHDopOk() const
{
for ( int ip = 0 ; ip < TAILLE_PILE_FE ; ip ++ )
    {
    if ( m_PosArr[ m_ipile ].m_HDop > HDOP_BON )
        return false ;
    if ( ip > 0 && (m_PosArr[m_ipile].m_HDop != m_PosArr[m_ipile-1].m_HDop) )
        return false ;
    }
return true ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Si en vol. Minutes de >= 0.
bool CFinDeVol::IsInFlight() const
{
if ( g_GlobalVar.m_DureeVolMin >= 0. )
    return true ;

return false ;
}

/*
////////////////////////////////////////////////////////////////////////////////
/// \brief Si a deja ete haut ou loin pour impossibilite reboot.
bool CFinDeVol::IsFlightLocked() const
{
bool ret = m_ADejaEteLointOuHaut ||
           (g_GlobalVar.m_VitesseKmh > VITESSE_FAIBLE_HOR_KMH) ||
           (g_GlobalVar.GetHauteurSol() > STATIONNARITE_Z_METRE) ;

return ret ;
}
*/

////////////////////////////////////////////////////////////////////////////////
/// \brief Empile la position/vitesse.
void CFinDeVol::PushPos4FlihgtEnd()
{
// push de la position dans la pile
m_PosArr[ m_ipile ].m_Lat  = g_GlobalVar.m_TerrainPosCur.m_Lat ;
m_PosArr[ m_ipile ].m_Lon  = g_GlobalVar.m_TerrainPosCur.m_Lon ;
m_PosArr[ m_ipile ].m_Alti = g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ;
m_PosArr[ m_ipile ].m_VitHor = g_GlobalVar.m_VitesseKmh ;
m_PosArr[ m_ipile ].m_VitVer = g_GlobalVar.m_VitVertMS ;
m_PosArr[ m_ipile ].m_HDop   = CTrame::GetHDop() ;
m_ipile++ ;

// rebouclage pile
if ( m_ipile >= TAILLE_PILE_FE )
    {
    m_ipile = 0 ;
    m_pile_full = true ;
    }

/*
// detarmination si a deja ete haut ou loin
if ( !m_ADejaEteLointOuHaut )
    {
    // distance au pt de decollage
    float DeltaLat = g_GlobalVar.m_TerrainPosCur.m_Lat - g_GlobalVar.m_TerrainPosDeco.m_Lat ;
    float DeltaLon = g_GlobalVar.m_TerrainPosCur.m_Lon - g_GlobalVar.m_TerrainPosDeco.m_Lon ;
    float DistanceDecoMetres = GetDistanceLatLon( DeltaLat , DeltaLon ) ;

    float HauteurAltiBaroDecoMetres = fabsf( g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec - g_GlobalVar.m_TerrainPosDeco.m_AltiBaroRec ) ;

    // si deja aller loint ou haut on verrouille
    if ( DistanceDecoMetres > DISTANCE_LOIN_XY_METRE && IsHDopOk() )
        m_ADejaEteLointOuHaut = true ;

    if ( HauteurAltiBaroDecoMetres > HAUTEUR_LOIN_DECO_METRE )
        m_ADejaEteLointOuHaut = true ;
    }
*/
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Donne la distance metres a partir de la lat/lon
float CFinDeVol::GetDistanceLatLon( float deltalat , float deltalon )
{
float DistanceMetresLat = ( deltalat ) * 60. * UnMileEnMetres ;
float DistanceMetresLon = ( deltalon ) * 60. * UnMileEnMetres ;

return GetDistanceMetres( DistanceMetresLat , DistanceMetresLon ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Donne la distance 3D cartesienne
float CFinDeVol::GetDistanceMetres( float deltax , float deltay )
{
return sqrtf( powf(deltax,2) + powf(deltay,2) ) ;
}
