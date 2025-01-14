////////////////////////////////////////////////////////////////////////////////
/// \file CTrame.h
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 14/01/2025
///

#ifndef _TRAME_
#define _TRAME_

////////////////////////////////////////////////////////////////////////////////
/// \brief Gestion trame Gps et rempissage variables CGlobalVar g_GlobalVar.
class CTrame
{
public :
    void Push(char c) ;
    bool IsGpsOk() const
        { return m_MillisPremierGGA != 0 ; } ;
    void ForceGpsOk()
        { m_MillisPremierGGA = 1 ; } ;

    static int GetNbSat()   ///< renvoi le nombre de satellites en vue
        { return m_NombreSatellite ; } ;


protected :
    static unsigned long m_MillisPremierGGA ;   ///< temps du premier GGA pour stabilisation donnees gps
    static int           m_NombreSatellite ;    ///< nombre de satellites pour le GGA

private :
    void TraiteBuffer() ;

    char m_BufRecep[300] = {0} ;    ///< buffer de reception de la trame
    int  m_iBuf = 0 ;               ///< i position buffer reception
} ;

#endif
