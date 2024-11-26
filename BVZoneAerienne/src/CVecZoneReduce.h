////////////////////////////////////////////////////////////////////////////////
/// \file CVecZoneReduce.h
///
/// \brief
///
/// \date creation     : 25/11/2024
/// \date modification : 25/11/2024
///

#ifndef _VECZONEREDUCE_
#define _VECZONEREDUCE_

#include <vector>
#include <math.h>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>

#define UnMileEnMetres         (1852.)
#define MilesParDegres         (60.)

#define DIST_METRE_4_ZONE       50  ///< distance en metre pour le calcul/compression entre 2 points
#define ANGLE_DEGRES_4_ZONE     6   ///< angle pour supression/compression si points alignés

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de reduction du nombre de points par suppression de ceux qui
/// sont espacés de 50 metres ou qui sont en ligne droite de 6°.
/// Uilise par BVTZoneAerienne lors de la creation du fichier de zones.
class CVecZoneReduce
{
public :

    /// \brief un point du polygone
    struct st_coord_poly
        {
        float m_Lat ;   ///< latitude en degres
        float m_Lon ;   ///< longitude en degres
        } ;

    void Set( std::vector<st_coord_poly*> & VecToReduce )
        { m_pVecOrigine = & VecToReduce ; } ;
    void ReduceToDistance( int DistanceMetresEntrePoints ) ;
    void ReduceToAngle( int MemeDirectionEnDegres ) ;

private :
    std::vector<st_coord_poly*> * m_pVecOrigine ;

    /*struct st_pts_i_b
        {
        int   m_ipts ;          ///< position vecteur origine
        float m_DistanceBari ;  ///< distance baricentre

        bool operator > ( const st_pts_i_b & pts ) const
            { return m_DistanceBari > pts.m_DistanceBari ; } ;
        bool operator < ( const st_pts_i_b & pts ) const
            { return m_DistanceBari < pts.m_DistanceBari  ; } ;
        } ; */
} ;

#endif
