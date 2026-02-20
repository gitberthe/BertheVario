////////////////////////////////////////////////////////////////////////////////
/// \file CRectIn.h
///
/// \brief
///
/// \date 27/10/2025 : creation
///

#ifndef _RECTIN_
#define _RECTIN_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe pour definir si l'on est dans le rectangle de la zone
class CRectIn
{
public :
    /// \brief un point du polygone
    struct st_coord_poly
        {
        union {
            float m_Lat ;   ///< latitude en degres
            float y ;
            } ;
        union {
            float m_Lon ;   ///< longitude en degres
            float x ;
            } ;
        } ;

    float m_LonMin ;    ///< bornes du rectangle contenant la zone
    float m_LonMax ;
    float m_LatMin ;
    float m_LatMax ;

    float GetDistanceMaxDegres( const st_coord_poly * PtsArr , int NbPts , const st_coord_poly & BaryCenter ) const ;
    void  SetLatLonMinMax( const st_coord_poly * PtsArr , int NbPts ) ;
    bool  IsInRect( const st_coord_poly & PtsVol ) const ;
} ;

#endif
