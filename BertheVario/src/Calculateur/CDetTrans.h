////////////////////////////////////////////////////////////////////////////////
/// \file CDetTrans.h
///
/// \brief
///
/// \date creation     : 07/03/2024
/// \date modification : 12/04/2024
///

#ifndef _DET_TRANS_
#define _DET_TRANS_

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de determiner si on est en transition, avec le cap magnetique
class CDetTrans
{
public :
    void PushCap() ;
    bool IsInTrans() const ;

private :
    float *m_pCapArr = NULL ;   ///< tableau des caps
    int    m_Size = 0 ;         ///< taille tableau
} ;

#endif
