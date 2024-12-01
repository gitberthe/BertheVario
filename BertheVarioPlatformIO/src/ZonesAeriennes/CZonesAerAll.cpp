////////////////////////////////////////////////////////////////////////////////
/// \file CZonesAerAll.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 01/12/2024
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
LectureFichierZonesAer() ;
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
m_File = SD.open(NOM_FCH_ZONE_AER);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
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

m_File.close();

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

// recopie nom de zone
pZone->m_NomOri = pChar ;
pZone->m_NomAff = pChar ;

// zone protegee on enleve "reserve naturelle nationale de la vallee de"
int iespacemax = 3 ;
char NomRND[]  = "Reserve naturelle nationale de " ;
char NomRNDV[] = "Reserve naturelle nationale de la Vallee de " ;
char NomProtect[] = "PROTECT " ;
if ( strstr( pChar , NomProtect ) )
    {
    iespacemax = 1 ;
    pChar += strlen( NomProtect ) ;
    char * pTmpChar = strstr( pChar , NomRNDV ) ;
    if ( pTmpChar != NULL )
       pChar += strlen( NomRNDV ) ;
    else
        {
        pTmpChar = strstr( pChar , NomRND ) ;
        if ( pTmpChar != NULL )
            pChar += strlen( NomRND ) ;
        }
    pZone->m_NomAff = pChar ;
    }

// formattage nom de zone 3 champs
int iespace = 0 ;
for ( int ic = 0 ; ic < pZone->m_NomAff.size() ; ic++ )
    {
    if ( pZone->m_NomAff[ic] == '(' )   // premier (
        {
        pZone->m_NomAff.resize( ic ) ;
        break ;
        }
    else if ( (pZone->m_NomAff[ic] == ' ' || pZone->m_NomAff[ic] == '-') && ++iespace >= iespacemax ) // trois champs max
        {
        pZone->m_NomAff.resize( ic ) ;
        break ;
        }
    }
pZone->m_NomOri.shrink_to_fit();
pZone->m_NomAff.shrink_to_fit();

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
m_File = SD.open(NOM_FCH_ZONE_PER);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
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
        TraiteBufferZonePeriode( TmpChar ) ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferZonePeriode( TmpChar ) ;

m_File.close();

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
NomOri += ' ' ;     // caractere de fin de champ pour recherche sans bug
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
    pZone->m_NomAff = NomAff ;
    pZone->m_NomAff.shrink_to_fit() ;
    }
/*Serial.print(pZone->m_NomOri.c_str()) ;
Serial.print("|") ;
Serial.println(pZone->m_NomAff.c_str()) ;*/

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
        if ( VecNomDejaEcrit[iv] == pZone->m_NomAff )
            {
            DejaFait = true ;
            break ;
            }
        }


    // si deja ecrit dans le fichier
    if ( DejaFait )
        continue ;

    // empilement pour suivit deja fait
    VecNomDejaEcrit.push_back( pZone->m_NomAff ) ;

    // ecriture fichier
    Fch.print( pZone->m_NomAff.c_str() ) ;
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
m_File = SD.open(NOM_FCH_ZONE_ACT);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
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
        TraiteBufferZoneActive( TmpChar ) ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferZoneActive( TmpChar ) ;

m_File.close();

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
    if ( strstr( Zone.m_NomOri.c_str() , NomOri.c_str() ) )
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
    if ( strstr( Zone.m_NomOri.c_str() , NomOri ) )
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
CZoneAer::TriParNom( true ) ;
TriSchellMetzner() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction de tri de vector.
bool MySortFunction (const CZoneAer *p1,const CZoneAer *p2)
{
return ((*p1)<(*p2));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri altitude croissante pour determination
void CZonesAerAll::TriZonesAltitude( std::vector<const CZoneAer *> & VecZones )
{
/*CSortArray::m_pArr = & VecZones[0] ;
CSortArray::m_Size = VecZones.size() ;
CZoneAer::TriParNom( false ) ;
TriSchellMetzner(true) ; */
CZoneAer::TriParNom( false ) ;
std::sort(VecZones.begin(), VecZones.end(),MySortFunction);
//std::reverse(VecZones.begin(),VecZones.end());
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
std::vector<const CZoneAer *> VecZoneInAreaNorm ;   // zones normale comme TMA
std::vector<const CZoneAer *> VecZoneInAreaActivee ;// zones activable comme R_368B
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
        if ( strstr( Zone.m_NomOri.c_str() , "PROTECT" ) != NULL )
            pZoneProtegee = & Zone ;
        // zone activable
        else if ( Zone.m_DansFchActivation )
            VecZoneInAreaActivee.push_back( & Zone ) ;
        // zone normale
        else
            VecZoneInAreaNorm.push_back( & Zone ) ;
        }
    }

// tri des zones par altitude croissante
TriZonesAltitude( VecZoneInAreaNorm ) ;
/*Serial.println( "debut de tri" ) ;
for ( int iz = 0 ; iz < VecZoneInAreaNorm.size() ; iz++ )
    Serial.println( VecZoneInAreaNorm[iz]->GetAltiBasse() ) ;*/

// cas des zones normales
if ( VecZoneInAreaNorm.size() == 0 )
    {
    RetStrIn = "aucune_zone" ;
    RetNbrIn = ZONE_EN_DEHORS ;
    }
else
    {
    // zone la plus basse
    const CZoneAer * pZone = VecZoneInAreaNorm[0] ;
    // si la zone du dessus a une derogation elle est prioritaire
    if ( VecZoneInAreaNorm.size() > 1 && VecZoneInAreaNorm[1]->HavePeriod() )
        pZone = VecZoneInAreaNorm[1] ;

    // variables de zone
    int  PlafondZone = pZone->GetAltiBasse() ;
    bool Corent = ( PlafondZone == 822 ) ;

    ////////////
    // si corent
    if ( Corent )
        {
        m_Plafond4Valid = PlafondZone ;

        // si dedans
        if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaro > m_Plafond4Valid) &&
             (g_GlobalVar.m_TerrainPosCur.m_AltiBaro > (g_GlobalVar.m_AltitudeSolHgt+300)) )
            {
            sprintf( TmpChar , "In %s al:%4dm" , pZone->m_NomAff.c_str() , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DEDANS ;
            RetStrIn = TmpChar ;
            }
        // dessous
        else
            {
            sprintf( TmpChar , "Be %s al:%4dm" , pZone->m_NomAff.c_str() , m_Plafond4Valid ) ;
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
             ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > (g_GlobalVar.m_AltitudeSolHgt+300)) )
            {
            sprintf( TmpChar , "Al %s al:%4dm" , pZone->m_NomAff.c_str() , m_Plafond4Valid ) ;
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
            sprintf( TmpChar , "In %s al:%4dm" , pZone->m_NomAff.c_str() , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DEDANS ;
            RetStrIn = TmpChar ;
            }
        // en dessous
        else
            {
            //const CZoneAer & ZoneDuDessus = *VecZoneInAreaNorm[1] ;
            m_Plafond4Valid = pZone->GetAltiBasse() ;
            sprintf( TmpChar , "Be %s al:%4dm" , pZone->m_NomAff.c_str() , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DESSOUS ;
            RetStrIn = TmpChar ;
            }

        // si limite altitude
        if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > PlafondZone )
            {
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "Al %s al:%4dm" , pZone->m_NomAff.c_str() , m_Plafond4Valid ) ;
            RetNbrLimite = ZONE_LIMITE_ALTI ;
            RetStrLimite = TmpChar ;
            }
        }
    }

// pour toutes les zones activables recherche dedans si pas deja dedans une comme TMA
for ( int iz = 0 ; iz < VecZoneInAreaActivee.size() && RetNbrIn != ZONE_DEDANS ; iz++ )
    {
    const CZoneAer & Zone = *VecZoneInAreaActivee[iz] ;
    int PlafondZone = Zone.GetAltiBasse() ;

    // si dedans
    if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro >= PlafondZone )
        {
        RetNbrIn = ZONE_DEDANS ;
        // construction nom + altitude
        char TmpChar[50] ;
        m_Plafond4Valid=PlafondZone ;
        sprintf( TmpChar , "In %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
        RetStrIn = TmpChar ;
        break ;
        }
    // si limite altitude
    else if ( (PlafondZone-g_GlobalVar.m_Config.m_AltiMargin) < g_GlobalVar.m_TerrainPosCur.m_AltiBaro )
        {
        m_Plafond4Valid = PlafondZone ;
        sprintf( TmpChar , "Al %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
        RetNbrLimite = ZONE_LIMITE_ALTI ;
        RetStrLimite = TmpChar ;
        }
    }

// si zone protege et pas deja dedans une comme TMA ou activable
int HauteurSolZoneProtegee = 1000 ;
int PlafondZoneProtegee = g_GlobalVar.m_AltitudeSolHgt+HauteurSolZoneProtegee ;
if ( pZoneProtegee != NULL && RetNbrIn != ZONE_DEDANS )
    {
    // detertimation plafond zone proetegee
    if ( strstr( pZoneProtegee->m_NomOri.c_str() , "1000m/sol" ) != NULL )
        HauteurSolZoneProtegee = 1000 ;
    else if ( strstr( pZoneProtegee->m_NomOri.c_str() , "500m/sol" ) != NULL )
        HauteurSolZoneProtegee = 500 ;
    else if ( strstr( pZoneProtegee->m_NomOri.c_str() , "300m/sol" ) != NULL )
        HauteurSolZoneProtegee = 300 ;
    else if ( strstr( pZoneProtegee->m_NomOri.c_str() , "150m/sol" ) != NULL )
        HauteurSolZoneProtegee = 150 ;
    else if ( strstr( pZoneProtegee->m_NomOri.c_str() , "50m/sol" ) != NULL )
        HauteurSolZoneProtegee = 50 ;

    // actualisation plafond zone protege
    PlafondZoneProtegee = g_GlobalVar.m_AltitudeSolHgt+HauteurSolZoneProtegee ;

    // si dedans
    if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro < PlafondZoneProtegee )
        {
        m_Plafond4Valid = HauteurSolZoneProtegee ;
        RetNbrIn = ZONE_DEDANS ;
        sprintf( TmpChar , "Zp %s al:%4dm" , (*pZoneProtegee).m_NomAff.c_str() , m_Plafond4Valid ) ;
        RetStrIn = TmpChar ;
        }
    // si limite
    else if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro < (PlafondZoneProtegee + g_GlobalVar.m_Config.m_AltiMargin) )
        {
        m_Plafond4Valid = HauteurSolZoneProtegee ;
        RetNbrLimite = ZONE_LIMITE_ALTI ;
        sprintf( TmpChar , "Al %s al:%4dm" , (*pZoneProtegee).m_NomAff.c_str() , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        }
    }

m_Mutex.PrendreMutex() ;
 m_DansDessousUneZone = RetNbrIn ;
 m_NomZoneDansDessous = RetStrIn ;
m_Mutex.RelacherMutex() ;

// pour toutes les zones recherche frontiere XY
for ( int iz = 0 ; iz < m_NbZones && RetNbrLimite != ZONE_LIMITE_ALTI && RetNbrIn != ZONE_DEDANS && g_GlobalVar.m_Config.m_XYMargin > 0 ; iz++ )
    {
    CZoneAer & Zone = *m_ZonesArr[iz] ;

    // si la zone n'est pas active
    if ( !Zone.m_Activee )
        continue ;

    // dans le du rayon + marge
    float DistBaryMetres = sqrtf( powf(Zone.m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(Zone.m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayonMarge = DistBaryMetres < (Zone.m_RayonMetre+g_GlobalVar.m_Config.m_XYMargin) ;

    // si pas dans le rayon + marge
    if ( !DansLeRayonMarge )
        continue ;

    // decompression de la zone
    Zone.UnCompressZone() ;

    // dedans la surface
    bool IsInArea = CPolygone::IsIn( Zone.m_PolygoneArr , Zone.m_NbPts , PtsEnCours ) ;
    // proche de la frontiere de zone
    bool IsNearFront = !IsInArea && CDistFront::IsNearFront( Zone.m_PolygoneArr , Zone.m_NbPts , PtsEnCours ) ;
    // determionation zone protege
    bool ZoneProtege = IsNearFront && (strstr( Zone.m_NomOri.c_str() , "PROTECT" ) != NULL) ;

    // liberation des float
    Zone.FreeFloat() ;

    // prise en compte de l'altitude
    bool IsNearFrontAlti ;
    if ( ZoneProtege )
        IsNearFrontAlti = IsNearFront && (g_GlobalVar.m_TerrainPosCur.m_AltiBaro <= (PlafondZoneProtegee+g_GlobalVar.m_Config.m_AltiMargin)) ;
    else
        IsNearFrontAlti = IsNearFront && (g_GlobalVar.m_TerrainPosCur.m_AltiBaro >= (Zone.GetAltiBasse()-g_GlobalVar.m_Config.m_AltiMargin)) ;

    // positionnement retour
    if ( IsNearFrontAlti )
        {
        RetNbrLimite = ZONE_LIMITE_FRONTIERE ;
        // construction nom + altitude
        if ( ZoneProtege )
            m_Plafond4Valid = HauteurSolZoneProtegee ;
        else
            m_Plafond4Valid = Zone.GetAltiBasse() ;
        sprintf( TmpChar , "Bo %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        break ;
        }
    }

// variables de sortie fontiere
m_Mutex.PrendreMutex() ;
 m_LimiteZone      = RetNbrLimite ;
 m_NomZoneEnLimite = RetStrLimite ;
m_Mutex.RelacherMutex() ;
}
