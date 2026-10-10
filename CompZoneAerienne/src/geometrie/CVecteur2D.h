///////////////////////////////////////////////////////////////////////////////
/// \file CVecteur2D.h
/// \date 05/01/2017 : date de creation
///
/// \brief
///
/// \date 20/10/2020 : NVIDIA_FUNC
/// \date 30/11/2024 : Derniere modification.
///

#ifndef _CVECTEUR2D_
#define _CVECTEUR2D_

#define PRECISION_ZERO (0.001)
#define RAD_2_DEG(x)   (x*180./T_PI)

///////////////////////////////////////////////////////////////////////////////
/// \brief Point en 3d.
class CPoint2D
{
public :
    CPoint2D()
        {m_x=m_y=0.;};
    CPoint2D(double x,double y)
        {m_x=x;m_y=y;} ;
    ~CPoint2D()
        {} ;
    double         GetDistanceAuPoint( const CPoint2D & pt ) const ;
    const CPoint2D GetPointLePlusProche( const CPoint2D & pta , const CPoint2D & ptb ) const ;
    const CPoint2D GetPointLePlusLoin( const CPoint2D & pta , const CPoint2D & ptb ) const ;
    void            SetPointMilieu( const CPoint2D & pta , const CPoint2D & ptb ) ;
    void            Set(double x,double y) ;

    double m_x ;   ///< coordonnees x du point
    double m_y ;   ///< coordonnees y du point

    bool operator == ( const CPoint2D & pt ) const ;
    bool operator != ( const CPoint2D & pt ) const ;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Classe de calcul sur vecteur 3d.
class CVecteur2D
{
public :
    CVecteur2D() ;
    CVecteur2D(double x,double y) ;
    CVecteur2D(const CPoint2D & ptdestination , const CPoint2D & ptorigine) ;
    ~CVecteur2D() ;

    double     GetAngleRad( const CVecteur2D & Vec ) const ;
    double     GetAngleDeg( const CVecteur2D & Vec ) const ;
    double     GetNorm() const ;
    double     GetProduitScalaire( const CVecteur2D & Vec ) const ;
    bool        IsZero() const ;
    void        Set(double x,double y) ;
    void        Set(const CPoint2D & ptdestination , const CPoint2D & ptorigine) ;
    bool        IsCollinear( const CVecteur2D & Vec , bool Strict = false ) const ;

    double m_x ;   ///< coordonnees x du vecteur
    double m_y ;   ///< coordonnees y du vecteur

private :

};

// surcharge d'operateur
CVecteur2D operator + ( const CVecteur2D & VecA , const CVecteur2D & VecB ) ;    ///< addition
CVecteur2D operator - ( const CVecteur2D & VecA , const CVecteur2D & VecB ) ;    ///< soustraction
CVecteur2D operator * ( const CVecteur2D & VecA , const CVecteur2D & VecB ) ;    ///< produit vectoriel
double     operator ^ ( const CVecteur2D & VecA , const CVecteur2D & VecB ) ;    ///< produit scalaire
CVecteur2D operator * ( const CVecteur2D & Vec , double Nombre ) ;               ///< multiplication de norme
CVecteur2D operator / ( const CVecteur2D & Vec , double Nombre ) ;               ///< division de norme

#endif // _CVECTEUR_
