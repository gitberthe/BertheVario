////////////////////////////////////////////////////////////////////////////////
/// \file CHgt2Agl.h
///
/// \brief
///
/// \date creation     : 01/04/2024
/// \date modification : 03/04/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la hauteur sol par rapport a Wgs84.
short CHgt2Agl::GetGroundZ( float LonDeg , float LatDeg )
{
// construction du nom de fichier
int LatRound = abs((int)LatDeg) ;
int LonRound = abs((int)LonDeg) ;

char NS = (LatDeg>=0) ? 'N' : 'S' ;
char EO = (LonDeg>=0) ? 'E' : 'W' ;

// coin sud ouest du fichier
// le fichier couvre 1° x 1°
char FileName[50] ;
sprintf( FileName , "%s/%c%02d%c%03d.HGT" , REP_HGT_FILE , NS , LatRound , EO , LonRound ) ;

// ouverture fichier
m_file = SD.open(FileName);
if ( !m_file )
    {
    // += 10° autour de la france
    if ( abs( LatDeg - 45 ) < 10 )
        m_ErreurFichier = true ;
    Serial.println("**** Failed to open .hgt file for reading ****");
    Serial.println( FileName ) ;
    return -1 ;
    }

float secondsLat = (LatDeg - LatRound) * NB_SEC_PER_DEG;
float secondsLon = (LonDeg - LonRound) * NB_SEC_PER_DEG;

int y = secondsLat / NB_SEC_PAR_TILE;
int x = secondsLon / NB_SEC_PAR_TILE;

// case d'a cote
short height[4];
height[2] = getTileHeight(x, y);
height[0] = getTileHeight(x, y + 1);
height[3] = getTileHeight(x + 1, y);
height[1] = getTileHeight(x + 1, y + 1);

// fermeture fichier
m_file.close();

float dy = fmodf(secondsLat, NB_SEC_PAR_TILE) / NB_SEC_PAR_TILE;
float dx = fmodf(secondsLon, NB_SEC_PAR_TILE) / NB_SEC_PAR_TILE;

// interpolation
float Alti = height[0] * dy * (1 - dx) +
             height[1] * dy * (dx) +
             height[2] * (1 - dy) * (1 - dx) +
             height[3] * (1 - dy) * dx ;

return Alti ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la hauteur  en metre.
/// \param x [in] : ilon
/// \param y [in] : ilat
short CHgt2Agl::getTileHeight( int x , int y )
{
int offsetX = 2 * (x + (NB_PT_PER_ROW - y - 1) * NB_PT_PER_ROW);

m_file.seek( offsetX ) ;

// lecture 16bits
short Alti ;
char * pChar = (char*)&Alti ;
*(pChar+1) = m_file.read() ;
*pChar     = m_file.read() ;

/* *pChar++ = m_file.read() ;
*pChar++ = m_file.read() ;

Alti = swap(Alti) ;*/

return Alti ;
}
