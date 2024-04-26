////////////////////////////////////////////////////////////////////////////////
/// \file CFauxDepart.cpp
///
/// \brief
///
/// \date creation     : 18/03/2024
/// \date modification : 26/04/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction chargée de detecter un faux depart de vol.
/// Si moins de +-30m et 50cm pendant 1min alors faux depart sauf si deja allé a plus de 300m
/// du deco ou a plus de +-3m alti baro.
/// Vitesse petite necessaire pour reboot.
/// \return true si l'on considere que c'est un faux vol.
bool CFauxDepart::IsBadFlightBegin()
{
// si on a deja ete loint ou haut, pas de faux vol
if ( m_ADejaEteLointOuHaut )
    return false ;

// distance et hauteur baro au pt de decollage
float DistanceDecoMetres = GetDistanceDeco() ;
float HauteurAltiBaroMetres = fabsf( g_GlobalVar.m_TerrainPosCur.m_AltiBaro - g_GlobalVar.m_TerrainPosDeco.m_AltiBaro ) ;

// si deja aller loint ou haut on verrouille
// erreur de 200m constate sur fichier IGC
if ( DistanceDecoMetres > 300 || HauteurAltiBaroMetres > 3 )
    {
    m_ADejaEteLointOuHaut = true ;
    return false ;
    }

// distance de deplacement
float DistanceMetresLat = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lat - m_LastLat ) * 60. * UnMileEnMetres ;
float DistanceMetresLon = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lon - m_LastLon ) * 60. * UnMileEnMetres ;
float DistanceDeplacementMetres = sqrtf( powf(DistanceMetresLat,2) + powf(DistanceMetresLon,2) ) ;
// stabilise le gps est precis a 2.5m d'apres les specifications mais 30m d'apres fichier IGC en ville
if ( DistanceDeplacementMetres > 30 )
    {
    m_TempsStationnaireLatLon = millis() ;
    m_LastLat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
    m_LastLon = g_GlobalVar.m_TerrainPosCur.m_Lon ;
    }

// ecart d'altitude de plus de 0.5m
if ( fabsf( g_GlobalVar.m_TerrainPosCur.m_AltiBaro - m_LastAlti ) > 0.5 )
    {
    m_TempsStationnaireAlti = millis() ;
    m_LastAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
    }

// temps de stationnarite lat lon
float TempsLatLon = millis() - m_TempsStationnaireLatLon ;
TempsLatLon /= 1000 * 60 ;

// temps de stationnarite alti
float TempsAlti = millis() - m_TempsStationnaireAlti ;
TempsAlti /= 1000 * 60 ;

// si un des temps est depassé laors reboot
float TempsMaxMin = 1. ;
#ifdef REBOOT_DEBUG
 TempsMaxMin = 0.1 ;
#endif
#ifdef TERMIC_DEBUG
 TempsMaxMin = 0.1 ;
#endif
if ( TempsLatLon >= TempsMaxMin || TempsAlti >= TempsMaxMin )
    {
    // si la vitesse est faible
    if ( g_GlobalVar.m_VitesseKmh < g_GlobalVar.m_Config.m_vitesse_igc_kmh )
        return true ;
    }

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation des variables de stationnarite.
void CFauxDepart::InitFauxDepart( float Lat, float Lon, unsigned long Temps)
{
m_LastLat = Lat ;
m_LastLon = Lon ;
m_TempsStationnaireLatLon = Temps ;
m_TempsStationnaireAlti = Temps ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Donne la distance 3D par rapport au deco.
float CFauxDepart::GetDistanceDeco()
{
// distance au pt de decollage
float DistanceMetresLat = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lat - g_GlobalVar.m_TerrainPosDeco.m_Lat ) * 60. * UnMileEnMetres ;
float DistanceMetresLon = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lon - g_GlobalVar.m_TerrainPosDeco.m_Lon ) * 60. * UnMileEnMetres ;
float DistanceMetresAlt = fabsf( g_GlobalVar.m_TerrainPosCur.m_AltiBaro - g_GlobalVar.m_TerrainPosDeco.m_AltiBaro ) ;
return GetDistance( DistanceMetresLat , DistanceMetresLon, DistanceMetresAlt ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Donne la distance 3D cartesienne
float CFauxDepart::GetDistance( float x , float y , float  z )
{
return sqrtf( powf(x,2) + powf(y,2) + powf(z,2) ) ;
}
