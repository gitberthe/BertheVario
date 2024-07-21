////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 21/07/2024
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
}

/******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou surface
bool CZoneAer::operator > ( const CZoneAer & Zone ) const
{
if ( ms_TriParNom )
    return (strcmp( m_NomAff.c_str() , Zone.m_NomAff.c_str() ) > 0 ) ;

return m_Area > Zone.m_Area ;
//return GetAlti() > Zone.GetAlti() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou surface
bool CZoneAer::operator < ( const CZoneAer & Zone ) const
{
if ( ms_TriParNom )
    return ( strcmp( m_NomAff.c_str() , Zone.m_NomAff.c_str() ) < 0 ) ;

return m_Area < Zone.m_Area ;
//return GetAlti() < Zone.GetAlti() ;
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
