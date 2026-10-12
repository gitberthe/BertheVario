////////////////////////////////////////////////////////////////////////////////
/// \file COpenAirFile.cpp
///
/// \brief
///
/// \date 10/10/2026 : creation
/// \date 12/10/2026 : calcul geodesique de l'arc debut/fin DB et V X=
///

#include "../CompZoneAerienne.h"

#include "computeOpenAirArc.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Init
COpenAirFile::COpenAirFile()
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Recherche la chaine dans les champs indifferement de la casse. Ex FT ou AGL
long COpenAirFile::RechercheChamp( const std::string & ToFind, const std::vector<std::string> & tokens )
{
// casse
std::string ToFindCasse = ToFind ;
for (char &c : ToFindCasse)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

// recharche dans les chaines
for ( long ic = 0 ; ic < (long)tokens.size() ; ic++ )
    {
    std::string MotCasse = tokens[ic] ;
    // casse
    for (char &c : MotCasse)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    if ( strstr( MotCasse.c_str() , ToFindCasse.c_str() ) != NULL )
        return ic ;
    }

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lit un fichier openair
long COpenAirFile::TraiteFichier( FILE *fp )
{
        /*
        // nom
        pZone->m_Name = jf["features"][izf]["properties"]["name"] ;

        // zone a ne pas traiter
        if ( strstr( pZone->m_Name.c_str() , "LTA FRANCE 1" ) != NULL ||
             strstr( pZone->m_Name.c_str() , "France-Metropolitaine" ) != NULL )
            {
            cerr << pZone->m_Name << " **** non traite **** " << endl ;
            continue ;
            }

        // bas de zone
        pZone->m_Bottom = jf["features"][izf]["properties"]["bottom_m"] ;

        // coordonnees de la zone
        auto CordArr = jf["features"][izf]["geometry"]["coordinates"][0] ;
        for (auto it = CordArr.begin(); it != CordArr.end(); ++it)
            {
            CVecZoneReduce::st_coord_poly * pStPts = new CVecZoneReduce::st_coord_poly ;
            pZone->m_VecPtsBig.push_back( pStPts ) ;
            pStPts->m_Lon = (*it)[0] ;
            pStPts->m_Lat = (*it)[1] ;
            }
        */
//ifs.exceptions() ; // std::ifstream::badbit);
//std::cin.exceptions(std::ios::badbit);

// pour toutes les lignes du fichier
//for ( std::string line; std::getline(ifs, line); )

char *line = NULL; // Buffer for the line
size_t len = 0; // Buffer size (managed by getline)
ssize_t read; // Number of characters read

while ((read = getline(&line, &len, fp)) != -1)
    {
    // si ligne vide
    if ( line == NULL || !strcmp(line,"") || line[0] == '*' )
        continue ;

    // decoupage en mots
    char delimiter[] = " \n" ;
    char * pChar = strtok(line,delimiter) ;
    std::vector<std::string> tokens;

    while ( pChar != NULL )
        {
        tokens.push_back(pChar);
        pChar = strtok(NULL,delimiter) ;
        }

    // traite la ligne
    TraiteLigne( tokens ) ;
    }

free( line ) ;

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite une ligne openair
long COpenAirFile::TraiteLigne(std::vector<std::string>& tokens)
{
// si ligne vide ou commentaire
if ( tokens.size() == 0 || tokens[0][0] == '*' )
    return 1 ;

// classe de la zone
if ( tokens[0] == "AC" )
    {
    m_pZoneCur = new CZone ;
    m_DClockwise = true ;
    // si zone non controlee
    if ( ! TraiteACClasseDeLaZone( tokens ) )
        m_pZoneArr.push_back( m_pZoneCur ) ;
    else
        {
        delete m_pZoneCur ;
        m_pZoneCur = NULL ;
        }
    }
// AN nom de zone
else if ( tokens[0] == "AN" && m_pZoneCur != NULL )
    {
    TraiteANNomDeLaZone( tokens ) ;
    //cout << m_pZoneCur->m_Name << endl ;
    }
// type de la zone
else if ( tokens[0] == "AY" && m_pZoneCur != NULL )
    {}
// AF frequence
else if ( tokens[0] == "AF" && m_pZoneCur != NULL )
    {}
// AG Station Name
else if ( tokens[0] == "AG" && m_pZoneCur != NULL )
    {}
else if ( tokens[0] == "AH" && m_pZoneCur != NULL )
    TraiteAHAltitudeHaute( tokens ) ;
else if ( tokens[0] == "AL" && m_pZoneCur != NULL )
    TraiteALAltitudeBasse( tokens ) ;
// DP point de zone
else if ( tokens[0] == "DP" && m_pZoneCur != NULL )
    {
    CVecZoneReduce::st_coord_poly * pStPts = new CVecZoneReduce::st_coord_poly ;
    m_pZoneCur->m_VecPtsBig.push_back( pStPts ) ;

    TraiteDPPointDePolygone( pStPts, tokens ) ;

    //cout << m_pZoneCur->m_Name << endl ;
    //cout << m_pZoneCur->m_VecPtsBig.size() << endl ;
    //cout << m_pZoneArr.size() << endl ;
    }
// variable
else if ( tokens[0] == "V" && m_pZoneCur != NULL )
    TraiteVVariableAssignement(tokens) ;
// arc de cercle
else if ( tokens[0] == "DB" && m_pZoneCur != NULL )
    TraiteDBArcBetweenPoints(tokens) ;
// cercle
else if ( tokens[0] == "DC" && m_pZoneCur != NULL )
    TraiteDCCercle(tokens) ;
// champs inconnu
else if ( m_pZoneCur != NULL )
    {
    cerr << "champ non connu : " << tokens[0] << endl ;
    exit( 1 ) ;
    }

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief classe A B C D E F G ZSM
/// Espaces interdit au parapentes: A B C D (B pas en france)
/// Espaces autorise au parapentes: E F G (F pas en france)
long COpenAirFile::TraiteACClasseDeLaZone(std::vector<std::string> & tokens)
{
// espaces aeriens non controle
//if ( tokens[1][0] == 'G' )
//    return 1 ;
if ( tokens[1] == "ZSM" )
    m_pZoneCur->m_TypeEspaceOpenAir = ZSM_PROTECT_BulleQuietude ;

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Cree et format le nom de la zone
long COpenAirFile::TraiteANNomDeLaZone(std::vector<std::string> & tokens)
{
for ( long it = 1 ; it < (long)tokens.size() ; it++ )
    {
    m_pZoneCur->m_Name += tokens[it] ;
    m_pZoneCur->m_Name += " " ;
    }

std::replace( m_pZoneCur->m_Name.begin(), m_pZoneCur->m_Name.end(), '|', ' '); // replace all 'x' to 'y'
std::replace( m_pZoneCur->m_Name.begin(), m_pZoneCur->m_Name.end(), '>', ' '); // replace all 'x' to 'y'
std::replace( m_pZoneCur->m_Name.begin(), m_pZoneCur->m_Name.end(), '<', ' '); // replace all 'x' to 'y'
std::replace( m_pZoneCur->m_Name.begin(), m_pZoneCur->m_Name.end(), '&', ' '); // replace all 'x' to 'y'

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
long COpenAirFile::TraiteAHAltitudeHaute(std::vector<std::string> & tokens)
{
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
long COpenAirFile::TraiteALAltitudeBasse(std::vector<std::string> & tokens)
{
return 0 ;
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Traite un point de polygone sous ses 2 formats
long COpenAirFile::TraiteDPPointDePolygone( CVecZoneReduce::st_coord_poly * pStPts, std::vector<std::string> & tokens )
{
char TmpChar[1000] = "" ;

// concatenation de la chaine
for ( long it = 1 ; it < (long)tokens.size() ; it++ )
    strcat( TmpChar, tokens[it].c_str()) ;

// compage des : et .
long Nb2Pts = 0 ;
long Nb1Pts = 0 ;
char * pChar = TmpChar ;
while ( *pChar != 0 )
    {
    if ( *pChar == ':' )
        Nb2Pts++ ;
    if ( *pChar == '.' )
        Nb1Pts++ ;
    pChar++ ;
    }

// nord et est
bool Nord = strchr( TmpChar , 'N' ) != NULL ;
bool Sud = strchr( TmpChar , 'S' ) != NULL ;
bool Est = strchr( TmpChar , 'E' ) != NULL ;
bool Ouest = strchr( TmpChar , 'W' ) != NULL ;

// pour une separation d:m:s
if ( Nb1Pts == 0 )
    {
    char Delimiter[] = ":NSEW " ;
    double Deg,Min,Sec ;

    // latitude
    Deg = atoi( strtok(TmpChar,Delimiter) ) ;
    Min = atoi( strtok(NULL,Delimiter) ) ;
    Sec = atoi( strtok(NULL,Delimiter) ) ;

    pStPts->m_Lat = Deg + Min/60. + Sec/3600. ;

    if ( !Nord )
        {
        if ( !Sud )
            goto erreur ;
        pStPts->m_Lat *= -1 ;
        }

    // longitude
    Deg = atoi( strtok(NULL,Delimiter) ) ;
    Min = atoi( strtok(NULL,Delimiter) ) ;
    Sec = atoi( strtok(NULL,Delimiter) ) ;

    pStPts->m_Lon = Deg + Min/60. + Sec/3600. ;

    if ( !Est )
        {
        if ( !Ouest )
            goto erreur ;
        pStPts->m_Lon *= -1 ;
        }
    }
// pour une separation d:m.s
else if ( Nb1Pts )
    {
    char Delimiter[] = ":NSEW " ;
    double Deg,Min,Sec ;

    // latitude
    if ( Nb2Pts == 2 )
        {
        Deg = atoi( strtok(TmpChar,Delimiter) ) ;
        Min = atof( strtok(NULL,Delimiter) ) ;
        Sec = 0. ;
        }
    else if ( Nb2Pts == 4 )
        {
        Deg = atoi( strtok(TmpChar,Delimiter) ) ;
        Min = atoi( strtok(NULL,Delimiter) ) ;
        Sec = atof( strtok(NULL,Delimiter) ) ;
        }
    else
        {
        cerr << "**** erreur lat DP." << endl ;
        exit(1) ;
        }

    pStPts->m_Lat = Deg + Min/60. + Sec/3600. ;

    if ( !Nord )
        {
        if ( !Sud )
            goto erreur ;
        pStPts->m_Lat *= -1 ;
        }

    // longitude
    if ( Nb2Pts == 2 )
        {
        Deg = atoi( strtok(NULL,Delimiter) ) ;
        Min = atof( strtok(NULL,Delimiter) ) ;
        Sec = 0. ;
        }
    else if ( Nb2Pts == 4 )
        {
        Deg = atoi( strtok(NULL,Delimiter) ) ;
        Min = atoi( strtok(NULL,Delimiter) ) ;
        Sec = atof( strtok(NULL,Delimiter) ) ;
        }
    else
        {
        cerr << "**** erreur lon DP." << endl ;
        exit(1) ;
        }

    pStPts->m_Lon = Deg + Min/60. + Sec/3600. ;

    if ( !Est )
        {
        if ( !Ouest )
            goto erreur ;
        pStPts->m_Lon *= -1 ;
        }
    }
// erreur programme
else
    {
    erreur :
    cerr << "*** probleme DP " << TmpChar << endl ;
    exit( 1 ) ;
    }

return 0 ;
}
////////////////////////////////////////////////////////////////////////////////
/// \brief Traite le champ V. Variable de centre de cercle ou arc de cercle.
long COpenAirFile::TraiteVVariableAssignement(std::vector<std::string> & tokens)
{
// traite le champ sens de rotation D
if ( tokens[1][0] == 'D' )
    {
    bool UnTrouve = false ;

    // recherche + sens horaire
    std::size_t found = tokens[1].find('+');
    if (found!=std::string::npos)
        {
        UnTrouve = true ;
        m_DClockwise = true ;
        }

    // recherche - sens trigonometrique
    found = tokens[1].find('-');
    if (found!=std::string::npos)
        {
        UnTrouve = true ;
        m_DClockwise = false ;
        }

    // si probleme + -
    if ( ! UnTrouve )
        {
        char TmpChar[1000] = "" ;

        // concatenation de la chaine
        for ( long it = 0 ; it < (long)tokens.size() ; it++ )
            strcat( TmpChar, tokens[it].c_str()) ;

        cerr << "**** probleme champ V D=+/- : " << TmpChar << endl ;
        exit( 1 ) ;
        }
    }
// traite le champ centre de cercle X
else if ( tokens[1][0] == 'X' )
    {
    std::replace( tokens[1].begin(), tokens[1].end(), 'X', ' ');
    std::replace( tokens[1].begin(), tokens[1].end(), '=', ' ');

    TraiteDPPointDePolygone( &m_XCenterDaDbDc, tokens ) ;
    }
// erreur programme
else
    {
    char TmpChar[1000] = "" ;

    // concatenation de la chaine
    for ( long it = 0 ; it < (long)tokens.size() ; it++ )
        strcat( TmpChar, tokens[it].c_str()) ;

    cerr << "**** probleme champ V X=D:M:S N/S D:M:S E/W : " << TmpChar << endl ;
    exit( 2 ) ;
    }


return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Trace un arc de cercle
long COpenAirFile::TraiteDBArcBetweenPoints(std::vector<std::string> & tokens)
{
char TmpChar[1000] = "" ;
// concatenation de la chaine, les 2 points
for ( long it = 1 ; it < (long)tokens.size() ; it++ )
    strcat( TmpChar, tokens[it].c_str()) ;

// decoupe des 2 points
std::string stringPt1 = strtok(TmpChar,",") ;
std::string stringPt2 = strtok(NULL,",") ;

// conversion en numerique des points debu/fin
std::vector<std::string> tokensPt1 ;
std::vector<std::string> tokensPt2 ;

tokensPt1.push_back("DP") ;
tokensPt1.push_back(stringPt1) ;

tokensPt2.push_back("DP") ;
tokensPt2.push_back(stringPt2) ;

CVecZoneReduce::st_coord_poly * pPts1 = new CVecZoneReduce::st_coord_poly ;
CVecZoneReduce::st_coord_poly * pPts2 = new CVecZoneReduce::st_coord_poly ;

TraiteDPPointDePolygone( pPts1, tokensPt1 ) ;
TraiteDPPointDePolygone( pPts2, tokensPt2 ) ;

// appel au module geodesique
Point center = { m_XCenterDaDbDc.m_Lat, m_XCenterDaDbDc.m_Lon }; // Issu de V X=
Point pStart = { pPts1->m_Lat, pPts1->m_Lon }; // Premier point du DB
Point pEnd   = { pPts2->m_Lat, pPts2->m_Lon }; // Second point du DB

// Précision de la discrétisation
int segments = 100;
// Par défaut dans OpenAir sauf si "V D=-"
bool sensHoraire = m_DClockwise ;

// calcul des points de l'arc
std::vector<Point> arc = computeOpenAirArc(center, pStart, pEnd, segments, sensHoraire);

// integartion des points de l'arc
for ( long ip = 0 ; ip < (long)arc.size() ; ip++ )
    {
    CVecZoneReduce::st_coord_poly * pPts = new CVecZoneReduce::st_coord_poly ;
    pPts->m_Lat = arc[ip].lat ;
    pPts->m_Lon = arc[ip].lon ;
    m_pZoneCur->m_VecPtsBig.push_back( pPts ) ;
    }

/*
double RadiusDeg = sqrt( pow( pPts2->m_Lat - Latm , 2. ) + pow( pPts2->m_Lon - Lonm , 2.) ) ;
// angles de depart et d'arrivee
double Angle1Rad = atan2( (pPts1->m_Lat - Latm) , (pPts1->m_Lon - Lonm) ) ;
double Angle2Rad = atan2( (pPts2->m_Lat - Latm) , (pPts2->m_Lon - Lonm) ) ;

// calcul du rayon du cercle
//double RadiusDeg = sqrt( pow( pPts2->m_Lat - m_XCenterDaDbDc.m_Lat , 2. ) + pow( pPts2->m_Lon - m_XCenterDaDbDc.m_Lon , 2.) ) ;
//double RadiusDeg1 = sqrt( pow( pPts1->m_Lat - m_XCenterDaDbDc.m_Lat , 2. ) + pow( pPts1->m_Lon - m_XCenterDaDbDc.m_Lon , 2.) ) ;
// angles de depart et d'arrivee
//double Angle1Rad = atan2( (pPts1->m_Lat - m_XCenterDaDbDc.m_Lat) , (pPts1->m_Lon - m_XCenterDaDbDc.m_Lon) ) ;
//double Angle2Rad = atan2( (pPts2->m_Lat - m_XCenterDaDbDc.m_Lat) , (pPts2->m_Lon - m_XCenterDaDbDc.m_Lon) ) ;
// pas de calcul
//double PasRadian = PAS_CERCLE_DEG*T_PI/180. ;
// angle couvert
double DiffAngle = Angle2Rad - Angle1Rad ;

// sens de rotation
if (m_DClockwise)
    {
    if (DiffAngle > 0) DiffAngle -= 2 * T_PI;
    }
else
    {
    // Sens anti-horaire (trigonométrique)
    if (DiffAngle < 0) DiffAngle += 2 * T_PI;
    }

double PasRadian = DiffAngle / 500. ;
//long AngleCount = DiffAngle / PasRadian ;

//m_pZoneCur->m_VecPtsBig.push_back( pPts1 ) ;
//for ( long is=0 ; is < (long)abs(AngleCount) ; is++ )
for ( long is=0 ; is < 500 ; is++ )
    {
    //double angleActuel = Angle1Rad + ((double)is) * DiffAngle / ((double)abs(AngleCount)) ;
    double angleActuel = Angle1Rad + ((double)is) * PasRadian ;
    CVecZoneReduce::st_coord_poly * pPts = new CVecZoneReduce::st_coord_poly ;
    pPts->m_Lat = RadiusDeg * sin( angleActuel ) + m_XCenterDaDbDc.m_Lat ;
    pPts->m_Lon = RadiusDeg * cos( angleActuel ) + m_XCenterDaDbDc.m_Lon ;
    m_pZoneCur->m_VecPtsBig.push_back( pPts ) ;
    }
//m_pZoneCur->m_VecPtsBig.push_back( pPts2 ) ;*/

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Trace un cercle. The radius is given in nautical miles.
/// soit en secondes.
long COpenAirFile::TraiteDCCercle(std::vector<std::string> & tokens)
{
double RadiusDeg=atof( tokens[1].c_str() ) ;
RadiusDeg /= 60. ;

if ( RadiusDeg == 0. )
    {
    char TmpChar[1000] = "" ;

    // concatenation de la chaine
    for ( long it = 0 ; it < (long)tokens.size() ; it++ )
        strcat( TmpChar, tokens[it].c_str()) ;

    cerr << "**** probleme champ DC Radius : " << TmpChar << endl ;
    exit( 1 ) ;
    }

// dessin du cercle
for ( double angle = 0 ; angle < 360. ; angle += PAS_CERCLE_DEG )
    {
    double AngleRadian = ((double)angle*T_PI/180.) ;
    CVecZoneReduce::st_coord_poly * pPts = new CVecZoneReduce::st_coord_poly ;
    pPts->m_Lat = RadiusDeg * sin( AngleRadian ) + m_XCenterDaDbDc.m_Lat ;
    pPts->m_Lon = RadiusDeg * cos( AngleRadian ) + m_XCenterDaDbDc.m_Lon ;
    m_pZoneCur->m_VecPtsBig.push_back( pPts ) ;
    }

return 0 ;
}


