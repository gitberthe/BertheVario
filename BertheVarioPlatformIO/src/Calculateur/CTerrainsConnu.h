////////////////////////////////////////////////////////////////////////////////
/// \file CTerrainsConnu.h
///
/// \brief
///
/// \date creation     : 17/03/2024
/// \date modification : 01/04/2024
///

#ifndef _TERRAINCONNU_
#define _TERRAINCONNU_

#define TERRAIN_FCH         "/config/terconnu.txt"
#define FINESSE_IMPOSSIBLE  10000

////////////////////////////////////////////////////////////////////////////////
/// \brief Coordonnees d'un terrain connu
class CLocTerrain
{
public :
    float m_Lat = 0. ;      ///< longitude E+, O- en degres
    float m_Lon = 0. ;      ///< longitude N+, S- en degres
    float m_AltiBaro = 0. ; ///< altitude en metres
    std::string m_Nom = "undefine" ;    ///< nom de terrain 10 caracteres maximum
    float m_Finesse ;                   ///< finesse utilisee pour le tri
    float m_GisementDeg = -1 ;          ///< gisement en degres
    float m_DistanceMetres = -1 ;       ///< distance en metres

    void CalcFinesse() ;

    bool operator > ( const CLocTerrain & PosPt ) const ;
    bool operator < ( const CLocTerrain & PosPt ) const ;

private :

} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Fichier de coordonnees des terrains connus, donne le terrain avec la
/// plus petite finesse par rapport a la position courante.
class CTerrainsConnu : public CSortArray<CLocTerrain>
{
public :

    void  LireFichierTerrains() ;
    void  CalcTerrainPlusProche() ;
    const CLocTerrain * GetTerrainProche( float & Finesse ) ;

private :
    CLocTerrain **  m_pTerrainsArr = NULL ; ///< tableau des terrains + le decollage
    float           m_Finesse ;             ///< finesse pour atteindre un terrain
} ;

#endif
