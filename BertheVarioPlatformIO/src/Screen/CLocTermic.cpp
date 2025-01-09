////////////////////////////////////////////////////////////////////////////////
/// \file CLocTermic.cpp
///
/// \brief
///
/// \date creation     : 14/03/2024
/// \date modification : 09/01/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Affiche le distancementre/cap termic/terrain
void CLocTermic::Affiche( float CapDeg , float DistMetres )
{
//g_GlobalVar.DoRect( 0 , 0 , 200 , 25 , false ); // x y w h

// en plein arrier +- 45
if ( fabsf( 180 + CapDeg ) < 45 )
    {
    g_GlobalVar.DoRect( 10 , 0 , 10 , 10 ); // x y w h
    g_GlobalVar.DoRect( 0 , 0 , 10 , 20 ); // x y w h
    g_GlobalVar.DoRect( 180 , 0 , 10 , 10 ); // x y w h
    g_GlobalVar.DoRect( 190 , 0 , 10 , 20 ); // x y w h
    CapDeg = 0. ;
    }
else if ( CapDeg < -90 )
    {
    g_GlobalVar.DoRect( 10 , 0 , 10 , 10 ); // x y w h
    g_GlobalVar.DoRect( 0 , 0 , 10 , 20 ); // x y w h
    }
else if ( CapDeg > 90 )
    {
    g_GlobalVar.DoRect( 180 , 0 , 10 , 10 ); // x y w h
    g_GlobalVar.DoRect( 190 , 0 , 10 , 20 ); // x y w h
    }

// distancemetre
int Distance100m = (int)(DistMetres/100.) ;
if ( Distance100m > 99 )
    Distance100m = 99 ;

char TmpChar[15] ;
//char Char = (g_GlobalVar.m_Config.m_Termic) ? '^' : '.' ;
//sprintf(TmpChar,"%c%2d%c", Char , Distance100m, Char ) ;
sprintf(TmpChar," %2d ", Distance100m ) ;

int x = 100 + 100*CapDeg/90. ;

if ( x > 170 )
    x = 170 ;
if ( x < 25 )
    x = 25 ;

// distancemetre
g_GlobalVar.DoChar( x - 27 , 15 , TmpChar ) ;

}
