////////////////////////////////////////////////////////////////////////////////
/// \file CScreen290.h
///
/// \brief
///
/// \date creation     : 15/03/2024
/// \date modification : 21/01/2025
///

#ifndef _SCREEN290_
#define _SCREEN290_

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet l'affichage de differentes pages d'ecrans et messages.
/// Les includes dans CSreen.cpp permetent de selectionner les differents ecrans materiel.
class CScreen290 : public CGestEcrans
{
public :
    void InitScreen() ;
    void AfficheVoltage() ;
    void AfficheConnectWifi() ;
    void AfficheWifi(char * IpAdress) ;
    void AfficheCalibreMag() ;
    void ScreenRaz( bool Froid = false ) ;
    void ScreenOff() ;
    void DoRect( int x , int y , int w , int h , bool Black ) ;
    void DoChar( int x , int y , const char * pChar ) ;

    EtatsAuto EcranVz() ;
    EtatsAuto EcranHisto() ;
    EtatsAuto EcranTmaAll() ;
    EtatsAuto EcranTmaMod() ;
    EtatsAuto EcranSys() ;
    EtatsAuto EcranCfgFch() ;
    EtatsAuto EcranListeIgcFch() ;
    EtatsAuto EcranConfimeArchIgcFch() ;
    EtatsAuto EcranTmaDessous() ;

private :
} ;

#endif
