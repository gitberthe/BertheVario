////////////////////////////////////////////////////////////////////////////////
/// \file CZonesAerAll.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 12/02/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
CZonesAerAll::CZonesAerAll()
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Detruit toutes les zones; libere de la place pour le file manager qui
/// bloque autrement.
void CZonesAerAll::DeleteAll()
{
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer * pZoneAer = m_ZonesArr[iz] ;
    delete pZoneAer ;
    }

m_NbZones = 0 ;
delete [] m_ZonesArr ;
m_ZonesArr = NULL ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne l'altitude des zones suivant leur periode et la date courante.
void CZonesAerAll::SetDatePeriode()
{
// si date pas encore acquise
if ( g_GlobalVar.m_Annee < 2010 )
    return ;

SetDateTU( g_GlobalVar.m_Annee, g_GlobalVar.m_Mois, g_GlobalVar.m_Jour, 0. ) ;
bool WeekEnd = IsWeekEnd() ;
float JDCur = GetJD_TU() ;

/*Serial.print( g_GlobalVar.m_Annee ) ;
Serial.print( g_GlobalVar.m_Mois ) ;
Serial.println( "CZonesAerAll::SetDatePeriode()" ) ;*/

// pour toutes les zones
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer & ZoneAer = *m_ZonesArr[iz] ;

    // si zone sans periode
    if ( ZoneAer.m_pDerogFfvl == NULL )
        continue ;

/*Serial.print( g_GlobalVar.m_Annee ) ;
Serial.print( g_GlobalVar.m_Mois ) ;
Serial.println( "CZonesAerAll::SetDatePeriode()" ) ;*/

    // test si dans periode
    ZoneAer.m_pDerogFfvl->m_AltiAPrendreEnCompte = ALTI_BASSE ;
    CDate DateDebut( g_GlobalVar.m_Annee , ZoneAer.m_pDerogFfvl->m_PeriodeDebutMois , ZoneAer.m_pDerogFfvl->m_PeriodeDebutJour , 0. ) ;
    CDate DateDefin( g_GlobalVar.m_Annee , ZoneAer.m_pDerogFfvl->m_PeriodeFinMois ,   ZoneAer.m_pDerogFfvl->m_PeriodeFinJour , 24. ) ;
    if ( (DateDebut.GetJD_TU() <= JDCur) && (JDCur <= DateDefin.GetJD_TU()) )
        {
        // positionnement alti fonction periode
        if ( WeekEnd )
            ZoneAer.m_pDerogFfvl->m_AltiAPrendreEnCompte = ALTI_PERIODE_WEEKEND ;
        else
            ZoneAer.m_pDerogFfvl->m_AltiAPrendreEnCompte = ALTI_PERIODE_SEMAINE ;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture des 3 fichier, zones, activation et periode.
void CZonesAerAll::LectureFichiers()
{
// ouverture fichier out nom zone
m_FileNomZoneOut = SD.open(NOM_FCH_OUT_NOM_ZONE, FILE_WRITE , true );
if (!m_FileNomZoneOut)
    Serial.print( "erreur creation fichier NOM_FCH_OUT_NOM_ZONE" ) ;
m_FileNomZoneOut.seek(0) ;

// traitement des zones
LectureFichierZonesAer() ;

// fermeture fichier nom zone
m_FileNomZoneOut.close() ;

LectureFichierZonesActive() ;
LectureFichierZonesPeriode() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture du fichier zonesaer.txt
void CZonesAerAll::LectureFichierZonesAer()
{
const int TaillMaxChar = 20000 ; // taille buffer en concurence memoire avec g_Termic
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
File FileZone = SD.open(NOM_FCH_ZONE_AER);
if ( !FileZone )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(FileZone.available())
    {
    char c = FileZone.read() ;
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

        // traitement du buffer si memoire 30k free mem
        if ( esp_get_free_heap_size() > 30*1024 )
            {
            CZoneAer * pZone = TraiteBufferZoneAer( TmpChar ) ;
            if ( pZone != NULL )
                {
                pZone->CompressZone() ;
                pZone->FreeFloat() ;
                }
            }
        else
            g_GlobalVar.BeepError() ;

        ic = 0 ;
        }
    }

// pour la derniere zone
CZoneAer * pZone = TraiteBufferZoneAer( TmpChar ) ;
if ( pZone != NULL )
    {
    pZone->CompressZone() ;
    pZone->FreeFloat() ;
    }

FileZone.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction traite une ligne texte de zone et la met sous forme binaire
/// dans le tableau m_ZonesArr.
/// Reduction de zone par CVecReduce.
/// Calcul aussi le barycentre et le rayon max de la zone pour une recheche rapide.
CZoneAer * CZonesAerAll::TraiteBufferZoneAer( char * buff )
{

// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' )
    {
    *buff = 0 ;
    return NULL ;
    }

// on ne prend pas en compte les zone 1.20, 5.20, 4.20...
//if ( strstr( pChar , ".20" ) )
//    return ;

// nouvelle zone
CZoneAer * pZone = new CZoneAer ;

// allocation nouveau tableau de zones plus grand et recopie de l'ancien contenu
if ( m_ZonesArr == NULL )
    {
    m_ZonesArr = new CZoneAer * [1] ;
    m_NbZones = 1 ;
    }
else
    {
    CZoneAer ** pOldArr = m_ZonesArr ;
    m_ZonesArr = new CZoneAer * [++m_NbZones] ;
    memcpy( m_ZonesArr , pOldArr , (m_NbZones-1) * sizeof(CZoneAer *) ) ;
    delete [] pOldArr ;
    }
// ajout de la nouvelle zone au tableau
m_ZonesArr[m_NbZones-1] = pZone ;

// zone protege PROTECT ou FFVL-Prot dans chaine
bool IsProtect = ( (*pChar == 'P') && (strstr( pChar , "PROTECT" ) != NULL)) ||
                 ( (*pChar == 'F') && (strstr( pChar , "FFVL-Prot" ) != NULL)) ;
if ( IsProtect )
    {
    // si m/sol trouve
    char * pCharFin = strstr( pChar , "m/sol" ) ;
    if ( pCharFin != NULL )
        {
        char TmpBuff[500] ;
        strcpy( TmpBuff , pChar ) ;
        *pCharFin = 0 ;
        // on recule jusqu'a l'espace
        while( *pCharFin != ' ' && *pCharFin != '(' && pCharFin != pChar )
            pCharFin-- ;
        // on ravance d'un coup
        pCharFin++ ;
        // hauteur sol
        pZone->m_HauteurSolZoneProtege = atoi( pCharFin ) ;
        // si probleme de conversion hauteur max
        if ( pZone->m_HauteurSolZoneProtege == 0 )
            pZone->m_HauteurSolZoneProtege = 1000 ;
        }
    // 1000 metres par defaut
    else
        pZone->m_HauteurSolZoneProtege = 1000 ;
    }

// pour controle nom zone
std::string NomOri = pChar ;

// zone protegee on enleve des chaines
int iespacemax = 3 ;
char NomRND[]  = "Reserve naturelle nationale de " ;
char NomRNDV[] = "Reserve naturelle nationale de la Vallee de " ;
char NomProtect[] = "PROTECT " ;
char NomFFVL[] = "FFVL-Prot " ;
// zone "FFVL"
if ( (pChar[0] == 'F') && strstr( pChar , NomFFVL ) )
    pChar += strlen( NomFFVL ) ;
// zone "PROTECT"
if ( (pChar[0] == 'P') && strstr( pChar , NomProtect ) )
    pChar += strlen( NomProtect ) ;
// reserve naturelle nationale de la vallee
char * pTmpChar = strstr( pChar , NomRNDV ) ;
if ( pTmpChar != NULL )
    {
    pChar += strlen( NomRNDV ) ;
    iespacemax = 1 ;
    }
// reserve naturelle nationale de
pTmpChar = strstr( pChar , NomRND ) ;
if ( pTmpChar != NULL )
    {
    pChar += strlen( NomRND ) ;
    iespacemax = 1 ;
    }
std::string NomAff = pChar ;

// formattage nom de zone 3 champs
int iespace = 0 ;
for ( int ic = 0 ; ic < NomAff.size() ; ic++ )
    {
    // premier (
    if ( NomAff[ic] == '(' )
        {
        NomAff.resize( ic ) ;
        break ;
        }
    // trois champs max
    else if ( (NomAff[ic] == ' ' || NomAff[ic] == '-') && ++iespace >= iespacemax )
        {
        NomAff.resize( ic ) ;
        break ;
        }
    }
// on eneleve le dernier blanc
if ( NomAff[NomAff.size()-1] == ' ' )
    NomAff.resize(NomAff.size()-1) ;

pZone->m_pNomAff = new char [ NomAff.size() + 1 ] ;
strcpy( pZone->m_pNomAff , NomAff.c_str() ) ;

// affichage nom zone pour controle
m_FileNomZoneOut.print( NomAff.c_str() ) ;
m_FileNomZoneOut.print( " <= " ) ;
m_FileNomZoneOut.println( NomOri.c_str() ) ;

// altitude de basse zone
pChar = strtok( NULL , ";" ) ;
pZone->m_AltiBasse = atoi( pChar ) ;

// ajout des points du polygone
pChar = strtok( NULL , ";," ) ;
std::vector<CZoneAer::st_coord_poly*> VecPoly ;
// reserve pour une grande zone
VecPoly.reserve( 2000 ) ;
while ( pChar != NULL )
    {
    // nouveau point
    CZoneAer::st_coord_poly * ppts = new CZoneAer::st_coord_poly ;

    // longitude
    ppts->m_Lon = atof( pChar ) ;
    pChar = strtok( NULL , ";," ) ;

    // latitude
    ppts->m_Lat = atof( pChar ) ;
    pChar = strtok( NULL , ";," ) ;

    // ajout au vecteur
    VecPoly.push_back( ppts ) ;

    //Serial.println( m_NbZones ) ;
    //Serial.println( esp_get_free_heap_size() ) ;
    }

/*CVecReduce VecReduce ;
VecReduce.Set( VecPoly ) ;
VecReduce.ReduceToDistance( DIST_METRE_4_ZONE ) ;
VecReduce.ReduceToAngle( ANGLE_DEGRES_4_ZONE ) ;*/

/*// pour gnuplot
Serial.println( "********************" ) ;
for ( int iv = 0 ; iv < VecPoly.size() ; iv ++ )
    {
    Serial.print(VecPoly[iv]->m_Lon , 5 ) ;
    Serial.print( " " ) ;
    Serial.println(VecPoly[iv]->m_Lat , 5 ) ;
    }
Serial.println( "********************" ) ; //*/

// recopie du vecteur de points vers le tableau de points
pZone->m_PolygoneArr = new CZoneAer::st_coord_poly * [ VecPoly.size() ] ;
memcpy( pZone->m_PolygoneArr , & VecPoly[0] , VecPoly.size() * sizeof( CZoneAer::st_coord_poly*) ) ;
pZone->m_NbPts = VecPoly.size() ;

// calcul de la surface et du barycentre
//pZone->m_Area = CPolygone::GetAreaSize( pZone->m_PolygoneArr , pZone->m_NbPts , pZone->m_Barycentre ) ;
CPolygone::CalcBarycentre( pZone->m_PolygoneArr , pZone->m_NbPts , pZone->m_Barycentre ) ;

// calcul du rayon max
pZone->m_RayonMetre = 0. ;
for ( int is = 0 ; is < pZone->m_NbPts ; is++ )
    {
    const CZoneAer::st_coord_poly & PtsCour = *pZone->m_PolygoneArr[is] ;
    float dist = sqrtf( powf(pZone->m_Barycentre.m_Lat-PtsCour.m_Lat,2) + powf(pZone->m_Barycentre.m_Lon-PtsCour.m_Lon,2) ) * 60. * UnMileEnMetres  ;
    if ( dist > pZone->m_RayonMetre )
        pZone->m_RayonMetre = dist ;
    }

// pour prochain appel, buffer vide
*buff = 0 ;

return pZone ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture fichier periode de zone
void CZonesAerAll::LectureFichierZonesPeriode()
{
const int TaillMaxChar = 1000 ;
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
File FilePeriode = SD.open(NOM_FCH_ZONE_PER);
if ( !FilePeriode )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(FilePeriode.available())
    {
    char c = FilePeriode.read() ;
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
        TraiteBufferZonePeriode( TmpChar ) ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferZonePeriode( TmpChar ) ;

FilePeriode.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite ligne periode de zone
void CZonesAerAll::TraiteBufferZonePeriode( char * buff )
{
// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' || *pChar == '\n' )
    {
    *buff = 0 ;
    return ;
    }

std::string NomOri = pChar ;
//NomOri += ' ' ;     // caractere de fin de champ pour recherche sans bug
std::string NomAff = strtok( NULL ,";");
std::string PeriodeDeb = strtok( NULL ,";");
std::string PeriodeFin = strtok( NULL ,";");
std::string AltiSemaine = strtok( NULL ,";");
std::string AltiWeekEnd = strtok( NULL ,";");

CZoneAer * pZone = Find( NomOri.c_str() ) ;

// si pas de zone
if ( pZone == NULL )
    {
    Serial.println("zone aerienne non trouvée periode") ;
    *buff = 0 ;
    return ;
    }

// recopie nom pour affiche
if ( NomAff != "-" && NomAff != "" )
    {
    delete [] pZone->m_pNomAff ;
    pZone->m_pNomAff = new char [ NomAff.size() + 1 ] ;
    strcpy( pZone->m_pNomAff , NomAff.c_str() ) ;
    }
/*Serial.print(pZone->m_NomOri.c_str()) ;
Serial.print("|") ;
Serial.println(pZone->m_pNomAff) ;*/

// periode debut
if ( PeriodeDeb != "-" && PeriodeDeb != "" )
    {
    if ( pZone->m_pDerogFfvl == NULL )
        pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
    std::string Jour = strtok( & PeriodeDeb[0] , "-" ) ;
    std::string Mois = strtok( NULL , "-" ) ;
    pZone->m_pDerogFfvl->m_PeriodeDebutJour = atoi( Jour.c_str() ) ;
    pZone->m_pDerogFfvl->m_PeriodeDebutMois = atoi( Mois.c_str() ) ;
    }

// periode fin
if ( PeriodeFin != "-" && PeriodeFin != "" )
    {
    if ( pZone->m_pDerogFfvl == NULL )
        pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
    std::string Jour = strtok( & PeriodeFin[0] , "-" ) ;
    std::string Mois = strtok( NULL , "-" ) ;
    pZone->m_pDerogFfvl->m_PeriodeFinJour = atoi( Jour.c_str() ) ;
    pZone->m_pDerogFfvl->m_PeriodeFinMois = atoi( Mois.c_str() ) ;
    }

// alti semaine
if ( AltiSemaine != "-" && AltiSemaine != "" )
    {
    if ( pZone->m_pDerogFfvl == NULL )
        pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
    std::string Alti = strtok( & AltiSemaine[0] , "-" ) ;
    pZone->m_pDerogFfvl->m_AltiBassePeriodeSemaine = atoi( Alti.c_str() ) ;
    }

// alti week-end
if ( AltiWeekEnd != "-" && AltiWeekEnd != "" )
    {
    if ( pZone->m_pDerogFfvl == NULL )
        pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
    std::string Alti = strtok( & AltiWeekEnd[0] , "-" ) ;
    pZone->m_pDerogFfvl->m_AltiBassePeriodeWeekEnd = atoi( Alti.c_str() ) ;
    }

// pour prochain appel
*buff = 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture fichier activation de zone
void CZonesAerAll::EcritureFichierZonesActive()
{

File Fch ;

// ouverture fichier
Fch = SD.open(NOM_FCH_ZONE_ACT, FILE_WRITE , true );
if (!Fch)
    {
    Serial.print( "erreur creation fichier zone actice" ) ;
    return ;
    }

// debut de fichier
Fch.seek(0) ;

std::vector<std::string> VecNomDejaEcrit ;

// ecriture des zone
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
    {
    // zone en memoire
    const CZoneAer * pZone = m_ZonesArr[iz] ;
    if ( !pZone->m_DansFchActivation )
        continue ;

    // verif si pas deja fait
    bool DejaFait = false ;
    for ( long iv = 0 ; iv < VecNomDejaEcrit.size() ; iv++ )
        {
        if ( VecNomDejaEcrit[iv] == pZone->m_pNomAff )
            {
            DejaFait = true ;
            break ;
            }
        }

    // si deja ecrit dans le fichier
    if ( DejaFait )
        continue ;

    // empilement pour suivit deja fait
    VecNomDejaEcrit.push_back( pZone->m_pNomAff ) ;

    // ecriture fichier
    Fch.print( pZone->m_pNomAff ) ;
    Fch.print( ";" ) ;
    if ( pZone->m_Activee )
        Fch.print( "1" ) ;
    else
        Fch.print( "0" ) ;
    Fch.println( ";" ) ;
    }

Fch.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture fichier activation de zone
void CZonesAerAll::LectureFichierZonesActive()
{
const int TaillMaxChar = 1000 ;
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
File FileAct = SD.open(NOM_FCH_ZONE_ACT);
if ( !FileAct )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(FileAct.available())
    {
    char c = FileAct.read() ;
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
        TraiteBufferZoneActive( TmpChar ) ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferZoneActive( TmpChar ) ;

FileAct.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite ligne activation zone.
void CZonesAerAll::TraiteBufferZoneActive( char * buff )
{
// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' || *pChar == '\n' )
    return ;

// nom de zone
std::string NomOri = pChar ;

// activation de zone
char * pActivation = strtok( NULL ,";") ;
if ( pActivation == NULL )
    return ;
std::string Activation = pActivation ;

// pour toutes les zones
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    const CZoneAer & Zone = *m_ZonesArr[iz] ;
    if ( NomOri == Zone.m_pNomAff )
        {
        CZoneAer * pZone = m_ZonesArr[iz] ;

        // activation
        if ( Activation == "0" )
            pZone->m_Activee = false ;
        else
            pZone->m_Activee = true ;

        pZone->m_DansFchActivation = true ;
        }
    }

*buff = 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Cherche la zone dans le tableau par rapport a son nom d'origine 3 champs.
CZoneAer * CZonesAerAll::Find( const char * NomOri )
{
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    const CZoneAer & Zone = *m_ZonesArr[iz] ;
    if ( ! strcmp( Zone.m_pNomAff , NomOri ) )
        return m_ZonesArr[iz] ;
    }

return NULL ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri croissant de nom pour ecran gestion zone
void CZonesAerAll::TriZonesNom()
{
CSortArray::m_pArr = m_ZonesArr ;
CSortArray::m_Size = m_NbZones ;
CZoneAer::TrierPar( CZoneAer::TriParNom ) ;
TriSchellMetzner() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction de tri de vector.
bool MySortFunction(const CZoneAer *p1,const CZoneAer *p2)
{
return ((*p1)<(*p2));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par altitude croissante des zones dans le vecteur indifferement
/// d'autres parametres
void CZonesAerAll::TriZonesParAltitude( std::vector<const CZoneAer *> & VecZones )
{
/*CSortArray::m_pArr = & VecZones[0] ;
CSortArray::m_Size = VecZones.size() ;
CZoneAer::TriParNom( false ) ;
TriSchellMetzner(true) ; */
CZoneAer::TrierPar( CZoneAer::TriParAltitude ) ;
std::sort(VecZones.begin(), VecZones.end(),MySortFunction);
//std::reverse(VecZones.begin(),VecZones.end());
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur aux zones necessaires en prenant en compte la derogation
/// et l'activabilite de la zone
void CZonesAerAll::ClasseZoneRetenueAltitude( std::vector<const CZoneAer *> & VecZonesIn )
{
std::vector<const CZoneAer *> VecZonesRet ;

// tri par altitude
TriZonesParAltitude( VecZonesIn ) ;

// tri suivant la derogation
for ( size_t iz = 0 ; iz < VecZonesIn.size()  ; iz++ )
    {
    // zone activable pris en compte apres
    if ( VecZonesIn[iz]->m_DansFchActivation )
        continue ;
    // si zone avec derogation, on efface les autres avant
    if ( VecZonesIn[iz]->HavePeriod() )
        VecZonesRet.clear() ;
    // ajout de la zone
    VecZonesRet.push_back( VecZonesIn[iz] ) ;
    }

// rajout des zones activables
for ( size_t iz = 0 ; iz < VecZonesIn.size()  ; iz++ )
    if ( VecZonesIn[iz]->m_DansFchActivation )
        VecZonesRet.push_back( VecZonesIn[iz] ) ;

// recopie des zones
VecZonesIn = VecZonesRet ;

// tri par altitude
TriZonesParAltitude( VecZonesIn ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul si l'on est dans une zone avec le nom de la zone et son altitude
/// \param m_NomZone [out] : nom de la zone en presence ou proche
/// \param m_DansDessousUneZone [out] : indique si on est dans ou au dessous d'une zone
/// \param m_NomZoneDansDessous [out] :  nom de la zone de laquelle on est dedans ou endessous
/// \param m_LimiteZone [out] : si en limite de zone
/// \param m_NomZoneEnLimite [out] : nom de la zone en limite
void CZonesAerAll::CalcZone()
{
m_Plafond4Valid = 100000 ;
std::string RetStrIn = "" ;
int         RetNbrIn = ZONE_EN_DEHORS ;
std::string RetStrLimite = "" ;
int         RetNbrLimite = ZONE_EN_DEHORS ;
char        TmpChar[100] ;

// test dans la zone, position courante
CZoneAer::st_coord_poly PtsEnCours ;
PtsEnCours.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
PtsEnCours.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon ;

#ifdef TMA_DEBUG
SetDate( 2024 , 3, 31 ) ;
//SetDate( 2024 , 2, 25 ) ;
g_GlobalVar.m_TerrainPosCur.m_AltiBaro = 1500 ;
g_GlobalVar.m_Config.m_XYMargin = 3000. ;

//PtsEnCours.m_Lat = 45.773785;   // pdd
//PtsEnCours.m_Lon = 2.96898;   // pdd
//PtsEnCours.m_Lat = 45.537966;   // saurier
//PtsEnCours.m_Lon = 3.04627;   // saurier
PtsEnCours.m_Lat = 45.331671;   // anzat-le-luguet
PtsEnCours.m_Lon = 3.041811;   // anzat-le-luguet

//PtsEnCours.m_Lat = 45.52730;   // sancy zone protege
//PtsEnCours.m_Lon = 2.81738;   // sancy zone protege

#endif

// si test wifi positionnement altitude sol
if ( g_GlobalVar.m_ModeHttp )
    g_GlobalVar.m_AltitudeSolHgt = g_GlobalVar.m_Hgt2Agl.GetGroundZ( g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_Lat ) ;

// pour toutes les zones, determination si dans surface
std::vector<const CZoneAer *> VecZoneInArea ;   // zones normale comme TMA et RTBA
const CZoneAer * pZoneProtegee = NULL ;             // zone protege
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer & Zone = *m_ZonesArr[iz] ;

    // si la zone n'est pas active
    if ( !Zone.m_Activee )
        continue ;

    // dans le du rayon
    float DistBaryMetres = sqrtf( powf(Zone.m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(Zone.m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayon = DistBaryMetres < Zone.m_RayonMetre ;

    // si pas dans le rayon
    if ( ! DansLeRayon )
        continue ;

    // decompression de la zone
    Zone.UnCompressZone() ;

    // dedans la surface
    bool IsInArea = DansLeRayon && CPolygone::IsIn( Zone.m_PolygoneArr , Zone.m_NbPts , PtsEnCours ) ;

    // liberation des float
    Zone.FreeFloat() ;

    // si dedans la surface
    if ( IsInArea )
        {
        // zone protege
        if ( Zone.IsProtect() )
            pZoneProtegee = & Zone ;
        // zone normale
        else
            VecZoneInArea.push_back( & Zone ) ;
        }
    }

// classement reduction zone
ClasseZoneRetenueAltitude( VecZoneInArea ) ;

/*Serial.println( "debut de tri" ) ;
for ( int iz = 0 ; iz < VecZoneInAreaNorm.size() ; iz++ )
    Serial.println( VecZoneInAreaNorm[iz]->GetAltiBasse() ) ;*/

// cas des zones normales
const CZoneAer * pZoneIn = NULL ;
if ( VecZoneInArea.size() == 0 )
    {
    RetStrIn = "aucune_zone" ;
    RetNbrIn = ZONE_EN_DEHORS ;
    }
else
    {
    // zone la plus basse
    pZoneIn = VecZoneInArea[0] ;

    // variables de zone
    int  PlafondZone = pZoneIn->GetAltiBasse() ;

    ////////////
    // si corent
    if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneCorent )
        {
        m_Plafond4Valid = PlafondZone ;

        // si dedans
        if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaro > m_Plafond4Valid) &&
             (g_GlobalVar.m_TerrainPosCur.m_AltiBaro > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone(CZoneAer::ZoneCorent))) )
            {
            sprintf( TmpChar , "In %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DEDANS ;
            RetStrIn = TmpChar ;
            }
        // dessous
        else
            {
            sprintf( TmpChar , "Be %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DESSOUS ;
            RetStrIn = TmpChar ;
            }

        // si limite alti
        /*Serial.print( g_GlobalVar.m_AltitudeSolHgt ) ;
        Serial.print( "," ) ;
        Serial.print( g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;
        Serial.print( "," ) ;
        Serial.println( g_GlobalVar.m_Config.m_AltiMargin ) ;*/
        if ( ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > m_Plafond4Valid) &&
             ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone(CZoneAer::ZoneCorent))) )
            {
            sprintf( TmpChar , "Al %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrLimite = ZONE_LIMITE_ALTI ;
            RetStrLimite = TmpChar ;
            }
        }
    ////////////////
    // si pas corent
    else
        {
        // si dedans
        if ( PlafondZone < g_GlobalVar.m_TerrainPosCur.m_AltiBaro )
            {
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "In %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DEDANS ;
            RetStrIn = TmpChar ;
            }
        // en dessous
        else
            {
            m_Plafond4Valid = pZoneIn->GetAltiBasse() ;
            sprintf( TmpChar , "Be %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DESSOUS ;
            RetStrIn = TmpChar ;
            }

        // si limite altitude
        if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > PlafondZone )
            {
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "Al %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrLimite = ZONE_LIMITE_ALTI ;
            RetStrLimite = TmpChar ;
            }
        }
    }

// liberation memoire
VecZoneInArea.clear() ;
VecZoneInArea.shrink_to_fit() ;

// si zone protege et pas deja dedans une comme TMA ou activable verification hauteur sol
int HauteurSolZoneProtegee = 1000 ;
int PlafondZoneProtegee = g_GlobalVar.m_AltitudeSolHgt+HauteurSolZoneProtegee ;
if ( pZoneProtegee != NULL && RetNbrIn != ZONE_DEDANS )
    {
    // hauteur zone protegee
    HauteurSolZoneProtegee = pZoneProtegee->GetHauteurSolZoneProtect() ;

    // actualisation plafond zone protege
    PlafondZoneProtegee = g_GlobalVar.m_AltitudeSolHgt+HauteurSolZoneProtegee ;

    // si dedans
    if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro < PlafondZoneProtegee )
        {
        m_Plafond4Valid = HauteurSolZoneProtegee ;
        RetNbrIn = ZONE_DEDANS ;
        sprintf( TmpChar , "Zp %s al:%4dm" , (*pZoneProtegee).m_pNomAff , m_Plafond4Valid ) ;
        RetStrIn = TmpChar ;
        }
    // si limite
    else if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro < (PlafondZoneProtegee + g_GlobalVar.m_Config.m_AltiMargin) )
        {
        m_Plafond4Valid = HauteurSolZoneProtegee ;
        RetNbrLimite = ZONE_LIMITE_ALTI ;
        sprintf( TmpChar , "Al %s al:%4dm" , (*pZoneProtegee).m_pNomAff , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        }
    }

// variables de sortie zone en cours
m_Mutex.PrendreMutex() ;
 m_DansDessousUneZone = RetNbrIn ;
 m_NomZoneDansDessous = RetStrIn ;
m_Mutex.RelacherMutex() ;

// raz des variables distance prochaine zone et mise a jour distance altitude
if ( pZoneIn != NULL )
    {
    int PlafondZone = pZoneIn->GetAltiBasse() ;
    int DistAltCurZone = PlafondZone - g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;

    // si corent prise en compte de l'altitude sol
    if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneCorent )
        {
        int AltitudeSolPlus300 = g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone(CZoneAer::ZoneCorent) ;
        int DistAltSol = AltitudeSolPlus300 - g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
        m_DistAltCurZone = std::max( DistAltSol , DistAltCurZone ) ;
        }
    else
        m_DistAltCurZone = DistAltCurZone ;
    }
else
    m_DistAltCurZone = 9999 ;

// determination des zones proches et à l'altitude de croisement dans les 1.5 km
std::vector<CZoneAer *> VecZoneProchesXY ;   // zones proches
for ( int iz = 0 ; iz < m_NbZones; iz++ )
    {
    CZoneAer * pZoneXY = m_ZonesArr[iz] ;

    // si la zone n'est pas active
    if ( !pZoneXY->m_Activee )
        continue ;

    // pas la zone en cours
    if ( pZoneIn == pZoneXY )
        continue ;

    // dans le rayon de 1.5 km
    float DistBaryMetres = sqrtf( powf(pZoneXY->m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(pZoneXY->m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayonProche = DistBaryMetres < ( pZoneXY->m_RayonMetre + DISTANCE_PROCHE_XY ) ;

    // si pas dans le rayon proche
    if ( !DansLeRayonProche )
        continue ;

    // prise en compte de l'altitude
    bool IsNearFrontAlti = false ;
    if ( pZoneXY->IsProtect() )
        IsNearFrontAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro <= (PlafondZoneProtegee+g_GlobalVar.m_Config.m_AltiMargin) ;
        //continue ;
    else
        IsNearFrontAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro >= (pZoneXY->GetAltiBasse()-g_GlobalVar.m_Config.m_AltiMargin) ;

    // si a l'altitude de croisement
    if ( IsNearFrontAlti )
        {
        VecZoneProchesXY.push_back( pZoneXY ) ;
        pZoneXY->UnCompressZone() ;
        // distance et point frontiere proche
        pZoneXY->m_DistanceFrontiere = CDistFront::IsNearFront( pZoneXY->m_PolygoneArr , pZoneXY->m_NbPts , PtsEnCours , pZoneXY->m_CapFrontProche ) ;
        pZoneXY->FreeFloat() ;
        }
    }

// tri distance minimale des zones
CZoneAer::TrierPar( CZoneAer::TriParDistance ) ;
std::sort(VecZoneProchesXY.begin(), VecZoneProchesXY.end(),MySortFunction);

// si zone la plus proche
m_DistXYNextZone = 9999 ;
m_CapFrontProche = -1 ;
if ( VecZoneProchesXY.size() )
    {
    const CZoneAer * pZoneProche = VecZoneProchesXY[0] ;

    // distance et point prochaine zone
    m_DistXYNextZone = pZoneProche->m_DistanceFrontiere ;
    m_CapFrontProche = pZoneProche->m_CapFrontProche ;

    // si marge distance atteinte
    if ( pZoneProche->m_DistanceFrontiere <= g_GlobalVar.m_Config.m_XYMargin )
        {
        RetNbrLimite = ZONE_LIMITE_FRONTIERE ;
        // construction nom + altitude
        if ( pZoneProche->IsProtect() )
            m_Plafond4Valid = HauteurSolZoneProtegee ;
        else
            m_Plafond4Valid = pZoneProche->GetAltiBasse() ;
        sprintf( TmpChar , "Bo %s al:%4dm" , pZoneProche->m_pNomAff , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        }
    }

/*
// pour toutes les zones recherche frontiere XY proche
for ( int iz = 0 ; iz < m_NbZones && RetNbrLimite != ZONE_LIMITE_ALTI && RetNbrIn != ZONE_DEDANS && g_GlobalVar.m_Config.m_XYMargin > 0 ; iz++ )
    {
    CZoneAer * pZoneXY = m_ZonesArr[iz] ;

    // si la zone n'est pas active
    if ( !pZoneXY->m_Activee )
        continue ;

    // dans le du rayon + marge
    float DistBaryMetres = sqrtf( powf(pZoneXY->m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(pZoneXY->m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayonMarge = DistBaryMetres < (pZoneXY->m_RayonMetre+g_GlobalVar.m_Config.m_XYMargin) ;

    // si pas dans le rayon + marge
    if ( !DansLeRayonMarge )
        continue ;

    // decompression de la zone
    pZoneXY->UnCompressZone() ;

    // dedans la surface
    bool IsInArea = (pZoneIn == pZoneXY) ;
    // proche de la frontiere de zone
    bool IsNearFront = !IsInArea && CDistFront::IsNearFront( pZoneXY->m_PolygoneArr , pZoneXY->m_NbPts , PtsEnCours ) ;
    // determionation zone protege
    bool IsZoneProtege = IsNearFront && pZoneXY->IsProtect() ;

    // liberation des float
    pZoneXY->FreeFloat() ;

    // prise en compte de l'altitude
    bool IsNearFrontAlti ;
    if ( IsZoneProtege )
        IsNearFrontAlti = IsNearFront && (g_GlobalVar.m_TerrainPosCur.m_AltiBaro <= (PlafondZoneProtegee+g_GlobalVar.m_Config.m_AltiMargin)) ;
    else
        IsNearFrontAlti = IsNearFront && (g_GlobalVar.m_TerrainPosCur.m_AltiBaro >= (pZoneXY->GetAltiBasse()-g_GlobalVar.m_Config.m_AltiMargin)) ;

    // positionnement retour
    if ( IsNearFrontAlti )
        {
        RetNbrLimite = ZONE_LIMITE_FRONTIERE ;
        // construction nom + altitude
        if ( IsZoneProtege )
            m_Plafond4Valid = HauteurSolZoneProtegee ;
        else
            m_Plafond4Valid = pZoneXY->GetAltiBasse() ;
        sprintf( TmpChar , "Bo %s al:%4dm" , pZoneXY->m_pNomAff , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        break ;
        }
    } */

// variables de sortie fontiere
m_Mutex.PrendreMutex() ;
 m_LimiteZone      = RetNbrLimite ;
 m_NomZoneEnLimite = RetStrLimite ;
m_Mutex.RelacherMutex() ;
}
