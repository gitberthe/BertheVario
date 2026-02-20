///////////////////////////////////////////////////////////////////////////////
/// \file CDroite3D.cpp
/// \date 07/01/2017 : date de creation
///
/// \brief
///
/// \date 20/10/2020 : NVIDIA_FUNC
/// \date 10/04/2024: Derniere modification.
///

#include "../BertheVario213.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur par defaut.
CDroite3D::CDroite3D()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur a partir d'un vecteur directeur et d'un point.
CDroite3D::CDroite3D(const CVecteur3D & VecDir, const CPoint3D & pta )
{
Set(VecDir,pta) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur a partir de 2 points.
CDroite3D::CDroite3D(const CPoint3D & pta, const CPoint3D & ptb )
{
Set(pta,ptb) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CDroite3D::Set(const CPoint3D & ptorigine, const CPoint3D & ptdestination )
{
m_VecDir.Set(ptorigine,ptdestination) ;
m_pta = ptorigine ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CDroite3D::Set(const CVecteur3D & VecDir, const CPoint3D & pta )
{
m_VecDir = VecDir ;
m_pta=pta;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief destructeur vide.
CDroite3D::~CDroite3D()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi si le point est dans la droite.
bool CDroite3D::IsPointInDroite( const CPoint3D& pt ) const
{
CVecteur3D VecAP(m_pta,pt) ;
return VecAP.IsCollinear(m_VecDir) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie un point de la droite. Calcul sous forme parametrique.
/// \param t [in] : le parametre de l'equation parametrique.
const CPoint3D CDroite3D::GetPoint( float t ) const
{
CPoint3D pt ;
pt.m_x = m_VecDir.m_x*t + m_pta.m_x ;
pt.m_y = m_VecDir.m_y*t + m_pta.m_y ;
pt.m_z = m_VecDir.m_z*t + m_pta.m_z ;
return pt ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi la plus courte distance point/droite.
/// d=|AB^VecDir|/|VectDir| (internet)
float CDroite3D::GetDistanceAuPoint(const CPoint3D& ptb) const
{
CVecteur3D VecAB( ptb.m_x-m_pta.m_x , ptb.m_y-m_pta.m_y , ptb.m_z-m_pta.m_z ) ;
CVecteur3D VecProduitVectoriel = VecAB.GetProduitVectoriel( m_VecDir );
float Distance=VecProduitVectoriel.GetNorm()/m_VecDir.GetNorm();
return Distance ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Donne le point projete perpendiculairement de pt sur la droite (internet).
/// \param pt [in] : point a projeter sur la droite.
/// \return Point projete perpendiculairement.
const CPoint3D CDroite3D::GetProjectionDuPoint(const CPoint3D& pt) const
{
//              dx(xp-xa)+dy(yp-ya)+dz(zp-za)
//k = kmini = ---------------------------------
//                       dx²+dy²+dz²

float NUM = m_VecDir.m_x*(pt.m_x-m_pta.m_x)+m_VecDir.m_y*(pt.m_y-m_pta.m_y)+m_VecDir.m_z*(pt.m_z-m_pta.m_z) ;
float DEN = POW(m_VecDir.m_x,2.)+POW(m_VecDir.m_y,2.)+POW(m_VecDir.m_z,2.) ;
float k_mini=NUM/DEN ;

CPoint3D PtProj = GetPoint(k_mini) ;
return PtProj ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi la plus courte distance droite/droite.
/// d=|det(AB,u,v)|/||u^v|| (internet)
float CDroite3D::GetDistanceADroite(const CDroite3D& db ) const
{
CVecteur3D AB(m_pta,db.m_pta) ;
CMatrice3D Matrice( AB , m_VecDir , db.m_VecDir ) ;
float Determinant=FABS(Matrice.GetDeterminant()) ;
float DEN=m_VecDir.GetProduitVectoriel(db.m_VecDir).GetNorm() ;
return Determinant/DEN ;
}
