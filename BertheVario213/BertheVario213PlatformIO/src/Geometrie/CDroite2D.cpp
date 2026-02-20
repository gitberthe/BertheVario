///////////////////////////////////////////////////////////////////////////////
/// \file CDroite2D.cpp
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
CDroite2D::CDroite2D()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur a partir d'un vecteur directeur et d'un point.
CDroite2D::CDroite2D(const CVecteur2D & VecDir, const CPoint2D & pta )
{
Set(VecDir,pta) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur a partir de 2 points.
CDroite2D::CDroite2D(const CPoint2D & pta, const CPoint2D & ptb )
{
Set(pta,ptb) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CDroite2D::Set(const CPoint2D & ptorigine, const CPoint2D & ptdestination )
{
m_VecDir.Set(ptorigine,ptdestination) ;
m_pta = ptorigine ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CDroite2D::Set(const CVecteur2D & VecDir, const CPoint2D & pta )
{
m_VecDir = VecDir ;
m_pta=pta;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief destructeur vide.
CDroite2D::~CDroite2D()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi si le point est dans la droite.
bool CDroite2D::IsPointInDroite( const CPoint2D& pt ) const
{
CVecteur2D VecAP(m_pta,pt) ;
return VecAP.IsCollinear(m_VecDir) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie un point de la droite. Calcul sous forme parametrique.
/// \param t [in] : le parametre de l'equation parametrique.
const CPoint2D CDroite2D::GetPoint( float t ) const
{
CPoint2D pt ;
pt.m_x = m_VecDir.m_x*t + m_pta.m_x ;
pt.m_y = m_VecDir.m_y*t + m_pta.m_y ;
return pt ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Donne le point projete perpendiculairement de pt sur la droite (internet).
/// \param pt [in] : point a projeter sur la droite.
/// \return Point projete perpendiculairement.
const CPoint2D CDroite2D::GetProjectionDuPoint(const CPoint2D& pt) const
{
//            dx(xp-xa)+dy(yp-ya)
//k = kmini = -------------------
//                 dx²+dy²

float NUM = m_VecDir.m_x*(pt.m_x-m_pta.m_x)+m_VecDir.m_y*(pt.m_y-m_pta.m_y) ;
float DEN = POW(m_VecDir.m_x,2.)+POW(m_VecDir.m_y,2.) ;
float k_mini=NUM/DEN ;

CPoint2D PtProj = GetPoint(k_mini) ;
return PtProj ;
}

/*
///////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi la plus courte distance droite/droite.
/// d=|det(AB,u,v)|/||u^v|| (internet)
float CDroite2D::GetDistanceADroite(const CDroite2D& db ) const
{
CVecteur2D AB(m_pta,db.m_pta) ;
CMatrice Matrice( AB , m_VecDir , db.m_VecDir ) ;
float Determinant=FABS(Matrice.GetDeterminant()) ;
float DEN=m_VecDir.GetProduitVectoriel(db.m_VecDir).GetNorm() ;
return Determinant/DEN ;
}
*/
