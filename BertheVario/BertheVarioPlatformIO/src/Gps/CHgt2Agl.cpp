////////////////////////////////////////////////////////////////////////////////
/// \file CHgt2Agl.h
///
/// \brief
///
/// \date creation     : 01/04/2024
/// \date 15/01/2026 : correction bug lat ouest et cache map
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
/// \brief regarde si les lon/lat sont egaux a 3 sec pres.
bool CCacheAlti::IsEqual( short Lon , short Lat , short x , short y ) const
{
if ( m_Lon == Lon && m_Lat == Lat && m_x == x && m_y == y )
    return true ;
return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne lon/lat a 3 sec pres ainsi que l'altitude.
void CCacheAlti::Set( short Lon , short Lat , short x , short y , short alti)
{
m_Lon = Lon ;
m_Lat = Lat ;
m_x = x ;
m_y = y ;
m_Alti = alti ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul la distance au carre en degres par rapport au pt courant.
float CCacheAlti::GetDistanceCarrePosCur() const
{
float Lon = ((float)m_Lon) + (((float)m_x) * ((float)NB_SEC_PAR_TILE)) / NB_SEC_PER_DEG ;
float Lat = ((float)m_Lat) + (((float)m_y) * ((float)NB_SEC_PAR_TILE)) / NB_SEC_PER_DEG ;
float distance =  powf( Lon - g_GlobalVar.m_TerrainPosCur.m_Lon , 2. )
                + powf( Lat - g_GlobalVar.m_TerrainPosCur.m_Lat , 2. ) ;
return distance ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief operateur de tri par distance courante
bool CCacheAlti::operator > ( const CCacheAlti & Other ) const
{
if ( GetDistanceCarrePosCur() > Other.GetDistanceCarrePosCur() )
    return true ;
return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief operateur de tri par distance courante
bool CCacheAlti::operator < ( const CCacheAlti & Other ) const
{
if ( GetDistanceCarrePosCur() < Other.GetDistanceCarrePosCur() )
    return true ;
return false ;
}

/******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// \brief Elimination des distances trop grandes puis tri croissant.
void CCacheAltiArrp::SortForProximity()
{
// distance au carre
float DistanceCarreMax = DIST_METRE_4_CACHE ;
DistanceCarreMax /= UnMileEnMetres * MilesParDegres ;
DistanceCarreMax = powf( DistanceCarreMax , 2. ) ;

// elimination des distances trop grandes
for ( int ic = 0 ; ic < GetSize() ; ic++ )
    if ( ElementAt(ic)->GetDistanceCarrePosCur() > DistanceCarreMax )
        {
        delete ElementAt(ic) ;
        ElementAt(ic) = NULL ;
        #ifdef GROUND_Z_DEBUG
         Serial.println( "SortForProximity NULL" ) ;
        #endif
        }
CompressNull() ;

// tri croissant pour recharche rapide
TriAdapte(true) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Cherche un element dans le cache dont les lon/lat sont egaux
/// a 3 sec pres
CCacheAlti * CCacheAltiArrp::Find( short Lon , short Lat , short x , short y )
{
// pour tri periodique
m_NbFindRequest++ ;

// toutes les 10 secondes un tri (4 tiles par secendes)
if ( ! (m_NbFindRequest%(4*NB_SEC_FOR_SORT)) )
    SortForProximity() ;

// recherche du cache le plus proche
for ( int ic = 0 ; ic < GetSize() ; ic++ )
    if ( GetAt(ic)->IsEqual( Lon , Lat , x , y ) )
        return ElementAt(ic) ;

return NULL ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Suppression des plus loint elements. Si plein on vide de 10%
/// tableau trie au paravent
void CCacheAltiArrp::SuppressFull()
{
int DebSupp = (int)(MAX_ELEMENT_ALTI * 0.9) ;
for ( int ic = DebSupp ; ic < GetSize() ; ic++ )
    {
    delete ElementAt(ic) ;
    ElementAt(ic) = NULL ;
    }

CompressNull() ;
}

/******************************************************************************/

/**
 * Performs bilinear interpolation for a given point (x, y)
 * inside a 2D grid of values.
 *
 * @param q11 Value at (x1, y1)
 * @param q21 Value at (x2, y1)
 * @param q12 Value at (x1, y2)
 * @param q22 Value at (x2, y2)
 * @param x1, y1 Coordinates of q11
 * @param x2, y2 Coordinates of q22
 * @param x, y   Target coordinates
 * @return Interpolated value
 */
float bilinear_interpolate(
    float q11, float q21, float q12, float q22,
    //float x1, double y1, double x2, double y2,
    float x, float y
) {
    // Prevent division by zero
    //if (fabs(x2 - x1) < 1e-12 || fabs(y2 - y1) < 1e-12)
    //{
    //    fprintf(stderr, "Error: Invalid interpolation bounds.\n");
    //    exit(EXIT_FAILURE);
    //return -1 ;
    //}

    //double r1 = ((x2 - x) / (x2 - x1)) * q11 + ((x - x1) / (x2 - x1)) * q21;
    //double r2 = ((x2 - x) / (x2 - x1)) * q12 + ((x - x1) / (x2 - x1)) * q22;
    //return ((y2 - y) / (y2 - y1)) * r1 + ((y - y1) / (y2 - y1)) * r2;
float r1 = ((1. - x) / (1. - 0.)) * q11 + ((x - 0.) / (1. - 0.)) * q21;
float r2 = ((1. - x) / (1. - 0.)) * q12 + ((x - 0.) / (1. - 0.)) * q22;
return     ((1. - y) / (1. - 0.)) *  r1 + ((y - 0.) / (1. - 0.)) *  r2;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la hauteur sol par rapport a Wgs84.
float CHgt2Agl::GetGroundZ( float LonDeg , float LatDeg )
{
#ifdef TIME_GROUND_Z
unsigned long time_gz = millis() ;
#endif // TIME_ZONE

// memorisation pour le cache (independement du fichier)
// il n'y a pas de *W000.hgt (coin bas gauche SO du fichier)
m_LonDegShort = floorf(LonDeg) ;
m_LatDegShort = floorf(LatDeg) ;

// si trop d'element dans le cache
if ( m_CacheArrp.GetSize() >= MAX_ELEMENT_ALTI )
    m_CacheArrp.SuppressFull() ;

// si map pleine, ou pas assez de memoire raz du cache
if ( ! CZonesAerAll::IsEnoughMemory() )
    m_CacheArrp.DeleteAll() ;

// construction du nom de fichier
int LatRound = abs(m_LatDegShort) ;
int LonRound = abs(m_LonDegShort) ;

char NS = (LatDeg>=0) ? 'N' : 'S' ;
char EO = (LonDeg>=0) ? 'E' : 'W' ;

// coin sud ouest du fichier
// le fichier couvre 1° x 1°
char FileName[50] ;
sprintf( FileName , "%s/%c%02d%c%03d.HGT" , REP_HGT_FILE , NS , LatRound , EO , LonRound ) ;

float secondsLat = (LatDeg - m_LatDegShort) * NB_SEC_PER_DEG;
float secondsLon = (LonDeg - m_LonDegShort) * NB_SEC_PER_DEG;

// X coresponds to x/y values,
// everything easter/norhter (< S) is rounded to X.
//
//  y   ^
//  3   |       |   S
//      +-------+-------
//  0   |   X   |
//      +-------+-------->
// (sec)    0        3   x  (lon)
int y = secondsLat / NB_SEC_PAR_TILE;
int x = secondsLon / NB_SEC_PAR_TILE;

// ouverture fichier
#ifdef _BERTHE_VARIO_TAC_
 g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
#endif // _BERTHE_VARIO_TAC_
m_file = SD.open(FileName);
if ( !m_file )
    {
    // += 10° autour de la france
    if ( abs( LatDeg - 45 ) < 10 )
        m_ErreurFichier = true ;
    Serial.println("**** Failed to open .hgt file for reading ****");
    Serial.println( FileName ) ;
    #ifdef _BERTHE_VARIO_TAC_
     g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
    #endif
    return -1 ;
    }

// case d'a cote
short height[4];
height[2] = getTileHeight(x, y);
height[0] = getTileHeight(x, y + 1);
height[3] = getTileHeight(x + 1, y);
height[1] = getTileHeight(x + 1, y + 1);

// fermeture fichier
m_file.close();
#ifdef _BERTHE_VARIO_TAC_
 g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
#endif

// ratio where X lays
float dy = fmodf(secondsLat, NB_SEC_PAR_TILE) / NB_SEC_PAR_TILE;
float dx = fmodf(secondsLon, NB_SEC_PAR_TILE) / NB_SEC_PAR_TILE;

// Bilinear interpolation
// h0------------h1
// |
// |--dx-- .
// |       |
// |      dy
// |       |
// h2------------h3
float Alti = height[0] * dy * (1. - dx) +
             height[1] * dy * (dx) +
             height[2] * (1. - dy) * (1. - dx) +
             height[3] * (1. - dy) * dx ;


#ifdef TIME_GROUND_Z
unsigned long ecart = millis() - time_gz ;
Serial.print( "time calc hgt ms : " ) ;
Serial.println( ecart ) ;
#endif

return Alti ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la hauteur  en metre.
/// Le nom de fichier correspond au coin SO de la carte.
/// La première altitude figurant dans le fichier HGT est celle de son coin
/// haut-gauche
/// \param x [in] : ilon
/// \param y [in] : ilat
short CHgt2Agl::getTileHeight( int x , int y )
{
#ifdef TIME_GROUND_Z
float Remplissage = 100. * (float)m_CacheArrp.GetSize() / (float) MAX_ELEMENT_ALTI ;
char TmpChar[50] ;
sprintf( TmpChar , "remplissage cache : %dpour %d" , (int)Remplissage , m_CacheArrp.GetSize() ) ;
Serial.println( TmpChar ) ;
Serial.print( "mem : ") ;
Serial.println ( esp_get_free_heap_size() ) ;
#endif

// altitude retournee si trouvee dans le cache
CCacheAlti * pCache = m_CacheArrp.Find( m_LonDegShort , m_LatDegShort , x , y ) ;
if ( pCache != NULL )
    {
    #ifdef GROUND_Z_DEBUG
     Serial.print( "CFound " ) ;
    #endif
    return pCache->GetAlti() ;
    }
else
    {
    #ifdef GROUND_Z_DEBUG
     Serial.print( "NCache " ) ;
    #endif
    }

// X coresponds to x/y values,
// everything easter/norhter (< S) is rounded to X.
//
//  y   ^
//  3   |       |   S
//      +-------+-------
//  0   |   X   |
//      +-------+-------->
// (sec)    0        3   x  (lon)

// 2 octets pour un short
int offsetX = 2 * (x + (NB_PT_PER_ROW - y - 1) * NB_PT_PER_ROW) ;

m_file.seek( offsetX ) ;

// lecture 16bits
short Alti ;
/*char * pChar = (char*)&Alti ;
*(pChar+1) = m_file.read() ;
*pChar     = m_file.read() ;*/

/* *pChar++ = m_file.read() ;
*pChar++ = m_file.read() ;

Alti = swap(Alti) ;*/

// lecture fichier en 1 seul passage
m_file.read( (uint8_t *) &Alti , sizeof(Alti) ) ;
Alti = swap(Alti) ;

// stocke l'altitude dans le cache
pCache = new CCacheAlti ;
pCache->Set( m_LonDegShort , m_LatDegShort , x , y , Alti ) ;
m_CacheArrp.Add( pCache ) ;

return Alti ;
}
