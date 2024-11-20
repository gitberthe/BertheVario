////////////////////////////////////////////////////////////////////////////////
/// \file CFinSol.h
///
/// \brief
///
/// \date creation     : 07/03/2024
/// \date modification : 20/11/2024
///

#ifndef _FINSOL_
#define _FINSOL_

////////////////////////////////////////////////////////////////////////////////
/// \brief Determine la finesse sol
class CFinSol
{
public :
    float m_FinesseSol = -1 ;   ///< finesse sol affichee en transition (finesse de mon aile)

    void PushDistAlti( float Dist , float Alti ) ;

private :
    void InitDistAltiArr( float Dist , float Alti ) ;

    const float m_CoefFiltrage = 0.4 ;  ///< pour filtrage de la finesse (plus grand plus de filtrage)

    float * m_pDistArr = NULL ;     ///< distance pour calcul finesse sol, tableau de Vz periode integrations cases
    float * m_pAltiArr = NULL ;     ///< altitude pour calcul finesse sol, tableau de Vz periode integrations cases
    const int m_Size = 5 ;          ///< taille des tableaux en secondes
} ;

#endif
