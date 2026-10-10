////////////////////////////////////////////////////////////////////////////////
/// \file COpenAirFile.cpp
///
/// \brief
///
/// \date 10/10/2026 : creation
/// \date 10/10/2026 : modification
///

#include "../CompZoneAerienne.h"

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
    m_pZoneArr.push_back( m_pZoneCur ) ;
    TraiteACClasseDeLaZone( tokens ) ;
    }
// AN nom de zone
else if ( tokens[0] == "AN" )
    {
    TraiteANNomDeLaZone( tokens ) ;
    //cout << m_pZoneCur->m_Name << endl ;
    }
else if ( tokens[0] == "AY" )
    {}
else if ( tokens[0] == "AH" )
    TraiteAHAltitudeHaute( tokens ) ;
else if ( tokens[0] == "AL" )
    TraiteALAltitudeBasse( tokens ) ;
// DP point de zone
else if ( tokens[0] == "DP" )
    {
    CVecZoneReduce::st_coord_poly * pStPts = new CVecZoneReduce::st_coord_poly ;
    m_pZoneCur->m_VecPtsBig.push_back( pStPts ) ;

    TraiteDPPointDePolygone( pStPts, tokens ) ;

    //cout << m_pZoneCur->m_Name << endl ;
    //cout << m_pZoneCur->m_VecPtsBig.size() << endl ;
    //cout << m_pZoneArr.size() << endl ;
    }
// variable
else if ( tokens[0] == "V" )
    TraiteVVariableAssignement(tokens) ;
// arc de cercle
else if ( tokens[0] == "DB" )
    TraiteDBArcBetweenPoints(tokens) ;
// cercle
else if ( tokens[0] == "DC" )
    TraiteDCCercle(tokens) ;
// champs inconnu
else
    {
    cerr << "champ non connu : " << tokens[0] << endl ;
    exit( 1 ) ;
    }

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief classe A B C D E F G ZSM
long COpenAirFile::TraiteACClasseDeLaZone(std::vector<std::string> & tokens)
{
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
long COpenAirFile::TraiteANNomDeLaZone(std::vector<std::string> & tokens)
{
for ( long it = 1 ; it < (long)tokens.size() ; it++ )
    {
    m_pZoneCur->m_Name += tokens[it] ;
    m_pZoneCur->m_Name += " " ;
    }
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

// nord et est
bool Nord = strchr( TmpChar , 'N' ) != NULL ;
bool Sud = strchr( TmpChar , 'S' ) != NULL ;
bool Est = strchr( TmpChar , 'E' ) != NULL ;
bool Ouest = strchr( TmpChar , 'W' ) != NULL ;

// pour une separation d:m:s
if ( ! strchr( TmpChar , '.' ) )
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
else if ( strchr( TmpChar , ':' ) && strchr( TmpChar , '.' ) )
    {
    char Delimiter[] = ":NSEW " ;
    double Deg,Min ;

    // latitude
    Deg = atoi( strtok(TmpChar,Delimiter) ) ;
    Min = atof( strtok(NULL,Delimiter) ) ;

    pStPts->m_Lat = Deg + Min/60. ;

    if ( !Nord )
        {
        if ( !Sud )
            goto erreur ;
        pStPts->m_Lat *= -1 ;
        }

    // longitude
    Deg = atoi( strtok(NULL,Delimiter) ) ;
    Min = atof( strtok(NULL,Delimiter) ) ;

    pStPts->m_Lon = Deg + Min/60. ;

    if ( !Est )
        {
        if ( !Ouest )
            goto erreur ;
        pStPts->m_Lon *= -1 ;
        }
    }
else
    {
    erreur :
    cerr << "*** probleme DP " << TmpChar << endl ;
    exit( 1 ) ;
    }

return 0 ;
}
////////////////////////////////////////////////////////////////////////////////
/// \brief
long COpenAirFile::TraiteVVariableAssignement(std::vector<std::string> & tokens)
{
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
long COpenAirFile::TraiteDBArcBetweenPoints(std::vector<std::string> & tokens)
{
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
long COpenAirFile::TraiteDCCercle(std::vector<std::string> & tokens)
{
return 0 ;
}


