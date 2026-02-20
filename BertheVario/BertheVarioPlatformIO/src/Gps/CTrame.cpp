////////////////////////////////////////////////////////////////////////////////
/// \file CTrame.cpp
///
/// \brief
///
/// \date creation   : 03/03/2024
/// \date 19/10/2025 : introduction du HDop
/// \date 21/10/2025 : nombre de fix altibaro, debut de vol
/// \date 24/10/2025 : modification break dans TraiteBuffer()
/// \date 25/10/2025 : '*' dans trame debut/fin message
/// \date 04/01/2026 : refonte TraiteBuffer()
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

unsigned long CTrame::m_MillisPremierGGA = 0 ;
int           CTrame::m_NombreSatellite = -1 ;
float         CTrame::m_HDop = 100. ;
float         CTrame::m_PDopGSA = 100. ;
float         CTrame::m_HDopGSA = 100. ;
float         CTrame::m_VDopGSA = 100. ;

///////////////////////////////////////////////////////////////////////////////
/// \brief
CTrame::CTrame()
{
m_pCharBuff = m_BufRecep ;
m_pCharBuffOverflow = & m_BufRecep[ (int)(TAILLE_BUFFER_TRAME*0.8) ] ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Ma comparaison de chaine.
bool CTrame::StrCmp( const char * pa , const char * pb )
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

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui empile le caractere dans le buffer. Met un espace si 2
/// virgules consecutives pour strtok.
void CTrame::Push(char c)
{
// si buffer presque plein
if ( m_pCharBuff > m_pCharBuffOverflow  )
    m_pCharBuff = m_BufRecep ;
// si separation dans message
else if ( c == ',' )
    *m_pCharBuff++ = 0 ;
// si debut de message suivant ou fin de message courant
else if ( c == '$' ) //|| c == '*' )
    {
    // fin de message
    *m_pCharBuff++ = 0 ;

    // traitement du message
    TraiteBuffer() ;

    // raz dans le buffer
    m_pCharBuff = m_BufRecep ;
    }
// si caractere de message
else
    // ajout du caractere dans le message
    *m_pCharBuff++ = c ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traitement de la trame et de ses parametres
void CTrame::TraiteBuffer()
{
char *pChar ;
int ipar ;

// suivant le recepteur gps les 2 premieres lettres different
m_BufRecep[0] =
m_BufRecep[1] = '-' ;

// repartition de champs dans le tableau
m_TabParam.SetSize(0) ;
pChar = m_BufRecep ;
while( pChar < m_pCharBuff )
    {
    // ajout au tableau des parametres
    // sauf premier parametre
    if ( pChar != m_BufRecep )
        m_TabParam.Add( pChar ) ;
    // prochain parametre
    while( *pChar != 0 )
        pChar++ ;
    // premier caractere parametre suivant
    pChar++ ;
    }

// si pas de parametres
if ( m_TabParam.GetSize() == 0 )
    return ;

// entete du message
pChar = m_BufRecep ;

// raz i parametre
ipar = 0 ;

/*
Serial.print( pChar ) ;
Serial.print( ";" ) ;
for ( int i = 0 ; i < m_TabParam.GetSize() ; i++ )
    {
    Serial.print( m_TabParam[i] ) ;
    Serial.print( ";" ) ;
    }
Serial.println( "<fin>" ) ; */

// si --GGA
if ( StrCmp(pChar,"--GGA" ) )
    {
    while ( ipar < m_TabParam.GetSize() )
        {
        // parametre i
        pChar = m_TabParam[ipar] ;

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

            #ifdef DEBUG_512
            m_MillisPremierGGA = millis() ;
            g_GlobalVar.m_TerrainPosCur.m_Lon = 3.257269 ; // thuret
            g_GlobalVar.m_TerrainPosCur.m_Lat = 45.982155 ;
            g_GlobalVar.m_AltitudeSolHgt = g_GlobalVar.m_Hgt2Agl.GetGroundZ( g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_Lat ) ;
            g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec = 1500 ;
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
            // mutex pour ecriture igc
            g_GlobalVar.m_MutexLatLonGps.PrendreMutex() ;
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
            g_GlobalVar.m_MutexLatLonGps.RelacherMutex() ;
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
            // mutex pour ecriture igc
            g_GlobalVar.m_MutexLatLonGps.PrendreMutex() ;
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
            g_GlobalVar.m_MutexLatLonGps.RelacherMutex() ;
            #endif
            }
        // nombre de satellites en poursuite
        else if ( ipar == 6 )
            {
            /*int NbSat = atoi( pChar ) ;
            // si nombre de satelites modifiée
            if ( m_NombreSatellite != NbSat )
                {
                //Serial.print( "hdop : " ) ;
                //Serial.println( m_HDop , 2 ) ;
                //g_GlobalVar.m_PileVit.SatChange() ;
                } */

            m_NombreSatellite = atoi( pChar ) ;

            #ifdef GPS_DEBUG
             Serial.print( "NbSat:" ) ;
             Serial.println( m_NombreSatellite ) ;
            #endif
            }
        // HDOP
        else if ( ipar == 7 )
            {
            // 1 optimale, entre 2 et 3 excellent, entre 5 et 6 bon, au-delà de 8 non acceptable
            m_HDop = atof( pChar ) ;
            #ifdef GPS_DEBUG
             Serial.print( "hdop : " ) ;
             Serial.println( m_HDop , 2 ) ;
            #endif
            }
        // altitude
        else if ( ipar == 8 )
            {
            #ifndef SIMU_VOL
             g_GlobalVar.m_AltiGps = atof( pChar ) ;
             #ifdef GPS_DEBUG
              Serial.print( "altigps:" ) ;
              Serial.println( g_GlobalVar.m_AltiGps ) ;
             #endif
            #endif
            }
        // correction Geoide en metres
        /*else if ( ipar == 10 )
            {
            m_AltiGpsGeoide = atof( pChar ) ;
            Serial.print( "altigeoide:" ) ;
            Serial.println( m_AltiGpsGeoide ) ;
            } */
        else if ( ipar > 8 )
            break ;

        // incrementation du parametre
        ipar++ ;
        }
    }
// si --VTG
else if ( StrCmp(pChar,"--VTG" ) )
    {
    #ifdef SIMU_VOL
     return ;
    #endif // SIMU_VOL
    ipar = 0 ;
    while ( ipar < m_TabParam.GetSize() )
        {
        // parametre i
        pChar = m_TabParam[ipar] ;

        // cap
        if ( ipar == 0 )
            {
            // si cap bien present
            if ( *pChar != ' ' )
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

        // incrementation du parametre
        ipar++ ;
        }
    }
// si --RMC
else if ( StrCmp(pChar,"--RMC" ) )
    {
    #ifdef SIMU_VOL
     return ;
    #endif
    ipar = 0 ;
    while ( ipar < m_TabParam.GetSize() )
        {
        // parametre i
        pChar = m_TabParam[ipar] ;

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
        else if ( ipar > 8 )
            break ;

        // incrementation du parametre
        ipar++ ;
        }
    }
/*
// si --GSA
else if ( !strcmp(pChar,"--GSA" ) )
    {
    static int Minimum = 8000 ;
    static int Maximum = 0 ;
    #ifdef SIMU_VOL
     return ;
    #endif
    while ( (pChar=strtok(NULL,Separ)) != NULL )
        {
        // decalage des parametres
        m_PDopGSA = m_HDopGSA ;
        m_HDopGSA = m_VDopGSA ;
        m_VDopGSA = m_ChecksumGsa ;
        m_ChecksumGsa = atof( pChar );
        }
    if ( m_MillisPremierGGA != 0 )
        {
        Minimum = MIN( Minimum , g_GlobalVar.m_AltiGps ) ;
        Maximum = MAX( Maximum , g_GlobalVar.m_AltiGps ) ;
        }
    Serial.print("\nmin:") ;
    Serial.println( Minimum ) ;
    Serial.print("max:") ;
    Serial.println( Maximum ) ;
    Serial.print("AltiGps:") ;
    Serial.println( g_GlobalVar.m_AltiGps ) ;
    Serial.print( "PDop:" ) ;
    Serial.println( m_PDopGSA ) ;
    Serial.print( "HDop:" ) ;
    Serial.println( m_HDopGSA ) ;
    Serial.print( "VDop:" ) ;
    Serial.println( m_VDopGSA ) ;
    } // */
}
