///////////////////////////////////////////////////////////////////////////////
/// \file CVecteur3D.h
/// \date 05/01/2017 : date de creation
///
/// \brief
///
/// \date 20/10/2020 : NVIDIA_FUNC
/// \date 10/04/2024 : Derniere modification.
///

#ifndef _CVECTEUR3D_
#define _CVECTEUR3D_

///////////////////////////////////////////////////////////////////////////////
/// \brief Point en 3d.
class CPoint3D
{
public :
    CPoint3D()
        {m_x=m_y=m_z=0.;};
    CPoint3D(float x,float y,float z)
        {m_x=x;m_y=y;m_z=z;} ;
    ~CPoint3D()
        {} ;
    float         GetDistanceAuPoint( const CPoint3D & pt ) const ;
    const CPoint3D GetPointLePlusProche( const CPoint3D & pta , const CPoint3D & ptb ) const ;
    const CPoint3D GetPointLePlusLoin( const CPoint3D & pta , const CPoint3D & ptb ) const ;
    void            SetPointMilieu( const CPoint3D & pta , const CPoint3D & ptb ) ;
    void            Set(float x,float y,float z) ;

    float m_x ;   ///< coordonnees x du point
    float m_y ;   ///< coordonnees y du point
    float m_z ;   ///< coordonnees z du point

    bool operator == ( const CPoint3D & pt ) const ;
    bool operator != ( const CPoint3D & pt ) const ;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Classe de calcul sur vecteur 3d.
class CVecteur3D
{
public :
    CVecteur3D() ;
    CVecteur3D(float x,float y,float z) ;
    CVecteur3D(const CPoint3D & ptdestination , const CPoint3D & ptorigine) ;
    ~CVecteur3D() ;

    float     GetAngleRad( const CVecteur3D & Vec ) const ;
    float     GetAngleDeg( const CVecteur3D & Vec ) const ;
    float     GetNorm() const ;
    float     GetProduitScalaire( const CVecteur3D & Vec ) const ;
    const CVecteur3D GetProduitVectoriel( const CVecteur3D & Vec ) const ;
    bool        IsZero() const ;
    void        Set(float x,float y,float z) ;
    void        Set(const CPoint3D & ptdestination , const CPoint3D & ptorigine) ;
    bool        IsCollinear( const CVecteur3D & Vec , bool Strict = false ) const ;
    const CVecteur3D GetPerpendicular(float x, float y ) const ;

    float m_x ;   ///< coordonnees x du vecteur
    float m_y ;   ///< coordonnees y du vecteur
    float m_z ;   ///< coordonnees z du vecteur

private :

};

// surcharge d'operateur
CVecteur3D operator + ( const CVecteur3D & VecA , const CVecteur3D & VecB ) ;    ///< addition
CVecteur3D operator - ( const CVecteur3D & VecA , const CVecteur3D & VecB ) ;    ///< soustraction
CVecteur3D operator * ( const CVecteur3D & VecA , const CVecteur3D & VecB ) ;    ///< produit vectoriel
float     operator ^ ( const CVecteur3D & VecA , const CVecteur3D & VecB ) ;    ///< produit scalaire
CVecteur3D operator * ( const CVecteur3D & Vec , float Nombre ) ;               ///< multiplication de norme
CVecteur3D operator / ( const CVecteur3D & Vec , float Nombre ) ;               ///< division de norme

#endif // _CVECTEUR_
