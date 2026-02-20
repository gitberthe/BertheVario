//////////////////////////////////////////////////////////////////////////////
/// \file  CoGeneral.h
/// \date  11/11/2002 :  date de creation
///
/// \brief Fichier commun inclus dans les fichiers .h de chaque classe.
///
/// \date 11/11/2002 : Compilation de wxWindows 2.2.9 avec des MFC.
/// \date 02/10/2004 : Compilation avec wxWindows 2.4.2.
/// \date 25/04/2012 : Compilation __GNUWIN32__ avec wxWidgets 2.8.11
/// \date 18/02/2016 : Modification sans _AVEC_WXWIN_H pour snb-artmap (strlwr...).
/// \date 16/11/2018 : Ajout de CLGTObjets::GetMaxIndex().
/// \date 19/10/2020 : NVIDIA_FUNC
/// \date 30/06/2025 : Derniere modification.
///

#ifndef _COGENERAL_
#define _COGENERAL_

/****************************************************************************/
/*            Variables a mettre en commentaire si necessaire               */
/****************************************************************************/

#define _REDEFINITION_ASSERT_
//#define _LIB_XML_

/****************************************************************************/
/*           includes et definitions quelque soit l'environnement           */
/****************************************************************************/

#include <stdarg.h>
#include <math.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
 //#include <sys/sysinfo.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
//#include <termio.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <climits>

//#include <sys/feature_tests.h>
#define LG_PI      (3.14159265358979323846264338327950288419716939937510582) ///< 180° en radians (PI=3.14...)

using namespace std;

// typedef suivant la machine
typedef     unsigned char    int8b  ;
typedef     short   int16b ;
typedef     int     int32b ;
typedef     float   float32b ;
typedef     double  float64b ;
typedef     long double float128b ;

// compilation 64 bits ou 32 bits
#if defined( __LP64__ ) || defined( _LP64 )
    typedef     long        int64b ;
#else
    typedef     long long   int64b ;
#endif // 64 bits

void    MyMessageError( const char * Message ) ;
#define stricmp strcasecmp
#define strnicmp strncasecmp
char * strlwr( char * ) ;
char * strupr( char * ) ;
char * strrev( char * ) ;

/// \brief renvoie l'azimut positif vers l'ouest a partir du sud
float GetAzimutRad( float Lat1Rad , float Lat2Rad , float Long1Rad , float Long2Rad ) ;
float GetDistanceRad( float Lat1Rad , float Lat2Rad , float Long1Rad , float Long2Rad ) ;
float GetDeltaLatDeg( float PosN , float PosN_1 , bool & ParLePole ) ;
float GetDeltaLonDeg( float PosN , float PosN_1 ) ;
bool  GetDeltaLatLonDeg( float PosLatN , float PosLatN_1 , float PosLonN , float PosLonN_1 , float & DeltaLat , float & DeltaLon ) ;

/****************************************************************************/
/*       Positionnement des variables fonction de l'environnement           */
/****************************************************************************/
/* Les variables si dessous ne sont plus definie a partir de __GNUWIN32__   */
/*             _WINDOWS_ _IN_WINDOWS _MSC_VER _AVEC_AFX_H                   */
/****************************************************************************/

#include <float.h>

// pour une compilation win32
#ifdef __GNUWIN32__
 #include "pthread.h"
 #include "sched.h"
 #include "semaphore.h"

// pour une compilation non win32
#else
 //#include <stddef.h>
 //#include <unistd.h>
 //#include <pthread.h>

 //typedef int            BOOL ;
 //typedef long           DWORD ;
 //typedef unsigned int   UINT ;
 //typedef unsigned char  BYTE ;
 // typedef unsigned short WORD ;
#endif // __GNUWIN32__


/****************************************************************************/
/*                            mode _LG_DEBUG_                               */
/****************************************************************************/

#ifdef  _DEBUG
 #ifndef _LG_DEBUG_
  #define _LG_DEBUG_
 #endif
#endif // _DEBUG

/*#ifdef __WXDEBUG__
 #ifndef _LG_DEBUG_
  #define _LG_DEBUG_
 #endif
#endif // __WXDEBUG__*/

// fonction appele par LG_ASSERT_VALID()
#ifdef _LG_DEBUG_
 #define LG_ASSERT_VALID(this)   this->LGAssertValid()
#else
 #define LG_ASSERT_VALID(this)
#endif // _GRL_DEBUG_

// message d'erreur en mode debug
#ifdef _LG_DEBUG_
 #define DEBUG_PRINT(expression) fprintf( stderr , expression ) ;
#else
 #define DEBUG_PRINT(expression)
#endif // _LG_DEBUG_

#ifdef _REDEFINITION_ASSERT_
  //////////////////////////////////////////////////////////////////////
  // definition de la fonction AssertFailed() suivant le systeme
  // d'exploitation
  void AssertFailed( const char * NomFichier , const int LigneFichier ) ;

  #ifdef _LG_DEBUG_
    #define ASSERT(expression) if (!(expression)) AssertFailed( __FILE__ , __LINE__ ) ;
  #else
    #define ASSERT(expression)
  #endif // _LG_DEBUG_
#endif // _REDEFINITION_ASSERT_

//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction est appelee lorsqu'un "ASSERT()".
void AssertFailed( const char * NomFichier , const int LigneFichier ) ;

#endif //_COGENERAL_

