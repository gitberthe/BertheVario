////////////////////////////////////////////////////////////////////////////////
/// \file CVecZoneReduce.cpp
///
/// \brief
///
/// \date creation     : 25/11/2024
/// \date modification : 25/11/2024
///

#include "CVecZoneReduce.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur en nombre de points. Prends les points 3 a 3 et
/// supprime le milieux s'ils sont distant de moins de DistanceMetresEntrePoints
void CVecZoneReduce::ReduceToDistance( int DistanceMetresEntrePoints )
{
/*// calcul du barycentre
st_coord_poly PtsBarycentre ;
PtsBarycentre.m_Lat = 0 ;
PtsBarycentre.m_Lon = 0 ;
for ( int is = 0 ; is < (int)m_pVecOrigine->size() ; is++ )
    {
    const st_coord_poly & PtsCour = *(*m_pVecOrigine)[is] ;
    PtsBarycentre.m_Lat += PtsCour.m_Lat ;
    PtsBarycentre.m_Lon += PtsCour.m_Lon ;
    }
PtsBarycentre.m_Lat /= m_pVecOrigine->size() ;
PtsBarycentre.m_Lon /= m_pVecOrigine->size() ;*/

// destruction des points
bool EncoreDesDestructions = true ;
while ( EncoreDesDestructions )
    {
    EncoreDesDestructions = false ;

    // supression des points 3 a 3
    for ( int ip = 0 ; ip+2 < (int)m_pVecOrigine->size() ; ip+=3 )
        {
        // tableau des 3 points
        std::vector< st_coord_poly* > Vect3pts ;
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
        std::vector<st_coord_poly*> VecReduce ;
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
void CVecZoneReduce::ReduceToAngle( int MemeDirectionEnDegres  )
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
        std::vector< st_coord_poly* > Vect3pts ;
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
            std::vector<st_coord_poly*> VecReduce ;
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
}
