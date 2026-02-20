////////////////////////////////////////////////////////////////////////////////
/// \file CHgt2Agl.h
///
/// \brief
///
/// \date creation   : 01/04/2024
/// \date 15/01/2026 : correction bug lat ouest et cache map
///

#ifndef _HGT2AGL_
#define _HGT2AGL_

#include <stdio.h>
#include <math.h>
#include <iostream>

#define REP_HGT_FILE "/config/hgtdata"
#define REP_CHK_FILE "/config/hgtdata/chks"

#define NB_PT_PER_ROW   1201
#define NB_SEC_PER_DEG  3600
#define NB_SEC_PAR_TILE 3

#define MAX_ELEMENT_ALTI (35*35)  // soit 3.2km/3.2km (reste 100k free mem)

#define NB_SEC_FOR_SORT    (10)
#define DIST_METRE_4_CACHE (3500)

////////////////////////////////////////////////////////////////////////////////
/// \brief Une case de 92m*92m de cache altitude sol par pas de 3sec
class CCacheAlti
{
public :
    bool    IsEqual( short Lon , short Lat , short x , short y ) const ;
    void    Set( short Lon , short Lat , short x , short y , short alti ) ;
    short   GetAlti() const
                { return m_Alti ; } ;

    float   GetDistanceCarrePosCur() const ;

    bool operator > ( const CCacheAlti & Other ) const ;
    bool operator < ( const CCacheAlti & Other ) const ;

private :

    short   m_Lon ;  ///< longitude du coin SO du fichier
    short   m_Lat ;  ///< latitude du coin SO du fichier
    short   m_x ;    ///< x longitude de 3sec
    short   m_y ;    ///< y de latitude de 3sec
    short   m_Alti ; ///< altitude cache en metre
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Tableau des pointeur de CCacheAlti
class CCacheAltiArrp : public CLGArrayPtr< CCacheAlti >
{
public :
    CCacheAltiArrp()
        { SetDeleteObjet(true); } ;
    CCacheAlti *    Find( short Lon , short Lat , short x , short y ) ;
    void            SuppressFull() ;

private :
    int  m_NbFindRequest = 0 ;  ///< nombre de requette effectuee. utile pour le tri

    void SortForProximity() ;   ///< tri croissant en distance

} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de lecture de fichier HGT pour avoir l'altitude sol fonction
/// de la position geographique Wgs84.
/// Le nom de fichier correspond au coin SO de la carte.
/// La première altitude figurant dans le fichier HGT est celle de son coin
/// haut-gauche
/// download : https://viewfinderpanoramas.org/Coverage%20map%20viewfinderpanoramas_org3.htm
class CHgt2Agl
{
public :

    bool  m_ErreurFichier = false ;  ///< si erreur fichier hgt

    float GetGroundZ( float LonDeg , float LatDeg ) ;
    int   GetCacheSize() const
            { return m_CacheArrp.GetSize() ; } ;

    static short swap( short a ) ;
    #ifdef TIME_GROUND_Z
     CCacheAltiArrp &  GetCacheAlti()
                            { return m_CacheArrp ; } ;
    #endif

private :
    File            m_file ;        ///< fichier *.hgt
    CCacheAltiArrp  m_CacheArrp ;   ///< cache des fichier *.hgt

    short           m_LonDegShort ;  ///< longitude en degres pour CCacheAltiArrp
    short           m_LatDegShort ;  ///< latitude en degres pour CCacheAltiArrp
    short           getTileHeight( int x , int y ) ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Swap 16 bits gauche/droite par paquet de 8.
inline short CHgt2Agl::swap( short a )
{
short left = a ;
left >>= 8 ;
left &= 0xFF ;

short right = a ;
right <<= 8 ;
right &= 0xFF00 ;

short out = left | right ;

// point non valide
if ( out == -32768 )
    return -1 ;

return out ;
}

#endif
