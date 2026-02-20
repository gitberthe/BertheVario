///////////////////////////////////////////////////////////////////////////////
/// \file CVecteur2D.cpp
/// \date 05/01/2017 : date de creation
///
/// \brief
///
/// \date 20/10/2020 : NVIDIA_FUNC
/// \date 10/04/2024 : Derniere modification.
///

#include "../BertheVario213.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Addition de 2 vecteurs.
CVecteur2D operator + ( const CVecteur2D & VecA , const CVecteur2D & VecB )
{
return CVecteur2D( VecA.m_x+VecB.m_x , VecA.m_y+VecB.m_y ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Soustraction de 2 vecteurs.
CVecteur2D operator - ( const CVecteur2D & VecA , const CVecteur2D & VecB )
{
return CVecteur2D( VecA.m_x-VecB.m_x , VecA.m_y-VecB.m_y ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Multiplication d'un vecteur.
CVecteur2D operator * ( const CVecteur2D & Vec , float Nombre )
{
return CVecteur2D( Vec.m_x*Nombre , Vec.m_y*Nombre ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Division d'un vecteur.
CVecteur2D operator / ( const CVecteur2D & Vec , float Nombre )
{
return CVecteur2D( Vec.m_x/Nombre , Vec.m_y/Nombre ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Produit scalaire de 2 vecteurs <a,b>.
float operator ^ ( const CVecteur2D & VecA , const CVecteur2D & VecB )
{
return VecA.GetProduitScalaire( VecB ) ;
}

/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// \brief Positionne le point.
void CPoint2D::Set(float x,float y)
{
m_x=x;
m_y=y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Donne la distance entre 2 points.
float CPoint2D::GetDistanceAuPoint( const CPoint2D & pt ) const
{
float Distance = SQRT(POW(m_x-pt.m_x,2.) + POW(m_y-pt.m_y,2.) ) ;
return Distance ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
const CPoint2D CPoint2D::GetPointLePlusProche( const CPoint2D & pta , const CPoint2D & ptb ) const
{
if ( GetDistanceAuPoint(pta) < GetDistanceAuPoint(ptb) )
    return pta ;
return ptb ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
const CPoint2D CPoint2D::GetPointLePlusLoin( const CPoint2D & pta , const CPoint2D & ptb ) const
{
if ( GetDistanceAuPoint(pta) > GetDistanceAuPoint(ptb) )
    return pta ;
return ptb ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CPoint2D::SetPointMilieu( const CPoint2D & pta , const CPoint2D & ptb )
{
CPoint2D PtMilieu ;
PtMilieu.m_x = ( pta.m_x + ptb.m_x )/2. ;
PtMilieu.m_y = ( pta.m_y + ptb.m_y )/2. ;

(*this) = PtMilieu ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
bool CPoint2D::operator == ( const CPoint2D & pt ) const
{
if ( FABS( m_x - pt.m_x ) > PRECISION_ZERO )
    return false ;
if ( FABS(m_y - pt.m_y) > PRECISION_ZERO )
    return false ;
return true ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
bool CPoint2D::operator != ( const CPoint2D & pt ) const
{
return ! ((*this) == pt) ;
}

/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur
CVecteur2D::CVecteur2D()
{
m_x=m_y=0.;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur
CVecteur2D::CVecteur2D(float x,float y)
{
m_x=x;
m_y=y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Positionne le vecteur.
void CVecteur2D::Set(float x,float y)
{
m_x=x;
m_y=y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur vecteur AB= ptdestination - ptorigine.
CVecteur2D::CVecteur2D(const CPoint2D & ptdestination , const CPoint2D & ptorigine )
{
Set(ptdestination , ptorigine) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CVecteur2D::Set(const CPoint2D & ptorigine , const CPoint2D & ptdestination )
{
m_x = ptdestination.m_x - ptorigine.m_x ;
m_y = ptdestination.m_y - ptorigine.m_y;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Destructeur
CVecteur2D::~CVecteur2D()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Definit si le vecteur est null.
bool CVecteur2D::IsZero() const
{
if ( FABS(m_x) < PRECISION_ZERO &&
     FABS(m_y) < PRECISION_ZERO )
    return true ;
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Retourne si les vecteurs sont collineaires.
/// Probleme de bruit de calcul contourne avec PRECISION_ZERO.
bool CVecteur2D::IsCollinear( const CVecteur2D & Vec , bool Strict /*= false */) const
{
float Precision = PRECISION_ZERO*1000. ; // 0.01
if ( Strict )
    Precision = PRECISION_ZERO ; // 0.00001
float rap=Vec.m_x/m_x ;
if ( FABS(rap - (Vec.m_y/m_y)) > Precision )
    return false ;
return true ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Retourne l'angle entre les 2 vecteur en radians (internet).
float CVecteur2D::GetAngleRad( const CVecteur2D & Vec ) const
{
// ASSERT( GetNorm() > 0. ) ;
// ASSERT( Vec.GetNorm() > 0. ) ;
float CosA = GetProduitScalaire(Vec)/(GetNorm()*Vec.GetNorm()) ;
if (CosA > 1. )
    CosA = 1.;
if( CosA < -1. )
    CosA = -1.;
float AngleRad=ACOS(CosA) ;
return AngleRad;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Retourne l'angle entre les 2 vecteur en degres (internet).
float CVecteur2D::GetAngleDeg( const CVecteur2D & Vec ) const
{
float AngleDeg=RAD_2_DEG(GetAngleRad(Vec)) ;
return AngleDeg ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Retourne la longeur du vecteur (internet).
float CVecteur2D::GetNorm() const
{
float Norme=SQRT(m_x*m_x + m_y*m_y) ;
return Norme ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Calcul le produit scalaire de 2 vecteurs, qui est nul si les vecteurs
/// sont orthogonos. u.v = |u|*|v|*COS(angle entre u et v) (internet).
/// ou note <u,v> parfois
float CVecteur2D::GetProduitScalaire( const CVecteur2D & Vec ) const
{
float PScalaire=m_x*Vec.m_x + m_y*Vec.m_y ;
return PScalaire ;
}
