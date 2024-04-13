///////////////////////////////////////////////////////////////////////////////
/// \file CMatrice3D.h
/// \date 12/01/2017 : date de creation
///
/// \brief
///
/// \date 20/10/2020 : NVIDIA_FUNC
/// \date 10/04/2024 : Derniere modification.
///

#ifndef _CMATRICE3D_
#define _CMATRICE3D_

///////////////////////////////////////////////////////////////////////////////
/// \brief Classe de calcul sur les matrice 3d. Utilise par CDroite::GetDistanceADroite().
class CMatrice3D
{
public :
    CMatrice3D() ;
    CMatrice3D( const CVecteur3D & a, const CVecteur3D & b , const CVecteur3D & c) ;
    ~CMatrice3D() ;

    void Set( const CVecteur3D & a, const CVecteur3D & b , const CVecteur3D & c) ;
    void SetRotX(float AngleRad ) ;
    void SetRotY(float AngleRad ) ;
    void SetRotZ(float AngleRad ) ;
    CPoint3D Mult(const CPoint3D & Pt ) ;
    float GetDeterminant() const ;

    float * m_pa ;   ///< pointeur sur la matrice
    float * m_pb ;   ///< pointeur sur la matrice
    float * m_pc ;   ///< pointeur sur la matrice
    float * m_pd ;   ///< pointeur sur la matrice
    float * m_pe ;   ///< pointeur sur la matrice
    float * m_pf ;   ///< pointeur sur la matrice
    float * m_pg ;   ///< pointeur sur la matrice
    float * m_ph ;   ///< pointeur sur la matrice
    float * m_pi ;   ///< pointeur sur la matrice

    float m_Matrice[3][3] ;   ///< matrice 3*3 ligne/colonne

private :
    void SetCoef() ;

};

#endif
