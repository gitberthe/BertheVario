////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.cpp
///
/// \brief
///
/// \date creation   : 23/03/2024
/// \date 26/10/2025 : tableau de st_coord_poly et plus *.
/// \date 27/10/2025 : Remplacement de m_RayonMetre par CRectIn.
/// \date 06/01/2026 : modification pour uncompress non systematique
/// \date 07/01/2026 : modification pour zone active non compresse
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

CZoneAer::TriPar CZoneAer::ms_TriPar = CZoneAer::TriParAltitude ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Destructeur, liberation des points
CZoneAer::~CZoneAer()
{
// points non compressés
if ( m_PolygoneArr != NULL )
    {
    //for ( int ip = 0 ; ip < m_NbPts ; ip++ )
    //    delete m_PolygoneArr[ip] ;
    delete [] m_PolygoneArr ;
    m_PolygoneArr = NULL ;
    m_NbPts = 0 ;
    }
// point compressés
if ( m_LowResShortArr != NULL )
    {
    delete [] m_LowResShortArr ;
    m_LowResShortArr = NULL ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou alti basse
bool CZoneAer::operator > ( const CZoneAer & Zone ) const
{
if ( ms_TriPar == TriParNom )
    return (strcmp( m_pNomAff , Zone.m_pNomAff ) > 0 ) ;
//else if ( ms_TriPar == TriParDistance )
//    return m_DistanceFrontiere > Zone.m_DistanceFrontiere ;

//return m_Area > Zone.m_Area ;
return GetAltiBasse() > Zone.GetAltiBasse() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou alti basse
bool CZoneAer::operator < ( const CZoneAer & Zone ) const
{
if ( ms_TriPar == TriParNom )
    return ( strcmp( m_pNomAff , Zone.m_pNomAff ) < 0 ) ;
//else if ( ms_TriPar == TriParDistance )
//    return m_DistanceFrontiere < Zone.m_DistanceFrontiere ;

//return m_Area < Zone.m_Area ;
return GetAltiBasse() < Zone.GetAltiBasse() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne le type de zone ex : corent, st-yan
void CZoneAer::SetTypeZone()
{
// determination corent
if ( GetAltiBasse() == 822 )
    {
    m_TypeZone = ZoneCorent ;
    return;
    }
// determination zone saint yan
if ( ! strcmp( "TMA SAINT-YAN" , m_pNomAff ) )
    {
    m_TypeZone = ZoneStYan ;
    return;
    }

m_TypeZone = ZoneGeneric ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Reourne la hauteur sol admissible pour cette zone.
int CZoneAer::GetAltiSolZone() const
{
if ( m_TypeZone == ZoneCorent )
    return 300 ;
if ( m_TypeZone == ZoneStYan )
    return 610 ;
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Altitude en cours fonction de la date
int CZoneAer::GetAltiBasse() const
{
if ( ! HavePeriod() )
    return m_AltiBasse ;
else if ( m_pDerogFfvl->m_AltiAPrendreEnCompte == ALTI_BASSE )
    return m_AltiBasse ;
else if ( m_pDerogFfvl->m_AltiAPrendreEnCompte == ALTI_PERIODE_SEMAINE )
    return m_pDerogFfvl->m_AltiBassePeriodeSemaine ;
else if ( m_pDerogFfvl->m_AltiAPrendreEnCompte == ALTI_PERIODE_WEEKEND )
    return m_pDerogFfvl->m_AltiBassePeriodeWeekEnd ;
return -1 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Compresse la zone en tableau de short de resolution variable.
/// (de 1 a 7 metres, 2 metres en moyenne)
void CZoneAer::CompressZone()
{
// si pas de points a compresser
if ( m_PolygoneArr == NULL )
    return ;

// si deja compresse en int
if ( m_LowResShortArr != NULL )
    return ;

// allocation du tableau de short
m_LowResShortArr = new short [m_NbPts*2] ;

// calcul du baricentre
CPolygone::CalcBarycentre( m_PolygoneArr , m_NbPts , m_Barycentre ) ;

// short maximum en float
const float ShortMax = 32767. ;

// calcul de la resolution minimum
float DistMaxLatLonZone = CRectIn::GetDistanceMaxDegres( m_PolygoneArr , m_NbPts , m_Barycentre ) ;
m_ResolutionDegres = 1.05 * DistMaxLatLonZone / ShortMax ;
//Serial.print( ResXYMetres ) ;
//Serial.print( " " ) ;

// positionnement des bornes du cadre de zone
CRectIn::SetLatLonMinMax( m_PolygoneArr , m_NbPts ) ;

/*
// verification que la zone n'est pas trop grande
const float DistMaxZoneEnMetre = ShortMax * m_ResolutionMetre ;
if ( m_RayonMetre > DistMaxZoneEnMetre )
    {
    Serial.println( "zone aerienne trop grande") ;
    Serial.print( "Resolution en metre:" ) ;
    Serial.println( m_ResolutionMetre ) ;
    g_GlobalVar.BeepError() ;
    }
*/

// passage en short
int ipshort = 0 ;
for ( int ipstruct = 0 ; ipstruct < m_NbPts ; ipstruct++ )
    {
    const st_coord_poly * pStPts = & m_PolygoneArr[ipstruct] ;

    m_LowResShortArr[ipshort++] = (short)((pStPts->m_Lat - m_Barycentre.m_Lat) / m_ResolutionDegres ) ;
    m_LowResShortArr[ipshort++] = (short)((pStPts->m_Lon - m_Barycentre.m_Lon) / m_ResolutionDegres ) ;
    }

// supression des float
FreeFloat() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Retourne le nom pour fichier d'activation soit les 2 premier champs
/// uniquement
CLGString CZoneAer::GetNom4Activation() const
{
CLGString NomRet = m_pNomAff ;

// parcour de la chaine
int CountBlanc = 0 ;
for ( int is = 0 ; is < NomRet.GetLength() ; is++ )
    {
    // commtage des blanc
    if ( NomRet[is] == ' ' )
        CountBlanc++ ;
    // si 2ieme blanc
    if ( CountBlanc == 2 )
        {
        NomRet.SetAt(is,0) ;
        break ;
        }
    }

return NomRet ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Retourne le nom pour affichage d'activation soi les 3 premier champs
/// sans blanc
CLGString CZoneAer::GetNom4AffichageActivation() const
{
CLGString NomRet ;

// parcour de la chaine
for ( int is = 0 ; is < strlen(m_pNomAff) ; is++ )
    {
    // si blanc
    if ( m_pNomAff[is] == ' ' )
        continue;
    NomRet += m_pNomAff[is] ;
    }

return NomRet ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief  Supression du tableau des float.
void CZoneAer::FreeFloat()
{
// destruction buffer structure float lat/lon
if ( m_PolygoneArr != NULL )
    {
    delete [] m_PolygoneArr ;
    m_PolygoneArr = NULL ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief  Supression du tableau des short.
void CZoneAer::FreeShort()
{
// destruction buffer structure float lat/lon
if ( m_LowResShortArr != NULL )
    {
    delete [] m_LowResShortArr ;
    m_LowResShortArr = NULL ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief De-compresse une zone en float
void CZoneAer::UnCompressZone()
{
// si deja decompressee en float/short
if ( m_PolygoneArr != NULL )
    return ;

// si pas compresse de int
if ( m_LowResShortArr == NULL )
    return ;

// allocation du tableau st *
m_PolygoneArr = new st_coord_poly [m_NbPts] ;

// passage en float
int ipshort = 0 ;
for ( int ipstruct = 0 ; ipstruct < m_NbPts ; ipstruct++ )
    {
    // ajout du points
    st_coord_poly * pStPts = & m_PolygoneArr[ipstruct] ;
    pStPts->m_Lat = ((float)m_LowResShortArr[ipshort++]) * m_ResolutionDegres + m_Barycentre.m_Lat ;
    pStPts->m_Lon = ((float)m_LowResShortArr[ipshort++]) * m_ResolutionDegres + m_Barycentre.m_Lon ;
    }

// liberation des shorts
FreeShort() ;
}

