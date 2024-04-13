////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief Programme de test de lecture de fichier Hgt pour AGL.
///
/// \date creation     : 01/04/2024
/// \date modification : 01/04/2024
///

#include <iostream>

using namespace std;

#include "CHgt2Agl.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
int main(int argc , char *argv[] )
{
// pdd
float PddLat = 45.772396 ;
float PddLon = 2.964061  ;
// vichy
float ViLat = 46.1239268 ;
float ViLon = 3.4203712  ;

// aiguille du midi
float MiLat = 45. + 52./60 + 43./3600. ;
float MiLon = 6. + 53./60 + 14./3600. ;

// aiguille du midi
float MbLat = 45.878723 ;
float MbLon = 6.887614 ;

// aiguille du midi
float PtLat = 45.885989 ;
float PtLon = 2.727933 ;

// corent
float CoLat = 45.660355 ;
float CoLon = 3.179634 ;

// zp sancy
float ZpLat = 45.491927 ;
float ZpLon = 2.740171 ;

CHgt2Agl Hgt2Agl ;
int PddAlti = Hgt2Agl.GetGroundZ( PddLon , PddLat ) ;
int ViAlti = Hgt2Agl.GetGroundZ( ViLon , ViLat ) ;
int MiAlti = Hgt2Agl.GetGroundZ( MiLon , MiLat ) ;
int MbAlti = Hgt2Agl.GetGroundZ( MbLon , MbLat ) ;
int PtAlti = Hgt2Agl.GetGroundZ( PtLon , PtLat ) ;
int CoAlti = Hgt2Agl.GetGroundZ( CoLon , CoLat ) ;
int ZpAlti = Hgt2Agl.GetGroundZ( ZpLon , ZpLat ) ;

cout << "pdd(1465) alti:"   << PddAlti << endl;
cout << "vichy() alti:" << ViAlti << endl;
cout << "midi(3842) alti:"  << MiAlti << endl;
cout << "mb(4809) alti:"  << MbAlti << endl;
cout << "pt(686) alti:"  << PtAlti << endl;
cout << "co(616) alti:"  << CoAlti << endl;
cout << "Zp(1141) alti:"  << ZpAlti << endl;
return 0;
}
