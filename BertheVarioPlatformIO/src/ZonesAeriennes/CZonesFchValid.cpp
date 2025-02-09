////////////////////////////////////////////////////////////////////////////////
/// \file CZonesFchValid.cpp
///
/// \brief
///
/// \date creation     : 30/03/2024
/// \date modification : 09/02/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Procedure de validation de zone fichier txt in/front/out.
void CZonesFchValid::Valid()
{
const int TaillMaxChar = 3000 ; // taille buffer en concurence memoire avec g_Termic
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier out valid
m_FileOutValid = SD.open(ZONE_VALID_OUT_FCH, FILE_WRITE , true );
if (!m_FileOutValid)
    {
    Serial.print( "erreur creation fichier ZONE_VALID_OUT_FCH" ) ;
    delete [] TmpChar ;
    return ;
    }
// debut de fichier
m_FileOutValid.seek(0) ;

// ouverture fichier out front
m_FileOutFront = SD.open(ZONE_FRONT_OUT_FCH, FILE_WRITE , true );
if (!m_FileOutFront)
    {
    Serial.print( "erreur creation fichier ZONE_FRONT_OUT_FCH" ) ;
    delete [] TmpChar ;
    return ;
    }
// debut de fichier
m_FileOutFront.seek(0) ;

// ouverture fichier in
File FileIn = SD.open(ZONE_VALID_IN_FCH);
if ( !FileIn )
    {
    Serial.println("Failed to open file for reading ZONE_VALID_IN_FCH");
    delete [] TmpChar ;
    return;
    }

// lecture fichier in
m_Ligne = 1 ;
while(FileIn.available())
    {
    char c = FileIn.read() ;
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

// fermeture des fichiers
FileIn.close();
m_FileOutValid.close();
m_FileOutFront.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Procedure de validation de zone fichier txt in/out.
void CZonesFchValid::TraiteBufferValidZoneIn( char * buff )
{
if ( buff == NULL || *buff == 0 || *buff == '#' || *buff == '\n' )
    {
    *buff = 0 ;
    return ;
    }

// desactivation de toutes les zone activable
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
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
    for ( int iz = 0 ; iz < m_NbZones ; iz++ )
        {
        const CZoneAer & Zone = *m_ZonesArr[iz] ;
        if ( ! strcmp( Zone.m_pNomAff , pChar ) )
            {
            CZoneAer * pZone = m_ZonesArr[iz] ;

            pZone->m_Activee = true ;
            }
        }
    // prochaine zone active
    pChar = strtok( NULL , "-,;" ) ;
    }

CalcZone() ;

//////////////////
// fichier zone in
// ecriture numero de ligne
m_FileOutValid.print(m_Ligne) ;
m_FileOutValid.print(";") ;

// ecriture nom lieux
m_FileOutValid.print(Nom.c_str()) ;
m_FileOutValid.print(";") ;

// ecriture date
m_FileOutValid.print(annee.c_str()) ;
m_FileOutValid.print(",") ;
m_FileOutValid.print(mois.c_str()) ;
m_FileOutValid.print(",") ;
m_FileOutValid.print(jour.c_str()) ;
m_FileOutValid.print(";") ;

// ecriture plafond
m_FileOutValid.print(m_Plafond4Valid) ;
m_FileOutValid.print(";") ;

// ecriture alarm
if ( m_DansDessousUneZone == ZONE_DEDANS )
    {
    m_FileOutValid.print(m_NomZoneDansDessous.c_str()) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("DedansUneZone") ;
    m_FileOutValid.print(";") ;
    }
else if ( m_LimiteZone == ZONE_LIMITE_ALTI )
    {
    m_FileOutValid.print(m_NomZoneEnLimite.c_str()) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("LimiteAltitude") ;
    m_FileOutValid.print(";") ;
    }
else if ( m_LimiteZone == ZONE_LIMITE_FRONTIERE )
    {
    m_FileOutValid.print(m_NomZoneEnLimite.c_str()) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("LimiteFrontiere") ;
    m_FileOutValid.print(";") ;
    }
else if ( m_DansDessousUneZone == ZONE_DESSOUS )
    {
    m_FileOutValid.print(m_NomZoneDansDessous.c_str()) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("DessousUneZone") ;
    m_FileOutValid.print(";") ;
    }
else if ( m_DansDessousUneZone == ZONE_DESSUS )
    {
    m_FileOutValid.print(m_NomZoneDansDessous.c_str()) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("DessusUneZone") ;
    m_FileOutValid.print(";") ;
    }
else if ( m_DansDessousUneZone == ZONE_EN_DEHORS && m_LimiteZone == ZONE_EN_DEHORS )
    {
    m_FileOutValid.print("-") ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("AucuneAlarm") ;
    m_FileOutValid.print(";") ;
    }
else
    {
    m_FileOutValid.print("-") ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("ErreurProgramme") ;
    m_FileOutValid.print(";") ;
    }

m_FileOutValid.println("") ;

/////////////////////
// fichier zone front
// ecritude numero de ligne
m_FileOutFront.print(m_Ligne) ;
m_FileOutFront.print("\t") ;

// ecriture nom lieux
m_FileOutFront.print(Nom.c_str()) ;
m_FileOutFront.print(";") ;
// ecriture date
m_FileOutFront.print(annee.c_str()) ;
m_FileOutFront.print(",") ;
m_FileOutFront.print(mois.c_str()) ;
m_FileOutFront.print(",") ;
m_FileOutFront.print(jour.c_str()) ;
m_FileOutFront.print(";") ;
// ecriture plafond
m_FileOutFront.print((int)g_GlobalVar.m_TerrainPosCur.m_AltiBaro) ;
m_FileOutFront.print("\t") ;

// si pas d'alarmes
if ( (m_DansDessousUneZone == ZONE_EN_DEHORS || m_DansDessousUneZone == ZONE_DESSOUS ) && m_LimiteZone == ZONE_EN_DEHORS )
    {
    // reste alti
    m_FileOutFront.print( ( m_DistAltCurZone >= 999 ) ? 999 : m_DistAltCurZone ) ;
    m_FileOutFront.print("A\t") ;

    // reste distance
    m_FileOutFront.print( ( m_DistXYNextZone >= 999 ) ? 999 : m_DistXYNextZone ) ;
    m_FileOutFront.print("\t") ;

    // cap point front
    char TmpNomCap[5] ;
    CGestEcrans::GetCapChar( (m_DistXYNextZone>= 999) ? -1 : m_CapFrontProche , TmpNomCap ) ;
    m_FileOutFront.print(TmpNomCap) ;
    }
else
    m_FileOutFront.print( "Alarme zone penetration/limite" ) ;

m_FileOutFront.println("") ;

*buff = 0 ;
}
