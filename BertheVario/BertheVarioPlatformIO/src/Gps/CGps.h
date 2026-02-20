////////////////////////////////////////////////////////////////////////////////
/// \file CGps.h
///
/// \brief
///
/// \date 03/03/2024 : creation
/// \date 14/10/2025 : supression de StabGps.
/// \date 30/12/2025 : TacheGpsIgcTempsHistoCalcul
///

#ifndef _CGPS_
#define _CGPS_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe qui gere le gps, le temps de vol et l'enregistrement du fichier
/// IGC.
class CGps : public CTrame , public CIgc
{
public :
    void InitGps() ;
    void LanceTacheGps( bool AvecPortSerie ) ;

    CFinDeVol        m_FinDeVol ;           /// pour la fin de vol

    static void TacheGpsSerial(void *param);

private :
    static const int m_BeepSecondes = 5 ;    ///< secondes de beep
    unsigned long    m_MillisDebutVol = 0 ; ///< millisecondes de debut de vol

    static void TacheGpsAttenteVol(void *param);
    static void TacheGpsIgcTempsHistoCalcul(void *param);
} ;

#endif
