////////////////////////////////////////////////////////////////////////////////
/// \file CMutex.h
///
/// \brief
///
/// \date creation     : 12/03/2024
/// \date modification : 12/03/2024
///

#ifndef _CMYMUTEX_
#define _CMYMUTEX_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de mutex
class CMutex
{
public :
    CMutex() ;

    void PrendreMutex() ;
    void RelacherMutex() ;

private :
    SemaphoreHandle_t m_shared_var_mutex = NULL ;
} ;

#endif

