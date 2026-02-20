////////////////////////////////////////////////////////////////////////////////
/// \file CHgt2Agl.h
///
/// \brief
///
/// \date creation   : 01/04/2024
/// \date 16/01/2025 :
///

#include "CHgt2Agl.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la hauteur sol par rapport a Wgs84.
short CHgt2Agl::GetGroundZ( float LonDeg , float LatDeg )
{
// construction du nom de fichier
int LatRound = abs((int)LatDeg) ;
int LonRound = abs((int)LonDeg) ;

char NS = (LatDeg>0) ? 'N' : 'S' ;
char EO = (LonDeg>0) ? 'E' : 'W' ;

// coin sud ouest du fichier
// le fichier couvre 1° x 1°
char FileName[50] ;
sprintf( FileName , "%s/%c%02d%c%03d.HGT" , REP_HGT_FILE , NS , LatRound , EO , LonRound ) ;

FILE * File ;

File = fopen( FileName , "r" ) ;
if ( File == NULL )
    {
    std::cout << "probleme ouverture fichier " << FileName << std::endl ;
    return -1 ;
    }

// 1200 * 3 secondes = 3600 secondes = 60minutes = 1deg
const short SizeOfRecord = 1201 ; // 1201*1201*2 = 2884802 octets
const short ResolFchSecondes = 3 ;

// determination de iLon et iLat
float NbSecondesLon = (LonDeg - floorf(LonDeg)) * 3600 ;
float NbSecondesLat = (LatDeg - floorf(LatDeg)) * 3600 ;
short iLon = (short) (NbSecondesLon/ResolFchSecondes) ;
short iLat = SizeOfRecord - (short) (NbSecondesLat/ResolFchSecondes) - 1 ;

// deplacement dans le fichier
fseek( File , 2 * (iLat * SizeOfRecord + iLon ) , SEEK_SET ) ;

// lectrue 16bits
short Alti ;
fread( &Alti , sizeof(Alti) , 1 , File ) ;

// swap des bits
Alti = swap(Alti) ;

// fermeture fichier
fclose( File ) ;

return Alti ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Swap 16 bits gauche/droite par paquet de 8.
short CHgt2Agl::swap( short a )
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
