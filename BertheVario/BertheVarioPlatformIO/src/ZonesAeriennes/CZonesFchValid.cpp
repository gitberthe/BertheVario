////////////////////////////////////////////////////////////////////////////////
/// \file CZonesFchValid.cpp
///
/// \brief
///
/// \date creation     : 30/03/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date modification : 17/09/2025
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
for ( int iz = 0 ; iz < GetSize() ; iz++ )
    {
    CZoneAer * pZone = (*this)[iz] ;
    if ( pZone->m_DansFchActivation )
        pZone->m_Activee = false ;
    }

char * pChar ;
CLGString Nom = strtok( buff , ";" ) ;
CLGString LonLat = strtok( NULL , ";" ) ;
CLGString AnneMoisJour = strtok( NULL , ";" ) ;
CLGString AltiMargexyMargealti = strtok( NULL , ";" ) ;
pChar = strtok( NULL , ";" ) ;
CLGString ZonesActive ;
if ( pChar != NULL )
    ZonesActive = pChar ;

// si commantaire
if ( Nom[0] == '#' )
    {
    *buff = 0 ;
    return ;
    }

// pos lieux
g_GlobalVar.m_TerrainPosCur.m_Lon = atof( strtok( LonLat.GetBuffer() , "," ) ) ;
g_GlobalVar.m_TerrainPosCur.m_Lat = atof( strtok( NULL , "," ) ) ;

// date
CLGString annee = strtok( AnneMoisJour.GetBuffer() , "," ) ;
CLGString mois = strtok( NULL , "," ) ;
CLGString jour = strtok( NULL , "," ) ;
g_GlobalVar.m_Annee = atoi( annee ) ;
g_GlobalVar.m_Mois  = atoi( mois ) ;
g_GlobalVar.m_Jour  = atoi( jour ) ;
SetDatePeriode() ;

// alti, marge xy, marge z
g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec = atoi( strtok( AltiMargexyMargealti.GetBuffer() , "," ) ) ;
/*pChar = strtok( NULL , "," ) ;
if ( pChar != NULL )
    g_GlobalVar.m_Config.m_XYMargin = atoi( pChar ) ;
pChar = strtok( NULL , "," ) ;
if ( pChar != NULL )
    g_GlobalVar.m_Config.m_AltiMargin = atoi( pChar ) ;*/

// positionnement zones actives
pChar = strtok( ZonesActive.GetBuffer() , "-,;" ) ;
while ( pChar != NULL )
    {
    // pour toutes les zones
    for ( int iz = 0 ; iz < GetSize() ; iz++ )
        {
        const CZoneAer & Zone = *(*this)[iz] ;
        if ( ! strcmp( Zone.m_pNomAff , pChar ) )
            {
            CZoneAer * pZone = (*this)[iz] ;

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
m_FileOutValid.print(Nom) ;
m_FileOutValid.print(";") ;

// ecriture date
m_FileOutValid.print(annee) ;
m_FileOutValid.print(",") ;
m_FileOutValid.print(mois) ;
m_FileOutValid.print(",") ;
m_FileOutValid.print(jour) ;
m_FileOutValid.print(";") ;

// ecriture plafond
m_FileOutValid.print(m_Plafond4Valid) ;
m_FileOutValid.print(";") ;

// ecriture alarm
if ( m_DansDessousUneZone == ZONE_DEDANS )
    {
    m_FileOutValid.print((const char*)m_NomZoneDansDessous) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("DedansUneZone") ;
    m_FileOutValid.print(";") ;
    }
/*else if ( m_LimiteZone == ZONE_LIMITE_ALTI )
    {
    m_FileOutValid.print(m_NomZoneEnLimite) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("LimiteAltitude") ;
    m_FileOutValid.print(";") ;
    }
else if ( m_LimiteZone == ZONE_LIMITE_FRONTIERE )
    {
    m_FileOutValid.print(m_NomZoneEnLimite) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("LimiteFrontiere") ;
    m_FileOutValid.print(";") ;
    } */
else if ( m_DansDessousUneZone == ZONE_DESSOUS )
    {
    m_FileOutValid.print((const char*)m_NomZoneDansDessous) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("DessousUneZone") ;
    m_FileOutValid.print(";") ;
    }
else if ( m_DansDessousUneZone == ZONE_DESSUS )
    {
    m_FileOutValid.print((const char*)m_NomZoneDansDessous) ;
    m_FileOutValid.print(";") ;
    m_FileOutValid.print("DessusUneZone") ;
    m_FileOutValid.print(";") ;
    }
else if ( m_DansDessousUneZone == ZONE_EN_DEHORS /*&& m_LimiteZone == ZONE_EN_DEHORS */ )
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
m_FileOutFront.print(Nom) ;
m_FileOutFront.print(";") ;
// ecriture date
m_FileOutFront.print(annee) ;
m_FileOutFront.print(",") ;
m_FileOutFront.print(mois) ;
m_FileOutFront.print(",") ;
m_FileOutFront.print(jour) ;
m_FileOutFront.print(";") ;
// ecriture plafond
m_FileOutFront.print((int)g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec) ;
m_FileOutFront.print("\t") ;

/*
// si pas d'alarmes
if ( (m_DansDessousUneZone == ZONE_EN_DEHORS || m_DansDessousUneZone == ZONE_DESSOUS ) && m_LimiteZone == ZONE_EN_DEHORS  )
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
    m_FileOutFront.print( "Alarme zone penetration/limite" ) ; */

m_FileOutFront.println("") ;

*buff = 0 ;
}
