////////////////////////////////////////////////////////////////////////////////
/// \file CConfigFile.cpp
///
/// \brief
///
/// \date creation     : 08/03/2024
/// \date modification : 14/07/2024
///

#include "../BertheVario.h"


////////////////////////////////////////////////////////////////////////////////
/// \brief
CConfigFile::CConfigFile()
{
st_line * pLine ;

pLine = new st_line ;
pLine->m_NomVar = "[termic]" ;
pLine->m_pVar = (void*) & m_Termic  ;
pLine->m_Type = TYPE_VAR_BOOL ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[coef_filtre_alti_baro]" ;
pLine->m_pVar = (void*) & m_coef_filtre_alti_baro  ;
pLine->m_Type = TYPE_VAR_FLOAT ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[periode_integration_sec]" ;
pLine->m_pVar = (void*) & m_periode_integration_sec ;
pLine->m_Type = TYPE_VAR_INT ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[vitesse_igc_kmh]" ;
pLine->m_pVar = (void*) & m_vitesse_igc_kmh ;
pLine->m_Type = TYPE_VAR_INT ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[stab_gps_m]" ;
pLine->m_pVar = (void*) & m_stab_gps ;
pLine->m_Type = TYPE_VAR_INT ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[vz_seuil_haut]" ;
pLine->m_pVar = (void*) & m_vz_seuil_haut ;
pLine->m_Type = TYPE_VAR_FLOAT ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[vz_seuil_bas]" ;
pLine->m_pVar = (void*) & m_vz_seuil_bas ;
pLine->m_Type = TYPE_VAR_FLOAT ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[vz_seuil_max]" ;
pLine->m_pVar = (void*) & m_vz_seuil_max ;
pLine->m_Type = TYPE_VAR_FLOAT ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[dtu]" ;
pLine->m_pVar = (void*) & m_dtu  ;
pLine->m_Type =  TYPE_VAR_INT;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[marge_alti]" ;
pLine->m_pVar = (void*) & m_AltiMargin  ;
pLine->m_Type = TYPE_VAR_INT ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[marge_xy]" ;
pLine->m_pVar = (void*) & m_XYMargin  ;
pLine->m_Type = TYPE_VAR_INT ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[ssid_ru]" ;
pLine->m_pVar = (void*) & m_Ssid ;
pLine->m_Type = TYPE_VAR_STR ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[passwd_ru]" ;
pLine->m_pVar = (void*) & m_Passwd ;
pLine->m_Type = TYPE_VAR_STR ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[pilote_ru]" ;
pLine->m_pVar = (void*) & m_Pilote ;
pLine->m_Type = TYPE_VAR_STR ;
m_LinesVect.push_back( pLine ) ;

pLine = new st_line ;
pLine->m_NomVar = "[voile_ru]" ;
pLine->m_pVar = (void*) & m_Voile ;
pLine->m_Type = TYPE_VAR_STR ;
m_LinesVect.push_back( pLine ) ;
}


////////////////////////////////////////////////////////////////////////////////
/// \brief lit le fichier de configuration et reparti dans les variables
void CConfigFile::EcritureFichier()
{
// ouverture fichier
File file = SD.open(CONFIG_FCH, FILE_WRITE , true );
if (!file)
    {
    Serial.print( "erreur creation fichier " ) ;
    Serial.println( CONFIG_FCH ) ;
    return ;
    }

// debut de fichier
file.seek(0) ;

for( long iv = 0 ; iv < m_LinesVect.size() ; iv++ )
    {
    const st_line * pline = m_LinesVect[iv] ;

    file.print( pline->m_NomVar.c_str() ) ;
    file.print( " " ) ;
    switch ( pline->m_Type )
        {
        case TYPE_VAR_BOOL :
            {
            bool * pbool = (bool*) pline->m_pVar ;
            file.print( ((*pbool == true) ? "1" : "0" ) ) ;
            file.print( " #\n" ) ;
            }
            break ;

        case TYPE_VAR_INT :
            {
            int * pint = (int*) pline->m_pVar ;
            file.print( *pint ) ;
            file.print( " #\n" ) ;
            }
            break ;

        case TYPE_VAR_FLOAT :
            {
            float * pf = (float*) pline->m_pVar ;
            file.print( *pf ) ;
            file.print( " #\n" ) ;
            }
            break ;

        case TYPE_VAR_STR :
            {
            std::string ps = *(std::string*) pline->m_pVar ;
            ReplaceCharIn( ps , ' ' , '_' ) ;
            file.print( ps.c_str() ) ;
            file.print( " #\n" ) ;
            }
            break ;

        default :
            Serial.print( "erreur prg 1" ) ;
        }
    }

file.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CConfigFile::GetChar( int iVec , std::string & Name , std::string & Value ) const
{
if ( iVec < 0 || iVec >= m_LinesVect.size() )
    {
    Name = "-" ;
    Value = "-" ;
    return ;
    }

st_line * pLine = m_LinesVect[iVec] ;
Name = pLine->m_NomVar ;
char TmpChar[30] ;

// champs du fichier
switch ( pLine->m_Type )
    {
    case TYPE_VAR_BOOL :
            {
            bool * pbool = (bool*) pLine->m_pVar ;
            if ( *pbool )
                Value = "1" ;
            else
                Value = "0" ;
            }
            break ;

        case TYPE_VAR_INT :
            {
            int * pint = (int*) pLine->m_pVar ;
            sprintf( TmpChar , "%d" , *pint ) ;
            Value = TmpChar ;
            }
            break ;

        case TYPE_VAR_FLOAT :
            {
            float * pf = (float*) pLine->m_pVar ;
            sprintf( TmpChar , "%.2f" , *pf ) ;
            Value = TmpChar ;
            }
            break ;

        case TYPE_VAR_STR :
            {
            Value = *((std::string*) pLine->m_pVar) ;
            }
            break ;

        default :
            Serial.print( "erreur prg 3" ) ;
        }

}

////////////////////////////////////////////////////////////////////////////////
/// \brief lit le fichier de configuration et reparti dans les variables
void CConfigFile::LectureFichier()
{
char * TmpChar = new char [10000] ;
int ic = 0 ;

// ouverture fichier
File file = SD.open(CONFIG_FCH);
if ( !file )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(file.available())
    {
    TmpChar[ic++] = file.read();
    }
TmpChar[ic++] = 0 ;

file.close();

std::vector<char*> VecLigne ;

// decoupage en ligne
char * pChar = strtok( TmpChar , "\n" ) ;
while ( pChar != NULL )
    {
    // recopie
    char * pLigne = new char[1000] ;
    strcpy( pLigne , pChar ) ;
    VecLigne.push_back( pLigne ) ;
    pChar = strtok( NULL , "\n" ) ;
    }

// analyse des champs
char Separ[] = " #\t\n" ;
for ( int i = 0 ; i < VecLigne.size() ; i++ )
    {
    // ligne en cours
    char * pLigne = VecLigne[i] ;
    // decoupage des 2 premiers champs
    char * pNomParam = strtok( pLigne , Separ ) ;
    char * pValParam = strtok( NULL , Separ ) ;
    // si champs vide
    if ( pNomParam == NULL || pValParam == NULL || *pNomParam == 0 || *pValParam == 0 )
        continue ;
    if ( *pNomParam == '\n' || *pValParam == '\n' )
        continue ;
    // allocation ligne fichier
    st_line * pLine = NULL ;

    // recherche variable
    for ( int iv = 0 ; iv < m_LinesVect.size() ; iv++ )
        {
        st_line * pLineVect = m_LinesVect[iv] ;
        if ( !strcmp( pLineVect->m_NomVar.c_str() , pNomParam ) )
            {
            pLine = pLineVect ;
            break ;
            }
        }

    if ( pLine == NULL )
        continue ;

    // champs du fichier
    switch ( pLine->m_Type )
        {
        case TYPE_VAR_BOOL :
            {
            bool * pbool = (bool*) pLine->m_pVar ;
            if ( *pValParam == '1' )
                *pbool = true ;
            else
                *pbool = false ;
            Serial.print( pLine->m_NomVar.c_str() ) ;
            Serial.print( ":" ) ;
            Serial.println( *pbool ) ;
            }
            break ;

        case TYPE_VAR_INT :
            {
            int * pint = (int*) pLine->m_pVar ;
            *pint = atoi( pValParam ) ;
            Serial.print( pLine->m_NomVar.c_str() ) ;
            Serial.print( ":" ) ;
            Serial.println( *pint ) ;
            }
            break ;

        case TYPE_VAR_FLOAT :
            {
            float * pf = (float*) pLine->m_pVar ;
            *pf = atof( pValParam ) ;
            Serial.print( pLine->m_NomVar.c_str() ) ;
            Serial.print( ":" ) ;
            Serial.println( *pf ) ;
            }
            break ;

        case TYPE_VAR_STR :
            {
            std::string * ps = (std::string*) pLine->m_pVar ;
            *ps = pValParam ;
            ReplaceCharIn( *ps , '_' , ' ' ) ;
            Serial.print( pLine->m_NomVar.c_str() ) ;
            Serial.print( ":" ) ;
            Serial.println( ps->c_str() ) ;
            }
            break ;

        default :
            Serial.print( "erreur prg 2" ) ;
        }

    }

// liberation memoire
for ( int i = 0 ; i < VecLigne.size() ; i++ )
    delete [] VecLigne[i] ;

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Libere la memoire des lignes
void CConfigFile::FreeVect()
{
for ( int il = 0 ; il < m_LinesVect.size() ; il++ )
    delete m_LinesVect[il] ;
m_LinesVect.clear() ;
m_LinesVect.shrink_to_fit() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Remplacement de caratere
void CConfigFile::ReplaceCharIn( std::string & str , char cfind , char creplace )
{
for ( int i = 0 ; i < str.length() ; i++ )
    if ( str[i] == cfind )
        str[i] = creplace ;
}
