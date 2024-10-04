////////////////////////////////////////////////////////////////////////////////
/// \file CTrame.cpp
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 04/10/2024
///

#include "../BertheVario.h"

unsigned long CTrame::m_MillisPremierGGA = 0 ;
int           CTrame::m_NombreSatellite = -1 ;

/*
///////////////////////////////////////////////////////////////////////////////
/// \brief Ma comparaison de chaine.
inline bool StrCmp( const char * pa , const char * pb )
{
while ( true )
    {
    // caractere different
    if ( *pa != *pb )
        return false ;
    pa++ ;
    pb++ ;

    // fin de chaine
    if ( *pa == 0 && *pb == 0 )
        return true ;
    }
}
*/

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui empile le caractere dans le buffer. Met un espace si 2
/// virgules consecutives pour strtok.
void CTrame::Push(char c)
{
// si separation dans message
if ( c == ',' )
    {
    // si 2 virgules successives insertion espace pour strtock
    if ( m_iBuf > 0 && m_BufRecep[m_iBuf-1] == ',' )
        m_BufRecep[m_iBuf++] = ' ' ;
    m_BufRecep[m_iBuf++] = c ;
    }
// si debut de message
else if ( c == '$' )
    {
    // traitement message precedent
    m_BufRecep[m_iBuf++] = 0 ;
    TraiteBuffer() ;

    // raz buffer
    m_iBuf = 0 ;
    m_BufRecep[m_iBuf++] = c ;
    }
// si caractere de message
else
    m_BufRecep[m_iBuf++] = c ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traitement de la trame et de ses parametres
void CTrame::TraiteBuffer()
{
int ipar = 0 ;
char Separ[] = "$,*" ;
char * pChar = strtok( m_BufRecep , Separ ) ;
// suivant le recepteur gps les 2 premieres lettres different
m_BufRecep[1] =
m_BufRecep[2] = '-' ;

// si NULL
if ( pChar == NULL )
    return ;

// si --GGA
if ( !strcmp(pChar,"--GGA" ) )
    {
    while ( (pChar=strtok(NULL,Separ)) != NULL )
        {
        // heure
        if ( ipar == 0 )
            {
            int heure = 0 ;
            heure += (pChar[0]-'0') * 10 * 3600 ;   // 10ene d'heure en seconde
            heure += (pChar[1]-'0') * 3600 ;        // heure en seconde
            heure += (pChar[2]-'0') * 10 * 60 ;     // 10ene de minutes en seconde
            heure += (pChar[3]-'0') * 60 ;          // minutes en seconde
            heure += (pChar[4]-'0') * 10 ;          // 10ene de seconde
            heure += (pChar[5]-'0') ;               // seconde
            heure += g_GlobalVar.m_Config.m_dtu * 3600 ;     // dtu en secondes

            // si gps pas accrocché ou erreur
            if ( heure < 0 )
                heure = 0 ;

            #ifndef SIMU_VOL
             g_GlobalVar.m_HeureSec = heure ;
            #endif

            #ifdef _LG_DEBUG_
             Serial.print( "heure gps sec : " ) ;
             Serial.println( heure ) ;
            #endif
            }
        // latitude valeur
        if ( ipar == 1 )
            {
            // si parametre vide
            if ( strlen( pChar ) != 10 )
                {
                //                               1234567
                //strcpy(g_GlobalVar.m_LatChar, "0000000") ;
                //g_GlobalVar.m_LatFloat = 0.;
                break ;
                }

            // temps premier GGA a la latitude valide
            if ( m_MillisPremierGGA == 0 )
                m_MillisPremierGGA = millis() ;

            // 01234567
            // 4807.038
            #ifndef SIMU_VOL
            g_GlobalVar.m_MutexVariable.PrendreMutex() ;
             g_GlobalVar.m_LatChar[0] = pChar[0] ;
             g_GlobalVar.m_LatChar[1] = pChar[1] ;
             g_GlobalVar.m_LatChar[2] = pChar[2] ;
             g_GlobalVar.m_LatChar[3] = pChar[3] ;

             g_GlobalVar.m_LatChar[4] = pChar[5] ;
             g_GlobalVar.m_LatChar[5] = pChar[6] ;
             g_GlobalVar.m_LatChar[6] = pChar[7] ;
             g_GlobalVar.m_LatChar[7] = 0 ;

             float Lat = 0. ;
             Lat += ((float)(pChar[0]-'0')) * 10 ;        // 10 ene de lat
             Lat += ((float)(pChar[1]-'0')) ;             // lat deg
             Lat += ((float)(pChar[2]-'0')) * 10. / 60. ; // lat 10 ene de minutes
             Lat += ((float)(pChar[3]-'0')) / 60. ;       // lat minutes

             Lat += ((float)(pChar[5]-'0')) / 60. / 10. ;  // lat 10ieme minutes
             Lat += ((float)(pChar[6]-'0')) / 60. / 100. ; // lat 100ieme minutes
             Lat += ((float)(pChar[7]-'0')) / 60. / 1000. ; // lat 1000ieme minutes
             Lat += ((float)(pChar[8]-'0')) / 60. / 10000. ; // lat 10000ieme minutes
             g_GlobalVar.m_TerrainPosCur.m_Lat = Lat ;
            #endif
            }
        // latitude signe
        else if ( ipar == 2 )
            {
            #ifndef SIMU_VOL
             if ( *pChar == 'S' )
                 g_GlobalVar.m_TerrainPosCur.m_Lat *= -1 ;
            g_GlobalVar.m_MutexVariable.RelacherMutex() ;
            #endif
            }
        // longitude valeur
        else if ( ipar == 3 )
            {
            // si parametre vide
            if ( strlen( pChar ) != 11 )
                {
                //                               12345678
                //strcpy(g_GlobalVar.m_LonChar, "00000000") ;
                //g_GlobalVar.m_LonFloat = 0 ;
                break ;
                }

            // 012345678
            // 01131.324
            #ifndef SIMU_VOL
            g_GlobalVar.m_MutexVariable.PrendreMutex() ;
             g_GlobalVar.m_LonChar[0] = pChar[0] ;
             g_GlobalVar.m_LonChar[1] = pChar[1] ;
             g_GlobalVar.m_LonChar[2] = pChar[2] ;
             g_GlobalVar.m_LonChar[3] = pChar[3] ;
             g_GlobalVar.m_LonChar[4] = pChar[4] ;

             g_GlobalVar.m_LonChar[5] = pChar[6] ;
             g_GlobalVar.m_LonChar[6] = pChar[7] ;
             g_GlobalVar.m_LonChar[7] = pChar[8] ;
             g_GlobalVar.m_LonChar[8] = 0 ;

             float Lon = 0. ;
             Lon += ((float)(pChar[0]-'0')) * 100 ;       // 100 ene de lon
             Lon += ((float)(pChar[1]-'0')) * 10 ;        // 10 ene de lon
             Lon += ((float)(pChar[2]-'0')) ;             // lon deg
             Lon += ((float)(pChar[3]-'0')) * 10. / 60. ; // lon 10 ene de minutes
             Lon += ((float)(pChar[4]-'0')) / 60. ;       // lon minutes

             Lon += ((float)(pChar[6]-'0')) / 60. / 10. ;  // lon 10ieme minutes
             Lon += ((float)(pChar[7]-'0')) / 60. / 100. ; // lon 100ieme minutes
             Lon += ((float)(pChar[8]-'0')) / 60. / 1000. ; // lon 1000ieme minutes
             Lon += ((float)(pChar[9]-'0')) / 60. / 10000. ;// lon 10000ieme minutes
             g_GlobalVar.m_TerrainPosCur.m_Lon = Lon ;
            #endif
            }
        // longitude signe
        else if ( ipar == 4 )
            {
            #ifndef SIMU_VOL
             if ( *pChar == 'W' )
                 g_GlobalVar.m_TerrainPosCur.m_Lon *= -1 ;
            g_GlobalVar.m_MutexVariable.RelacherMutex() ;
            // mise a jour des altitudes
            g_GlobalVar.m_AltitudeSolHgt = g_GlobalVar.m_Hgt2Agl.GetGroundZ( g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_Lat ) ;
            g_GlobalVar.m_TerrainPosCur.m_AltiBaro = g_GlobalVar.m_MS5611.GetAltiMetres() ;
            #endif
            }
        // nombre de satellites en poursuite
        else if ( ipar == 6 )
            {
            int NbSat = atoi( pChar ) ;
            // si nombre de satelites modifiée
            if ( m_NombreSatellite != NbSat )
                g_GlobalVar.m_PileVit.SatChange() ;

            m_NombreSatellite = NbSat ;

            #ifdef GPS_DEBUG
             Serial.print( "NbSat:" ) ;
             Serial.println( NbSat ) ;
            #endif
            }
        // altitude
        else if ( ipar == 8 )
            {
            #ifndef SIMU_VOL
            g_GlobalVar.m_MutexVariable.PrendreMutex() ;
             g_GlobalVar.m_AltiGps = atof( pChar ) ;
            g_GlobalVar.m_MutexVariable.RelacherMutex() ;
            #endif
            }

        ipar++ ;
        }
    }
// si --VTG
else if ( !strcmp(pChar,"--VTG" ) )
    {
    #ifdef SIMU_VOL
     return ;
    #endif // SIMU_VOL
    while ( (pChar=strtok(NULL,Separ)) != NULL )
        {
        // cap
        if ( ipar == 0 )
            {
            g_GlobalVar.m_CapGpsDeg = (int)atof( pChar ) ;
            }
        // vitesse sol
        else if ( ipar == 6 )
            {
            g_GlobalVar.m_VitesseKmh = atof( pChar ) ;
            #ifdef GPS_DEBUG
             Serial.print( "vitesse sol gps : " ) ;
             Serial.print( pChar ) ;
             Serial.print( " " ) ;
             Serial.println( g_GlobalVar.m_VitesseKmh ) ;
            #endif

            }
        // si fin parametre car pas d'*
        else if ( ipar > 6 )
            break ;

        ipar++ ;
        }
    }
// si --RMC
else if ( !strcmp(pChar,"--RMC" ) )
    {
    #ifdef SIMU_VOL
     return ;
    #endif
    while ( (pChar=strtok(NULL,Separ)) != NULL )
        {
        // date
        if ( ipar == 8 )
            {
            int jour = 0 ;
            jour += (pChar[0]-'0') * 10 ;   // 10 ene de jour
            jour += (pChar[1]-'0') ;        // jour
            g_GlobalVar.m_Jour = (jour>=1) ? jour : 1 ;

            int mois = 0 ;
            mois += (pChar[2]-'0') * 10 ;   // 10 ene de mois
            mois += (pChar[3]-'0') ;        // mois
            g_GlobalVar.m_Mois = (mois>=1) ? mois : 1 ;

            int annee = 2000 ;
            annee += (pChar[4]-'0') * 10 ;   // 10 ene de annee
            annee += (pChar[5]-'0') ;        // annee
            g_GlobalVar.m_Annee = (annee>2000) ? annee : 2000 ;
            }

        ipar++ ;
        }
    }
}
