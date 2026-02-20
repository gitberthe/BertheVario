////////////////////////////////////////////////////////////////////////////////
/// \file CNumTaskArr.cpp
///
/// \brief
///
/// \date creation   : 05/05/2024
/// \date 02/01/2026 : ajout de TaskHandle_t
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \file Arrete toutes les taches sauf les scan bouton
void CNumTaskArr::StopAll()
{
// demande arret
for ( int it = 0 ; it < SIZE_TASK ; it++ )
    //if ( it != SCAN_BUTON_NUM_TASK )
        m_TaskArr[it].m_Run = false ;

// attente des arret
for ( int it = 0 ; it < SIZE_TASK ; it++ )
    {
    //if ( it != SCAN_BUTON_NUM_TASK )
        {
        if ( IsRunning(it) )
            {
            delay( DELAY_ATTENTE_IDLE ) ;
            continue ;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \file Renseigne si la tache est en cours
bool CNumTaskArr::IsRunning( int NumTask )
{
if ( m_TaskArr[NumTask].m_TaskHandle == NULL )
    return false ;

if ( eTaskGetState( m_TaskArr[NumTask].m_TaskHandle ) == eRunning )
    return true ;

if ( eTaskGetState( m_TaskArr[NumTask].m_TaskHandle ) == eReady )
    return true ;

if ( eTaskGetState( m_TaskArr[NumTask].m_TaskHandle ) == eBlocked )
    return true ;

if ( eTaskGetState( m_TaskArr[NumTask].m_TaskHandle ) == eSuspended )
    return true ;

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief retourne le TaskHandle_t de la tache pour enregistrement
TaskHandle_t * GetTaskHandle( int NumTask )
{
return & g_GlobalVar.m_TaskArr[NumTask].m_TaskHandle ;
}

////////////////////////////////////////////////////////////////////////////////
/// \file Arret de la tache appelante et consignation dans le tableau
void DeleteTask( int NumTask )
{
g_GlobalVar.m_TaskArr[NumTask].m_TaskHandle = NULL ;
vTaskDelete( NULL );
while( true ) ;
    //delay( DELAY_ATTENTE_IDLE ) ;
}
