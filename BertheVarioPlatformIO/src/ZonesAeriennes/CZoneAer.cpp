////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 26/11/2024
///

#include "../BertheVario.h"

bool CZoneAer::ms_TriParNom = false ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Destructeur, liberation des points
CZoneAer::~CZoneAer()
{
if ( m_PolygoneArr != NULL )
    {
    for ( int ip = 0 ; ip < m_NbPts ; ip++ )
        delete m_PolygoneArr[ip] ;
    delete [] m_PolygoneArr ;
    m_PolygoneArr = NULL ;
    m_NbPts = 0 ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou alti basse
bool CZoneAer::operator > ( const CZoneAer & Zone ) const
{
if ( ms_TriParNom )
    return (strcmp( m_NomAff.c_str() , Zone.m_NomAff.c_str() ) > 0 ) ;

//return m_Area > Zone.m_Area ;
return GetAltiBasse() > Zone.GetAltiBasse() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou alti basse
bool CZoneAer::operator < ( const CZoneAer & Zone ) const
{
if ( ms_TriParNom )
    return ( strcmp( m_NomAff.c_str() , Zone.m_NomAff.c_str() ) < 0 ) ;

//return m_Area < Zone.m_Area ;
return GetAltiBasse() < Zone.GetAltiBasse() ;
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

// short maximum en float
const float ShortMax = 32767. ;

// resolution entiere minimum
m_ResolutionMetre = ceilf( m_RayonMetre / ShortMax ) ;

// verification que la zone n'est pas trop grande
const float RayonMaxZoneEnMetre = ShortMax * m_ResolutionMetre ;
if ( m_RayonMetre >= RayonMaxZoneEnMetre )
    {
    Serial.println( "zone aerienne trop grande") ;
    g_GlobalVar.BeepError() ;
    g_GlobalVar.Reboot() ;
    }

// passage en short
int ipshort = 0 ;
for ( int ipstruct = 0 ; ipstruct < m_NbPts ; ipstruct++ )
    {
    const st_coord_poly * pStPts = m_PolygoneArr[ipstruct] ;

    m_LowResShortArr[ipshort++] = (short)((float)((pStPts->m_Lat - m_Barycentre.m_Lat) * MilesParDegres * UnMileEnMetres / ((float)m_ResolutionMetre))) ;
    m_LowResShortArr[ipshort++] = (short)((float)((pStPts->m_Lon - m_Barycentre.m_Lon) * MilesParDegres * UnMileEnMetres / ((float)m_ResolutionMetre))) ;
    }

// supression des float
FreeFloat() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief  Supression du tableau des float.
void CZoneAer::FreeFloat()
{
// destruction buffer structure float lat/lon
if ( m_PolygoneArr != NULL )
    {
    for ( int ip = 0 ; ip < m_NbPts ; ip++ )
        delete m_PolygoneArr[ip] ;
    delete [] m_PolygoneArr ;
    m_PolygoneArr = NULL ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief De-compresse une zone en float
void CZoneAer::UnCompressZone()
{
// si deja decompressee en float/short
FreeFloat() ;

// allocation du tableau st *
m_PolygoneArr = new st_coord_poly * [m_NbPts] ;

// passage en float
int ipshort = 0 ;
for ( int ipstruct = 0 ; ipstruct < m_NbPts ; ipstruct++ )
    {
    // ajout du points
    st_coord_poly * pStPts = new st_coord_poly ;
    m_PolygoneArr[ipstruct] = pStPts ;

    pStPts->m_Lat = ((float)m_LowResShortArr[ipshort++]) / (MilesParDegres * UnMileEnMetres / ((float)m_ResolutionMetre)) + m_Barycentre.m_Lat ;
    pStPts->m_Lon = ((float)m_LowResShortArr[ipshort++]) / (MilesParDegres * UnMileEnMetres / ((float)m_ResolutionMetre)) + m_Barycentre.m_Lon ;
    }
}

/******************************************************************************/

/*
///////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur en nombre de points. Prends les points 3 a 3 et
/// supprime le milieux s'ils sont distant de moins de DistanceMetresEntrePoints
void CVecReduce::ReduceToDistance( int DistanceMetresEntrePoints )
{

// destruction des points
bool EncoreDesDestructions = true ;
while ( EncoreDesDestructions )
    {
    EncoreDesDestructions = false ;

    // supression des points 3 a 3
    for ( int ip = 0 ; ip+2 < (int)m_pVecOrigine->size() ; ip+=3 )
        {
        // tableau des 3 points
        std::vector< CZoneAer::st_coord_poly* > Vect3pts ;
        for ( long ipb = 0 ; ipb < 3 ; ipb++ )
            Vect3pts.push_back( (*m_pVecOrigine)[ip+ipb] ) ;

        // calcul distance min entre points
        float DistMinMetres = 99999999 ;
        for ( long ipd = 0 ; ipd < 2 ; ipd++ )
            {
            float Dist = sqrtf( powf( Vect3pts[ipd]->m_Lat - Vect3pts[ipd+1]->m_Lat , 2. ) + powf( Vect3pts[ipd]->m_Lon - Vect3pts[ipd+1]->m_Lon , 2. ) );
            Dist *= UnMileEnMetres * 60 ;
            if ( DistMinMetres > Dist )
                DistMinMetres = Dist ;
            }

        // si distance trop petite alors supression
        if ( DistMinMetres < DistanceMetresEntrePoints )
            {
            EncoreDesDestructions = true ;
            int ipd = 1 ;
            delete (*m_pVecOrigine)[ip+ipd] ;
            (*m_pVecOrigine)[ip+ipd] = NULL ;
            }

        // recopie des points restant
        std::vector<CZoneAer::st_coord_poly*> VecReduce ;
        for ( long ipr = 0 ; ipr < (int)m_pVecOrigine->size() ; ipr++ )
            {
            if ( (*m_pVecOrigine)[ipr] != NULL )
                VecReduce.push_back( (*m_pVecOrigine)[ipr] ) ;
            }

        // recopie du vecteur reduit
        *m_pVecOrigine = VecReduce ;
        }
    }
m_pVecOrigine->shrink_to_fit() ;
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur en nombre de points. Prends les points 3 a 3 et
/// supprime le points du centre si ils ont la meme direction à MemeDirectionEnDegres.
void CVecReduce::ReduceToAngle( int MemeDirectionEnDegres  )
{
// temps que points a deleter
bool DeletePts = true ;
while ( DeletePts )
    {
    DeletePts = false ;
    // supression des points 3 a 3
    for ( int ip = 0 ; ip+2 < (int)m_pVecOrigine->size() ; ip++ )
        {
        // vecteur des 3 poitns
        std::vector< CZoneAer::st_coord_poly* > Vect3pts ;
        for ( long ipb = 0 ; ipb < 3 ; ipb++ )
            Vect3pts.push_back( (*m_pVecOrigine)[ip+ipb] ) ;

        // calcul directions par produit scalaire angle ouvert points milieu
        float u1 = Vect3pts[0]->m_Lon - Vect3pts[1]->m_Lon ;
        float v1 = Vect3pts[2]->m_Lon - Vect3pts[1]->m_Lon ;
        float u2 = Vect3pts[0]->m_Lat - Vect3pts[1]->m_Lat ;
        float v2 = Vect3pts[2]->m_Lat - Vect3pts[1]->m_Lat  ;
        float CosAngle = ( u1*v1+u2*v2 ) / (sqrtf(u1*u1+u2*u2)*sqrtf(v1*v1+v2*v2)) ;
        float AngleDeg = acosf(CosAngle) * 180. / 3.14159265358979323846 ;


        // si angle plat alors suppression
        if ( fabs(AngleDeg) > (180 - MemeDirectionEnDegres) )
            {
            // point à detruire
            long ip2del = 1 ;
            DeletePts = true ;

            // destruction points
            delete (*m_pVecOrigine)[ ip+ip2del ] ;
            (*m_pVecOrigine)[ ip+ip2del ] = NULL ;

            // recopie des points restant
            std::vector<CZoneAer::st_coord_poly*> VecReduce ;
            VecReduce.reserve( m_pVecOrigine->size() ) ;
            for ( long ipt = 0 ; ipt < (int)m_pVecOrigine->size() ; ipt++ )
                {
                if ( (*m_pVecOrigine)[ipt] != NULL )
                    VecReduce.push_back( (*m_pVecOrigine)[ipt] ) ;
                }

            // recopie du vecteur reduit
            *m_pVecOrigine = VecReduce ;
            //m_pVecOrigine->shrink_to_fit() ;
            }
        }
    }
} */

/*////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur en nombre de points. Prends les points 2 a 2 et supprime le
/// plus proche du barycentre qui sont distant de moins de DistanceMetresEntrePoints
void CVecReduce::ReduceTo( int DistanceMetresEntrePoints )
{
// calcul du barycentre
CZoneAer::st_coord_poly PtsBarycentre ;
PtsBarycentre.m_Lat = 0 ;
PtsBarycentre.m_Lon = 0 ;
for ( int is = 0 ; is < m_pVecOrigine->size() ; is++ )
    {
    const CZoneAer::st_coord_poly & PtsCour = *(*m_pVecOrigine)[is] ;
    PtsBarycentre.m_Lat += PtsCour.m_Lat ;
    PtsBarycentre.m_Lon += PtsCour.m_Lon ;
    }
PtsBarycentre.m_Lat /= m_pVecOrigine->size() ;
PtsBarycentre.m_Lon /= m_pVecOrigine->size() ;

// destruction des points
bool EncoreDesDestructions = true ;
while ( EncoreDesDestructions )
    {
    EncoreDesDestructions = false ;

    // distance du barycentre de chaque point
    std::vector<float> VecDistBary ;
    VecDistBary.reserve( m_pVecOrigine->size() ) ;
    for ( int ip = 0 ; ip < m_pVecOrigine->size() ; ip++ )
        {
        float Dist = sqrtf( powf( (*m_pVecOrigine)[ip]->m_Lat - PtsBarycentre.m_Lat , 2.) +
                            powf( (*m_pVecOrigine)[ip]->m_Lon - PtsBarycentre.m_Lon , 2.) ) ;
        VecDistBary.push_back( Dist ) ;
        }

    // supression des points 2 a 2
    for ( int ip = 0 ; ip+1 < m_pVecOrigine->size() ; ip+=2 )
        {
        // calcul distance entre les 2 points
        float DistAnguDeg = sqrtf( powf( (*m_pVecOrigine)[ip]->m_Lat - (*m_pVecOrigine)[ip+1]->m_Lat , 2.) +
                            powf( (*m_pVecOrigine)[ip]->m_Lon - (*m_pVecOrigine)[ip+1]->m_Lon , 2.) ) ;

        // si distance trop petite alors supression
        if ( (DistAnguDeg * UnMileEnMetres * 60) < DistanceMetresEntrePoints )
            {
            EncoreDesDestructions = true ;
            // points le plus proche du barycentre
            if ( VecDistBary[ip] < VecDistBary[ip+1] )
                {
                delete (*m_pVecOrigine)[ ip ] ;
                (*m_pVecOrigine)[ ip ] = NULL ;
                }
            else
                {
                delete (*m_pVecOrigine)[ ip+1 ] ;
                (*m_pVecOrigine)[ ip+1 ] = NULL ;
                }
            }
        }

    // recopie des points restant
    std::vector<CZoneAer::st_coord_poly*> VecReduce ;
    VecReduce.reserve( m_pVecOrigine->size() / 2 ) ;
    for ( long ip = 0 ; ip < m_pVecOrigine->size() ; ip++ )
        {
        if ( (*m_pVecOrigine)[ip] != NULL )
            VecReduce.push_back( (*m_pVecOrigine)[ip] ) ;
        }

    // recopie du vecteur reduit
    *m_pVecOrigine = VecReduce ;
    }
m_pVecOrigine->shrink_to_fit() ;
} */
