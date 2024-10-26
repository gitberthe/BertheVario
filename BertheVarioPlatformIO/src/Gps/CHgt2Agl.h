////////////////////////////////////////////////////////////////////////////////
/// \file CHgt2Agl.h
///
/// \brief
///
/// \date creation     : 01/04/2024
/// \date modification : 03/04/2024
///

#ifndef _HGT2AGL_
#define _HGT2AGL_

#include <stdio.h>
#include <math.h>
#include <iostream>

#define REP_HGT_FILE "/config/hgtdata"

#define NB_PT_PER_ROW   1201
#define NB_SEC_PER_DEG  3600
#define NB_SEC_PAR_TILE 3

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de lecture de fichier HGT pour avoir l'altitude sol fonction
/// de la position geographique Wgs84.
class CHgt2Agl
{
public :

    bool  m_ErreurFichier = false ;  ///< si erreur fichier hgt

    short GetGroundZ( float LonDeg , float LatDeg ) ;

private :
    File m_file ;

    short swap( short a ) ;
    short getTileHeight( int x , int y ) ;
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
