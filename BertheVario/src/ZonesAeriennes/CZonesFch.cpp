////////////////////////////////////////////////////////////////////////////////
/// \file CZonesFch.cpp
///
/// \brief
///
/// \date creation     : 30/03/2024
/// \date modification : 15/07/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Procedure de validation de zone fichier txt in/out.
void CZonesFch::Valid()
{
const int TaillMaxChar = 3000 ; // taille buffer en concurence memoire avec g_Termic
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier out
m_FileOut = SD.open(ZONE_VALID_OUT_FCH, FILE_WRITE , true );
if (!m_FileOut)
    {
    Serial.print( "erreur creation fichier ZONE_VALID_OUT_FCH" ) ;
    delete [] TmpChar ;
    return ;
    }
// debut de fichier
m_FileOut.seek(0) ;

// ouverture fichier in
m_File = SD.open(ZONE_VALID_IN_FCH);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading ZONE_VALID_IN_FCH");
    delete [] TmpChar ;
    return;
    }

// lecture fichier in
m_Ligne = 1 ;
while(m_File.available())
    {
    char c = m_File.read() ;
    if ( c != '\n' )
        {
        // si taille chaine maximum on empile plus dans le buffer
        if ( ic < TaillMaxChar )
            TmpChar[ic++] = c ;
        }
    else
        {
        // si fin de ligne on traite le buffer
        TmpChar[ic++] = 0 ;
        TraiteBufferValidZoneIn( TmpChar ) ;
        m_Ligne++ ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferValidZoneIn( TmpChar ) ;

m_File.close();
m_FileOut.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Procedure de validation de zone fichier txt in/out.
void CZonesFch::TraiteBufferValidZoneIn( char * buff )
{
if ( buff == NULL || *buff == 0 || *buff == '#' || *buff == '\n' )
    {
    *buff = 0 ;
    return ;
    }

// desactivation de toutes les zone activable
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer * pZone = m_ZonesArr[iz] ;
    if ( pZone->m_DansFchActivation )
        pZone->m_Activee = false ;
    }

char * pChar ;
std::string Nom = strtok( buff , ";" ) ;
std::string LonLat = strtok( NULL , ";" ) ;
std::string AnneMoisJour = strtok( NULL , ";" ) ;
std::string AltiMargexyMargealti = strtok( NULL , ";" ) ;
pChar = strtok( NULL , ";" ) ;
std::string ZonesActive ;
if ( pChar != NULL )
    ZonesActive = pChar ;

// si commantaire
if ( Nom[0] == '#' )
    {
    *buff = 0 ;
    return ;
    }

// pos lieux
g_GlobalVar.m_TerrainPosCur.m_Lon = atof( strtok( & LonLat[0] , "," ) ) ;
g_GlobalVar.m_TerrainPosCur.m_Lat = atof( strtok( NULL , "," ) ) ;

// date
std::string annee = strtok( & AnneMoisJour[0] , "," ) ;
std::string mois = strtok( NULL , "," ) ;
std::string jour = strtok( NULL , "," ) ;
g_GlobalVar.m_Annee = atoi( annee.c_str() ) ;
g_GlobalVar.m_Mois  = atoi( mois.c_str() ) ;
g_GlobalVar.m_Jour  = atoi( jour.c_str() ) ;
SetDatePeriode() ;

// alti, marge xy, marge z
g_GlobalVar.m_TerrainPosCur.m_AltiBaro = atoi( strtok( & AltiMargexyMargealti[0] , "," ) ) ;
pChar = strtok( NULL , "," ) ;
if ( pChar != NULL )
    g_GlobalVar.m_Config.m_XYMargin = atoi( pChar ) ;
pChar = strtok( NULL , "," ) ;
if ( pChar != NULL )
    g_GlobalVar.m_Config.m_AltiMargin = atoi( pChar ) ;

// positionnement zones actives
pChar = strtok( &ZonesActive[0] , "-,;" ) ;
while ( pChar != NULL )
    {
    // pour toutes les zones
    for ( long iz = 0 ; iz < m_NbZones ; iz++ )
        {
        const CZoneAer & Zone = *m_ZonesArr[iz] ;
        if ( strstr( Zone.m_NomOri.c_str() , pChar ) )
            {
            CZoneAer * pZone = m_ZonesArr[iz] ;

            pZone->m_Activee = true ;
            }
        }
    // prochaine zone active
    pChar = strtok( NULL , "-,;" ) ;
    }

CalcZone() ;

// ecriture numero de ligne
m_FileOut.print(m_Ligne) ;
m_FileOut.print(";") ;

// ecriture nom lieux
m_FileOut.print(Nom.c_str()) ;
m_FileOut.print(";") ;

// ecriture date
m_FileOut.print(annee.c_str()) ;
m_FileOut.print(",") ;
m_FileOut.print(mois.c_str()) ;
m_FileOut.print(",") ;
m_FileOut.print(jour.c_str()) ;
m_FileOut.print(";") ;

// ecriture plafond
m_FileOut.print(m_Plafond4Valid) ;
m_FileOut.print(";") ;

// ecriture alarm
if ( m_DansDessousUneZone == ZONE_DEDANS )
    {
    m_FileOut.print(m_NomZoneDansDessous.c_str()) ;
    m_FileOut.print(";") ;
    m_FileOut.print("DedansUneZone") ;
    m_FileOut.print(";") ;
    }
else if ( m_DansDessousUneZone == ZONE_LIMITE_ALTI )
    {
    m_FileOut.print(m_NomZoneDansDessous.c_str()) ;
    m_FileOut.print(";") ;
    m_FileOut.print("LimiteAltitude") ;
    m_FileOut.print(";") ;
    }
else if ( m_LimiteZone == ZONE_LIMITE_FRONTIERE )
    {
    m_FileOut.print(m_NomZoneEnLimite.c_str()) ;
    m_FileOut.print(";") ;
    m_FileOut.print("LimiteFrontiere") ;
    m_FileOut.print(";") ;
    }
else if ( m_DansDessousUneZone == ZONE_DESSOUS )
    {
    m_FileOut.print(m_NomZoneDansDessous.c_str()) ;
    m_FileOut.print(";") ;
    m_FileOut.print("DessousUneZone") ;
    m_FileOut.print(";") ;
    }
else if ( m_DansDessousUneZone == ZONE_DESSUS )
    {
    m_FileOut.print(m_NomZoneDansDessous.c_str()) ;
    m_FileOut.print(";") ;
    m_FileOut.print("DessusUneZone") ;
    m_FileOut.print(";") ;
    }
else if ( m_DansDessousUneZone == ZONE_EN_DEHORS && m_LimiteZone == ZONE_EN_DEHORS )
    {
    m_FileOut.print("-") ;
    m_FileOut.print(";") ;
    m_FileOut.print("AucuneAlarm") ;
    m_FileOut.print(";") ;
    }
else
    {
    m_FileOut.print("-") ;
    m_FileOut.print(";") ;
    m_FileOut.print("ErreurProgramme") ;
    m_FileOut.print(";") ;
    }

m_FileOut.println("") ;

*buff = 0 ;
}
