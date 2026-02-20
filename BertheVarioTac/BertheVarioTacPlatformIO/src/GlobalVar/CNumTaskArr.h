////////////////////////////////////////////////////////////////////////////////
/// \file CNumTaskArr.ch
///
/// \brief
///
/// \date creation   : 05/05/2024
/// \date 02/01/2026 : ajout de TaskHandle_t
///

#ifndef _NUMTASKARR_
#define _NUMTASKARR_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe pour la gestion des marche/arret des taches.
/// A noter qu'une tache ne doit jamais retouner mais faire un :
/// while( true ) vTaskDelete(NULL) ;
class CNumTaskArr
{
public :
    struct st_run_handle
        {
        bool         m_Run = true ;        ///< pour l'arreter quand on veut
        TaskHandle_t m_TaskHandle = NULL ; ///< pour connaitre sons etat
        } ;

    struct st_run_handle m_TaskArr[SIZE_TASK] ;  ///< tableau des marche/arret taches

    void StopAll() ;
    bool IsRunning( int NumTask ) ;
} ;

TaskHandle_t * GetTaskHandle( int NumTask ) ;
void DeleteTask( int NumTask ) ;

#endif
