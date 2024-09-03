////////////////////////////////////////////////////////////////////////////////
/// \file CTrame.h
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 03/09/2024
///

#ifndef _TRAME_
#define _TRAME_

////////////////////////////////////////////////////////////////////////////////
/// \brief Gestion trame Gps
class CTrame
{
public :
    void Push(char c) ;

    static int GetNbSat()
        { return m_NombreSatellite ; } ;

protected :
    static unsigned long m_MillisPremierGGA ;   ///< temps du premier GGA pour stabilisation donnees gps
    static int           m_NombreSatellite ;    ///< nombre de satellites pour le GGA

private :
    void TraiteBuffer() ;

    char m_BufRecep[1000] = {0} ;   ///< buffer de reception de la trame
    int  m_iBuf = 0 ;               ///< i position buffer reception
} ;

#endif
