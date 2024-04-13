///////////////////////////////////////////////////////////////////////////////
/// \file CDroite3D.h
/// \date 07/01/2017 : date de creation
///
/// \brief
///
/// \date 20/10/2020 : NVIDIA_FUNC
/// \date 10/04/2024 : Derniere modification.
///

#ifndef _CDROITE3D_
#define _CDROITE3D_

///////////////////////////////////////////////////////////////////////////////
/// \brief Definit une droite par un point et son vecteur directeur 3d.
class CDroite3D
{
public :
    CDroite3D() ;
    CDroite3D(const CVecteur3D & VecDir, const CPoint3D & pta ) ;
    CDroite3D(const CPoint3D & pta, const CPoint3D & ptb ) ;
    ~CDroite3D() ;

    void    Set(const CPoint3D & pta, const CPoint3D & ptb ) ;
    void    Set(const CVecteur3D & VecDir, const CPoint3D & pta ) ;
    bool    IsPointInDroite( const CPoint3D& pt ) const ;
    float GetDistanceAuPoint(const CPoint3D& pt) const ;
    float GetDistanceADroite(const CDroite3D& db ) const ;
    const CPoint3D GetPoint( float t ) const ;
    const CPoint3D GetProjectionDuPoint(const CPoint3D& pt) const ;

    // donnees de la droite
    CVecteur3D m_VecDir ;  ///< vecteur directeur
    CPoint3D   m_pta ;     ///< coordonnees du point A par ou passe la droite
};

#endif // _CDROITE_
