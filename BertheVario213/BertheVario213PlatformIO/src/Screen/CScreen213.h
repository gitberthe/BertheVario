////////////////////////////////////////////////////////////////////////////////
/// \file CScreen213.h
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 19/03/2025
///

#ifndef _SCREEN154_
#define _SCREEN154_

#define HAUTEUR_213  (250)
#define LARGEUR_213  (122)

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet l'affichage de differentes pages d'ecrans et messages.
/// Les includes dans CSreen.cpp permetent de selectionner les differents ecrans materiel.
class CScreen213 : public CGestEcrans
{
public :
    void InitScreen() ;
    void AfficheVoltage() ;
    void AfficheBoutons() ;
    void AfficheConnectWifi() ;
    void AfficheWifi(char * IpAdress) ;
    void AfficheCalibreMag() ;
    void ScreenRaz() ;
    void ScreenOff() ;
    void DoRect( int x , int y , int w , int h ) ;
    void DoChar( int x , int y , const char * pChar ) ;

    EtatsAuto EcranVz() ;
    EtatsAuto EcranHisto() ;
    EtatsAuto EcranTmaAll() ;
    EtatsAuto EcranTmaMod() ;
    EtatsAuto EcranSys() ;
    EtatsAuto EcranOta() ;
    EtatsAuto EcranCfgFch() ;
    EtatsAuto EcranListeIgcFch() ;
    EtatsAuto EcranConfimeArchIgcFch() ;
    EtatsAuto EcranTmaDessous() ;
    void      EcranRandoVol() ;

private :
    float GetPourcentageVoltage( float volt ) const ;
} ;

#endif
