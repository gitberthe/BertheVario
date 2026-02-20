///////////////////////////////////////////////////////////////////////////////
/// \file CDroite2D.h
/// \date 07/01/2017 : date de creation
///
/// \brief
///
/// \date 20/10/2020 : NVIDIA_FUNC
/// \date 10/04/2024 : Derniere modification.
///

#ifndef _CDROITE2D_
#define _CDROITE2D_

///////////////////////////////////////////////////////////////////////////////
/// \brief Definit une droite par un point et son vecteur directeur 3d.
class CDroite2D
{
public :
    CDroite2D() ;
    CDroite2D(const CVecteur2D & VecDir, const CPoint2D & pta ) ;
    CDroite2D(const CPoint2D & pta, const CPoint2D & ptb ) ;
    ~CDroite2D() ;

    void    Set(const CPoint2D & pta, const CPoint2D & ptb ) ;
    void    Set(const CVecteur2D & VecDir, const CPoint2D & pta ) ;
    bool    IsPointInDroite( const CPoint2D& pt ) const ;
    float GetDistanceAuPoint(const CPoint2D& pt) const ;
    float GetDistanceADroite(const CDroite2D& db ) const ;
    const CPoint2D GetPoint( float t ) const ;
    const CPoint2D GetProjectionDuPoint(const CPoint2D& pt) const ;

    // donnees de la droite
    CVecteur2D m_VecDir ;  ///< vecteur directeur
    CPoint2D   m_pta ;     ///< coordonnees du point A par ou passe la droite
};

#endif // _CDROITE_
