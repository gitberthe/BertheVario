///////////////////////////////////////////////////////////////////////////////
/// \file CMatrice3D.cpp
/// \date 12/01/2017 : date de creation
///
/// \brief
///
/// \date 10/04/2024 : Derniere modification.
///

#include "../BertheVario213.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur
CMatrice3D::CMatrice3D()
{
SetCoef() ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur qui positionne la matrice avec 3 vecteurs
CMatrice3D::CMatrice3D( const CVecteur3D & a, const CVecteur3D & b , const CVecteur3D & c)
{
SetCoef() ;
Set(a,b,c) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Destructeur
CMatrice3D::~CMatrice3D()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Positionne les coefficients a,b,c...
void CMatrice3D::SetCoef()
{
m_pa=&m_Matrice[0][0] ; // premiere ligne
m_pb=&m_Matrice[0][1] ;
m_pc=&m_Matrice[0][2] ;

m_pd=&m_Matrice[1][0] ; // seconde ligne
m_pe=&m_Matrice[1][1] ;
m_pf=&m_Matrice[1][2] ;

m_pg=&m_Matrice[2][0] ; // troisieme ligne
m_ph=&m_Matrice[2][1] ;
m_pi=&m_Matrice[2][2] ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie le determinant de la matrice
float CMatrice3D::GetDeterminant() const
{
float Det = ( *m_pa * *m_pe * *m_pi + *m_pd * *m_ph * *m_pc + *m_pg * *m_pb * *m_pf )
            - ( *m_pg * *m_pe * *m_pc + *m_pa * *m_ph * *m_pf + *m_pd * *m_pb * *m_pi ) ;
return Det ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Positionne la matrice avec 3 vecteurs comme ceci :
/// [ ax bx cx ]
/// [ ay by cy ]
/// [ az bz cz ]
void CMatrice3D::Set( const CVecteur3D & a, const CVecteur3D & b , const CVecteur3D & c)
{
m_Matrice[0][0] = a.m_x ; // premiere ligne
m_Matrice[0][1] = b.m_x ;
m_Matrice[0][2] = c.m_x ;

m_Matrice[1][0] = a.m_y ; // seconde ligne
m_Matrice[1][1] = b.m_y ;
m_Matrice[1][2] = c.m_y ;

m_Matrice[2][0] = a.m_z ; // troisieme ligne
m_Matrice[2][1] = b.m_z ;
m_Matrice[2][2] = c.m_z ;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Positione la matrice pour une rotation autour de X.
void CMatrice3D::SetRotX( float AngleRad )
{
m_Matrice[0][0] = 1. ; // premiere ligne
m_Matrice[0][1] = 0. ;
m_Matrice[0][2] = 0. ;

m_Matrice[1][0] = 0. ; // seconde ligne
m_Matrice[1][1] =  COS(AngleRad) ;
m_Matrice[1][2] = -SIN(AngleRad) ;

m_Matrice[2][0] = 0. ; // troisieme ligne
m_Matrice[2][1] = SIN(AngleRad) ;
m_Matrice[2][2] = COS(AngleRad) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Positione la matrice pour une rotation autour de Y.
void CMatrice3D::SetRotY( float AngleRad )
{
m_Matrice[0][0] = COS(AngleRad) ; // premiere ligne
m_Matrice[0][1] = 0. ;
m_Matrice[0][2] = SIN(AngleRad) ;

m_Matrice[1][0] = 0. ; // seconde ligne
m_Matrice[1][1] = 1. ;
m_Matrice[1][2] = 0. ;

m_Matrice[2][0] = -SIN(AngleRad) ; // troisieme ligne
m_Matrice[2][1] = 0. ;
m_Matrice[2][2] =  COS(AngleRad) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Positione la matrice pour une rotation autour de Z.
void CMatrice3D::SetRotZ( float AngleRad )
{
m_Matrice[0][0] =  COS(AngleRad) ; // premiere ligne
m_Matrice[0][1] = -SIN(AngleRad) ;
m_Matrice[0][2] = 0. ;

m_Matrice[1][0] = SIN(AngleRad) ; // seconde ligne
m_Matrice[1][1] = COS(AngleRad) ;
m_Matrice[1][2] = 0. ;

m_Matrice[2][0] = 0. ; // troisieme ligne
m_Matrice[2][1] = 0. ;
m_Matrice[2][2] = 1. ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Change de repere le point.
CPoint3D CMatrice3D::Mult(const CPoint3D & Pt )
{
CPoint3D PtRet ;

PtRet.m_x = m_Matrice[0][0] * Pt.m_x + m_Matrice[0][1] * Pt.m_y + m_Matrice[0][2] * Pt.m_z ;
PtRet.m_y = m_Matrice[1][0] * Pt.m_x + m_Matrice[1][1] * Pt.m_y + m_Matrice[1][2] * Pt.m_z ;
PtRet.m_z = m_Matrice[2][0] * Pt.m_x + m_Matrice[2][1] * Pt.m_y + m_Matrice[2][2] * Pt.m_z ;

return PtRet ;
}
