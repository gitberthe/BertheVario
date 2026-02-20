////////////////////////////////////////////////////////////////////////////////
/// \file CTerrainsConnu.h
///
/// \brief
///
/// \date creation   : 17/03/2024
/// \date 23/10/2025 : fabsf dans CalcFinesse()
///

#ifndef _TERRAINCONNU_
#define _TERRAINCONNU_

#define TERRAIN_FCH         "/config/terconnu.txt"
#define PT_REMARQUABLE_FCH  "/config/ptremarq.txt"
#define FINESSE_IMPOSSIBLE  10000

////////////////////////////////////////////////////////////////////////////////
/// \brief Coordonnees d'un terrain connu
class CLocTerrain
{
public :
    float m_Lat = 0. ;          ///< longitude E+, O- en degres
    float m_Lon = 0. ;          ///< longitude N+, S- en degres
    float m_AltiBaroRec = 0. ;  ///< altitude barometrique en metres recalee par fichier hgt
    CLGString m_Nom = "undefine" ;      ///< nom de terrain 10 caracteres maximum
    float m_Finesse ;                   ///< finesse utilisee pour le tri
    //float m_GisementDeg = -1 ;          ///< gisement en degres
    float m_DistanceMetres = -1 ;       ///< distance en metres

    void CalcFinesse() ;

    bool operator > ( const CLocTerrain & PosPt ) const ;
    bool operator < ( const CLocTerrain & PosPt ) const ;

private :

} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Fichier de coordonnees des terrains connus, donne le terrain avec la
/// plus petite finesse par rapport a la position courante.
class CTerrainsConnu : public CLGArrayPtr<CLocTerrain>
{
public :

    CTerrainsConnu() ;
    ~CTerrainsConnu() ;

    void  LireFichierTerrains( const char * NameFch ) ;
    void  CalcTerrainPlusProche() ;
    const CLocTerrain * GetTerrainProche( float & Finesse ) ;
    const CLocTerrain * GetData(int it) const
            { return (*this)[it] ; } ;
    virtual void  DeleteAll() ;

private :
    float           m_Finesse ;             ///< finesse pour atteindre un terrain
} ;

#endif
