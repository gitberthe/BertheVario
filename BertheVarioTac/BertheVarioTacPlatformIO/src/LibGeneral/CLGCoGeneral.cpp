//////////////////////////////////////////////////////////////////////////////
/// \file CoGeneral.cpp
/// \date 15/11/2002 : Creation
///
/// \brief Fichier commun inclus dans les fichiers .h de chaque classe.
///
/// \date 11/11/2002 : compilation de wxWindows avec des MFC.
/// \date 20/12/2002 : Ajout de ListeStrData.
/// \date 30/12/2002 : Passage de fonctions Array en const.
/// \date 11/01/2003 : Ajout de CopyWithoutAlloc() dans CLGArrayPtr.
/// \date 12/04/2003 : Passage des parametres par reference.
/// \date 02/10/2004 : Compilation avec wxWindows 2.4.2.
/// \date 25/09/2005 : Ajout de m_DernierTri dans CLGArrayPtr.
/// \date 09/08/2012 : passage de iso-8859-15 à UTF-8 dans FormatCharIn()
/// \date 08/01/2018 : Modification sans _AVEC_WXWIN_H pour snb-artmap (strlwr...).
/// \date 03/12/2023 : derniere modification.
///

#include "LibGeneral.h"

int  static_Nb_Control_C     = 0 ;
bool static_Control_C_Active = false ;  ///< passe a true lors d'un appui dur controle-c
bool g_Verbose = false ;                ///< a true si option --v sur la ligne de commande

//////////////////////////////////////////////////////////////////////
/// \brief Fonction recopier de github.com. Casse en minuscule.
char *strlwr(char *a)
{
char *p, c;

if ( a == NULL )
    return NULL ;

for (p = a; *p; p++)
	if (isascii(*p)) {
		c = *p;
		if (isupper(c))
			c = tolower(c);
		*p = c;
	}
return a;
}

//////////////////////////////////////////////////////////////////////
/// \brief Fonction recopiée de github.com. Casse en majuscule.
char *strupr(char *a)
{
char *p, c;

if ( a == NULL )
    return NULL ;

for (p = a; *p; p++)
	if (isascii(*p)) {
		c = *p;
		if (islower(c))
			c = toupper(c);
		*p = c;
	}
return a;
}

//////////////////////////////////////////////////////////////////////
/// \brief fonction recopier de github.com.
/// Reverse a string (rotate it about its midpoint).
char * strrev (char	* s)
{
char *start, *end, c1, c2;

start = end = s;

while(*end) end++;
end--;

while(end > start)
    {
    c1 = *start;
	c2 = *end;

	*start++ = c2;
	*end--   = c1;
    }

return s;
}

//////////////////////////////////////////////////////////////////////
/// \brief Sortie du message sur l'ecran.
void MyMessageError( const char * Message )
{

printf( "%s" , Message ) ;
printf("\n");
//printf("\nAppuier sur Entree.\n");
//getchar() ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction permet de creer un texte centre dans une chaine de
/// longueure length
void MakeStringCentered( char * Dest , const char * Texte , int length )
{
if ( length <= (int)strlen( Texte ) )
    strcpy( Dest , Texte ) ;
else
    {
    length = length - strlen( Texte ) ;
    length = length / 2 ;
    CLGString cotes( ' ' , length ) ;
    strcpy( Dest , cotes ) ;
    strcat( Dest , Texte ) ;
    strcat( Dest , cotes ) ;
    }
}

//////////////////////////////////////////////////////////////////////
/// \brief Remplace une chaine par une autre dans st.
/// \param st [in][out] : chaine traite.
void ReplaceStringIn( CLGString & sst, const char *orig, const char *repl , char * buffer , bool strlenparam /* = true */  )
{
//const int TailleBuff = 4096*10 ;
//char buffer[TailleBuff];
//ASSERT( sst.GetLength() < TailleBuff ) ;
char *ch;
// tant que chaine trouve
while ((ch = strstr(sst.GetBuffer(), orig)))
    {
    // copy du debut de la chaine
    strncpy(buffer, sst, ch-(const char*)sst);
    buffer[ch-(const char*)sst] = 0;
    strcat( buffer , repl ) ;
    strcat( buffer , ch+strlen(orig) ) ;
    sst = buffer ;
    }

if ( strlenparam )
    sst.Strlen() ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Permet de remplacer les caracteres à probleme dans une chaine
/// de caracteres.
///
/// \param pchar [in] [out]     : chaine à formater.
/// \param remplace_espace [in] : remplace les espaces par des '_'.
///
/// \return le pointeur de caractere de la chaine passée parametre.
///
const char * FormatCharIn( CLGString & spchar , bool remplace_espace /*= true */ , bool alpha_seul /* = false */ , bool strlen /* true */ )
{
//if ( pchar == NULL || *pchar == '\0' )
//    return pchar ;
if ( spchar.IsEmpty() )
    return spchar ;

// voir https://outils-javascript.aliasdmc.fr/encodage-caracteres-accentues/

const int TailleBuff = 4096*100 ;
char buffer[TailleBuff];
ASSERT( spchar.GetLength() < TailleBuff ) ;

ReplaceStringIn( spchar , "à" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "á" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "â" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "ä" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "ã" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "Ä" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "Å" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "Á" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "ç" , "c" , buffer, false  ) ;
ReplaceStringIn( spchar , "Ç" , "c" , buffer, false  ) ;
ReplaceStringIn( spchar , "Č" , "c" , buffer, false  ) ;
ReplaceStringIn( spchar , "ć" , "c" , buffer, false  ) ;
ReplaceStringIn( spchar , "Đ" , "d" , buffer, false  ) ;
ReplaceStringIn( spchar , "é" , "e" , buffer, false  ) ;  // attention pas meme code char 0xFF
ReplaceStringIn( spchar , "é" , "e" , buffer, false  ) ;  // attention pas meme code char 0xFF
ReplaceStringIn( spchar , "è" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "È" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "É" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "ê" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "ë" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "î" , "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "ï" , "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "í" , "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "ı" , "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "ñ" , "n" , buffer, false  ) ;
ReplaceStringIn( spchar , "ô" , "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "ö" , "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "ó" , "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "ø" , "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "ù" , "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "ü" , "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "š" , "s" , buffer, false  ) ;
ReplaceStringIn( spchar , "ș" , "s" , buffer, false  ) ;
ReplaceStringIn( spchar , "±" , "-" , buffer, false  ) ;
ReplaceStringIn( spchar , "\351" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "\347" , "c" , buffer, false  ) ;
ReplaceStringIn( spchar , "\307" , "c" , buffer, false  ) ;
ReplaceStringIn( spchar , "\350" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "\352" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "\353" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "\313" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "\342" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "\300" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "\340" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "\302" , "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "\222" , " " , buffer, false  ) ;
ReplaceStringIn( spchar , "\310" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "\311" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "\312" , "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "\356" , "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "\357" , "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "\316" , "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "\317" , "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "\364" , "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "\324" , "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "\326" , "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "\366" , "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "\325" , "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "\333" , "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "\334" , "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "\373" , "u" , buffer, false  ) ;
ReplaceStringIn( spchar ,    "û" , "u" , buffer, false  ) ;
ReplaceStringIn( spchar ,    "ú" , "u" , buffer, false  ) ;

ReplaceStringIn( spchar , "%22",    "-" , buffer, false  ) ; // float cote
ReplaceStringIn( spchar , "%20",    " " , buffer, false  ) ; // espace
ReplaceStringIn( spchar , "%27",    " " , buffer, false  ) ; // '

ReplaceStringIn( spchar , "%C3%80", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A0", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%81", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A1", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%82", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A2", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%83", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A3", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%84", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A4", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%85", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A5", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%86", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A6", "a" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%87", "c" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A7", "c" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C4%87", "c" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%90", "d" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%B0", "d" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%88", "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A8", "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C2%A9", "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%89", "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%A9", "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%8A", "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%AA", "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%8B", "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%AB", "e" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%8C", "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%AC", "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%8D", "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%AD", "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%8E", "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%AE", "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%8F", "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%AF", "i" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%91", "n" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%B1", "n" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%92", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%B2", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%93", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%B3", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%94", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%B4", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%95", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%B5", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%96", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%B6", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C5%93", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C5%92", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%98", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%B8", "o" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%9F", "b" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%99", "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%B9", "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%9A", "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%BA", "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%9B", "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%BB", "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%9C", "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%BC", "u" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%9D", "y" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%BD", "y" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%9E", "t" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%BE", "t" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C5%B8", "y" , buffer, false  ) ;
ReplaceStringIn( spchar , "%C3%BF", "y" , buffer, false  ) ;

if ( remplace_espace )
    ChangeCharIn( spchar , ' ' , '_' , false ) ;

//ChangeCharIn( spchar , 0x89 , 'e' ) ;
//ChangeCharIn( spchar , 0xC3 , 'e' ) ;
//ChangeCharIn( spchar , 0xA9 , 'e' ) ;

// remplacement des caracteres non autorise
static char CaractereOkAll[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-+ *()_0123456789./:" ;
static char CaractereOkAlpha[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" ;
char * p=spchar.GetBuffer() ;
while ( *p != 0 )
    {
    bool CarAChanger = true ;
    char * pok ;
    if ( alpha_seul )
        pok = CaractereOkAlpha ;
    else
        pok = CaractereOkAll ;
    while( *pok != 0 )
        {
        if ( *p == *pok )
            {
            CarAChanger=false ;
            break ;
            }
        pok++ ;
        }

    if ( CarAChanger )
        {
        #ifdef _LG_DEBUG_
        if ( *p != ',' && *p != ';' && *p != '\'' && *p != '?' && *p != '['
             && *p != ']' && *p != '!' && *p != '/' && *p != '\\' && *p != '"')
            {
            //cout << p << endl ;
            //int a = 1 ;
            }
        #endif

        *p='-' ;
        }

    p++ ;
    }

if ( strlen )
    spchar.Strlen() ;

spchar.MakeLower() ;

return spchar ;
}
//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction permet de pemplacer un caractere par un autre
void ChangeCharIn( CLGString & spchar , char find , char replace , bool strlenparam /* = true */ )
{
if ( spchar.IsEmpty() )
    return ;

for ( int ic = 0 ; ic < spchar.GetLength() ; ic++ )
    if ( spchar.GetAt(ic) == find )
        spchar.SetAt(ic,replace) ;

if ( strlenparam )
    spchar.Strlen() ;
}



//////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie l'azimut en radian positif vers l'ouest a partir du sud.
// Renvoi l'azimut O° pour le nord 90° pour l'est, 180° pour le sud
// 270° pour l'ouest.

float GetAzimutRad( float lat1Rad , float lat2Rad , float long1Rad , float long2Rad )
{
/*if ( lat1Rad == lat2Rad && long1Rad == long2Rad )
    return 0. ;

if ( lat1Rad == lat2Rad )
    {
    if ( long1Rad < long2Rad )
        return LG_PI/2. ;
    else
        return LG_PI + LG_PI/2. ;
    }

if ( long1Rad == long2Rad )
    {
    if ( lat1Rad > lat2Rad )
        return 0. ;
    else
        return LG_PI ;
    }

float phia=lat1Rad ;
float phib=lat2Rad ;
float thetaa=-long1Rad ;
float thetab=-long2Rad ;

float T = cosf( thetaa - thetab ) * sinf( LG_PI/2. - phib ) * sinf( LG_PI/2. - phia ) + cosf( LG_PI/2. - phib ) * cosf( LG_PI/2. - phia ) ;
//ASSERT( T != 1. ) ;
//float Angle = -atanf( T/sqrtf(1-T*T)) + LG_PI/2. ;
float Angle = -atan2f( T , sqrtf(1.-T*T) ) + LG_PI/2. ;
ASSERT( Angle != 0. ) ;
if ( Angle == 0. )
    cout << lat1Rad << " " << lat2Rad << " " << long1Rad << " " << long2Rad << endl ;
float R = (sinf(phib)-sinf(phia)*cosf(Angle))/(sinf(Angle)*cosf(phia)) ;
//ASSERT( R != 1. ) ;
//float Arc = -atanf( R / (sqrtf(1.-R*R))) + LG_PI/2. ;
float Arc = -atan2f( R , sqrtf(1.-R*R) ) + LG_PI/2. ;

if ( thetab-thetaa > 0 )
    Arc = 2*LG_PI - Arc ;

return Arc ; */

float phi0 = lat1Rad ;
float phix = lat2Rad ;
float lam0 = -long1Rad ;
float lamx = -long2Rad ;
float NUM = sinf(phi0)*cosf(lamx-lam0) - cosf(phi0)*tanf(phix) ;
float DEN = sinf(lamx-lam0) ; // cota = 1/tanga
float azimut = atan2f( DEN , NUM ) ; // l'azimut positif vers l'ouest a partir du sud
//azimut = -azimut ; // -LG_PI/2. ; // positif vers l'est a partir du sud
if ( azimut > LG_PI )
    azimut -= 2*LG_PI ;
if ( azimut < -LG_PI )
    azimut += 2*LG_PI ;
return azimut ;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief renvoie la distance entre 2 points
float GetDistanceRad( float lat1Rad , float lat2Rad , float long1Rad , float long2Rad )
{
float phi0 = lat1Rad ;
float phix = lat2Rad ;
float lam0 = long1Rad ;
float lamx = long2Rad ;
float acos = sinf(phi0)*sinf(phix) + cosf(phi0)*cosf(phix)*cosf(lam0-lamx) ;
if ( acos > 1. )
    acos = 1. ;
return acosf( acos ) ;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Retourne la variation de longitude pour un mouvement.
float GetDeltaLonDeg(float PosN, float PosN_1)
{
// norme du deplacement
float DeltaLonDeg = PosN - PosN_1 ;

// signe du deplacement
if ( fabsf(DeltaLonDeg) > 180. )
    DeltaLonDeg = -SIGNE(DeltaLonDeg) * (360 - fabs(DeltaLonDeg)) ;

// verif
if ( fabsf(DeltaLonDeg) > 180. )
    cout << "**** n:" << PosN << " n_1:" << PosN_1 << " dlon:" << DeltaLonDeg << endl ;

return DeltaLonDeg ;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Retourne la variation de latitude pour un mouvement.
float GetDeltaLatDeg(float PosN, float PosN_1 , bool & ParLePole )
{
ParLePole = false ;
// norme du delacement
float DeltaLatDeg = PosN - PosN_1 ;

// si trop grand
if ( fabs(DeltaLatDeg) > 90. )
    {
    DeltaLatDeg = -SIGNE(DeltaLatDeg) * (180 - fabsf(DeltaLatDeg)) ;
    ParLePole = true ;
    }

// verif
if ( fabsf(DeltaLatDeg) > 90. )
    cout << "**** n:" << PosN << " n_1:" << PosN_1 << " dlat:" << DeltaLatDeg << endl ;

return DeltaLatDeg ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Renvoi le delta lat et long et true si par les poles.
bool  GetDeltaLatLonDeg( float PosLatN , float PosLatN_1 , float PosLonN , float PosLonN_1 , float & DeltaLat , float & DeltaLon )
{
bool ParLePole ;
DeltaLat = GetDeltaLatDeg( PosLatN , PosLatN_1 , ParLePole ) ;
DeltaLon = GetDeltaLonDeg( PosLonN , PosLonN_1 ) ;

if ( ParLePole )
    DeltaLon *= -1. ;

return ParLePole ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction est appelee lorsqu'un "ASSERT()".
void AssertFailed( const char * NomFichier , const int LigneFichier )
{
static int NumAssert= 0 ;
//char TmpChar[5000] ;
printf("%ld Une assertion n'est pas verifiee a : %s ligne %ld\n", NumAssert++, NomFichier, LigneFichier);

// beep system
//system( (const char *) "beep" , false ) ;
//system( (const char *) "dd if=/dev/random of=/dev/audio count=2 2>&1 | /dev/null" , false ) ;
int retour = system( "paplay /usr/share/sounds/freedesktop/stereo/bell.oga" ) ;
if ( retour )
    cout << "impossible de jouer un son." << endl ;

// sortie au bout de 70 ASSERT
static int iass = 1 ;
if ( !(iass++ % 70) )
    exit(1);
}



