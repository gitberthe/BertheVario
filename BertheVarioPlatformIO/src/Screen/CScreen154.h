////////////////////////////////////////////////////////////////////////////////
/// \file CScreen154.h
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 08/08/2025
///

#ifndef _SCREEN154_
#define _SCREEN154_

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet l'affichage de differentes pages d'ecrans et messages.
/// Les includes dans CSreen.cpp permetent de selectionner les differents ecrans materiel.
class CScreen154 : public CGestEcrans
{
public :
    void InitScreen() ;
    void AfficheVoltage() ;
    void AfficheConnectWifi() ;
    void AfficheWifi(char * IpAdress) ;
    void AfficheCalibreMag() ;
    void ScreenRaz() ;
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
