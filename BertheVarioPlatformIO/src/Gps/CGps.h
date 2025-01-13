////////////////////////////////////////////////////////////////////////////////
/// \file CGps.h
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 13/01/2025
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
    CStabGps         m_StabGps ;

    static void TacheGpsSerial(void *param);

private :
    static const int m_BeepSecondes = 5 ;    ///< secondes de beep
    unsigned long    m_MillisDebutVol = 0 ; ///< millisecondes de debut de vol

    static void TacheGpsTempsVol(void *param);
    static void TacheGpsIgc(void *param);


} ;

#endif
