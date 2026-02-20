///////////////////////////////////////////////////////////////////////////////
/// \file CVecteur3D.cpp
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
CVecteur3D operator + ( const CVecteur3D & VecA , const CVecteur3D & VecB )
{
return CVecteur3D( VecA.m_x+VecB.m_x , VecA.m_y+VecB.m_y , VecA.m_z+VecB.m_z ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Soustraction de 2 vecteurs.
CVecteur3D operator - ( const CVecteur3D & VecA , const CVecteur3D & VecB )
{
return CVecteur3D( VecA.m_x-VecB.m_x , VecA.m_y-VecB.m_y , VecA.m_z-VecB.m_z ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Multiplication d'un vecteur.
CVecteur3D operator * ( const CVecteur3D & Vec , float Nombre )
{
return CVecteur3D( Vec.m_x*Nombre , Vec.m_y*Nombre , Vec.m_z*Nombre ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Division d'un vecteur.
CVecteur3D operator / ( const CVecteur3D & Vec , float Nombre )
{
return CVecteur3D( Vec.m_x/Nombre , Vec.m_y/Nombre , Vec.m_z/Nombre ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Produit vectoriel de 2 vecteurs.
CVecteur3D operator * ( const CVecteur3D & VecA , const CVecteur3D & VecB )
{
return VecA.GetProduitVectoriel( VecB ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Produit scalaire de 2 vecteurs <a,b>.
float operator ^ ( const CVecteur3D & VecA , const CVecteur3D & VecB )
{
return VecA.GetProduitScalaire( VecB ) ;
}

/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// \brief Positionne le point.
void CPoint3D::Set(float x,float y,float z)
{
m_x=x;
m_y=y;
m_z=z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Donne la distance entre 2 points.
float CPoint3D::GetDistanceAuPoint( const CPoint3D & pt ) const
{
float Distance = SQRT(POW(m_x-pt.m_x,2.) +
                        POW(m_y-pt.m_y,2.) +
                        POW(m_z-pt.m_z,2.) ) ;
return Distance ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
const CPoint3D CPoint3D::GetPointLePlusProche( const CPoint3D & pta , const CPoint3D & ptb ) const
{
if ( GetDistanceAuPoint(pta) < GetDistanceAuPoint(ptb) )
    return pta ;
return ptb ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
const CPoint3D CPoint3D::GetPointLePlusLoin( const CPoint3D & pta , const CPoint3D & ptb ) const
{
if ( GetDistanceAuPoint(pta) > GetDistanceAuPoint(ptb) )
    return pta ;
return ptb ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CPoint3D::SetPointMilieu( const CPoint3D & pta , const CPoint3D & ptb )
{
CPoint3D PtMilieu ;
PtMilieu.m_x = ( pta.m_x + ptb.m_x )/2. ;
PtMilieu.m_y = ( pta.m_y + ptb.m_y )/2. ;
PtMilieu.m_z = ( pta.m_z + ptb.m_z )/2. ;

(*this) = PtMilieu ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
bool CPoint3D::operator == ( const CPoint3D & pt ) const
{
if ( FABS( m_x - pt.m_x ) > PRECISION_ZERO )
    return false ;
if ( FABS(m_y - pt.m_y) > PRECISION_ZERO )
    return false ;
if ( FABS(m_z - pt.m_z) > PRECISION_ZERO )
    return false ;
return true ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
bool CPoint3D::operator != ( const CPoint3D & pt ) const
{
return ! ((*this) == pt) ;
}

/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur
CVecteur3D::CVecteur3D()
{
m_x=m_y=m_z=0.;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur
CVecteur3D::CVecteur3D(float x,float y,float z)
{
m_x=x;
m_y=y;
m_z=z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Positionne le vecteur.
void CVecteur3D::Set(float x,float y,float z)
{
m_x=x;
m_y=y;
m_z=z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur vecteur AB= ptdestination - ptorigine.
CVecteur3D::CVecteur3D(const CPoint3D & ptdestination , const CPoint3D & ptorigine )
{
Set(ptdestination , ptorigine) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CVecteur3D::Set(const CPoint3D & ptorigine , const CPoint3D & ptdestination )
{
m_x = ptdestination.m_x - ptorigine.m_x ;
m_y = ptdestination.m_y - ptorigine.m_y;
m_z = ptdestination.m_z - ptorigine.m_z;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Destructeur
CVecteur3D::~CVecteur3D()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Definit si le vecteur est null.
bool CVecteur3D::IsZero() const
{
if ( FABS(m_x) < PRECISION_ZERO &&
     FABS(m_y) < PRECISION_ZERO &&
     FABS(m_z) < PRECISION_ZERO )
    return true ;
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Retourne si les vecteurs sont collineaires.
/// Probleme de bruit de calcul contourne avec PRECISION_ZERO.
bool CVecteur3D::IsCollinear( const CVecteur3D & Vec , bool Strict /*= false */) const
{
float Precision = PRECISION_ZERO*1000. ; // 0.01
if ( Strict )
    Precision = PRECISION_ZERO ; // 0.00001
float rap=Vec.m_x/m_x ;
if ( FABS(rap - (Vec.m_y/m_y)) > Precision )
    return false ;
if ( FABS(rap - (Vec.m_z/m_z)) > Precision )
    return false ;
return true ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Retourne l'angle entre les 2 vecteur en radians (internet).
float CVecteur3D::GetAngleRad( const CVecteur3D & Vec ) const
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
float CVecteur3D::GetAngleDeg( const CVecteur3D & Vec ) const
{
float AngleDeg=RAD_2_DEG(GetAngleRad(Vec)) ;
return AngleDeg ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Retourne la longeur du vecteur (internet).
float CVecteur3D::GetNorm() const
{
float Norme=SQRT(m_x*m_x + m_y*m_y + m_z*m_z) ;
return Norme ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Calcul le produit scalaire de 2 vecteurs, qui est nul si les vecteurs
/// sont orthogonos. u.v = |u|*|v|*COS(angle entre u et v) (internet).
/// ou note <u,v> parfois
float CVecteur3D::GetProduitScalaire( const CVecteur3D & Vec ) const
{
float PScalaire=m_x*Vec.m_x + m_y*Vec.m_y + m_z*Vec.m_z ;
return PScalaire ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Retourne un vecteur perpendiculaire a l'instance.
/// \param x [in] : composante x du vecteur
/// \param y [in] : composante y du vecteur
const CVecteur3D CVecteur3D::GetPerpendicular(float x, float y ) const
{
float z=1. ;

if ( IsZero() )
    {
    // ASSERT( 1 == 0 ) ;
    return *this ;
    }

if ( FABS(m_z) > 0. )
    {
    float m_z_z=-m_x*x-m_y*y ;
    float z = m_z_z/m_z ;
    CVecteur3D Vec(x,y,z) ;
    // ASSERT( Vec.GetNorm() > 0. ) ;
    // ASSERT( FABS( this->GetAngleDeg(Vec) - 90. ) < PRECISION_ZERO ) ;
    return Vec ;
    }
else if ( FABS(m_y) > 0. )
    {
    float m_y_y=-m_x*x-m_z*z ;
    float y = m_y_y/m_y ;
    CVecteur3D Vec(x,y,z) ;
    // ASSERT( Vec.GetNorm() > 0. ) ;
    // ASSERT( FABS( this->GetAngleDeg(Vec) - 90. ) < PRECISION_ZERO ) ;
    return Vec ;
    }
else if ( FABS(m_x) > 0. )
    {
    float m_x_x=-m_y*y-m_z*z ;
    float x = m_x_x/m_x ;
    CVecteur3D Vec(x,y,z) ;
    // ASSERT( Vec.GetNorm() > 0. ) ;
    // ASSERT( FABS( this->GetAngleDeg(Vec) - 90. ) < PRECISION_ZERO ) ;
    return Vec ;
    }

// ASSERT( 1 == 0 ) ;
return *this ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Calcul le produit vectoriel de 2 vecteur, qui est un troisieme
/// vecteur w=u^v orthogonal aux 2 premiers |w|=|u|*|v|*abs(SIN(angle entre u et v))
/// (internet).
const CVecteur3D CVecteur3D::GetProduitVectoriel( const CVecteur3D & Vec ) const
{
CVecteur3D VecProVec ;
VecProVec.m_x = m_y*Vec.m_z - m_z*Vec.m_y ;
VecProVec.m_y = m_z*Vec.m_x - m_x*Vec.m_z ;
VecProVec.m_z = m_x*Vec.m_y - m_y*Vec.m_x ;
return VecProVec ;
}
