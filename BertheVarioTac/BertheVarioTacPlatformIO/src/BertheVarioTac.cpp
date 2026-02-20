////////////////////////////////////////////////////////////////////////////////
/// \file BertheVarioTac.cpp
///
/// \brief Main du projet de vario tactile
///
/// \date creation     : 20/09/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date modification : 08/07/2025
///

#include "BertheVarioTac.h"

char g_NumVersion[] = "20260125a" ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Affiche le nom et le firmware
void AfficheEcranDebut()
{
g_tft.setBrightness( 255 ) ;
g_GlobalVar.m_Screen.ScreenRaz() ;
// nom
g_tft.setTextColor(TFT_MAGENTA) ;
g_tft.setTextSize(2) ;
g_tft.setCursor( 35 , 50 ) ;
g_tft.print("BertheVarioTac");
// version
g_tft.setTextColor(TFT_WHITE) ;
g_tft.setTextSize(3) ;
g_tft.setCursor( 40 , 100 ) ;
g_tft.print(g_NumVersion);
// batterie
char TmpChar[20] ;
float Voltage = g_GlobalVar.GetBatteryVoltage() ;
g_tft.setTextColor(TFT_CYAN) ;
g_tft.setTextSize(3) ;
g_tft.setCursor( 80 , 150 ) ;
sprintf(TmpChar,"%4.2fv",Voltage);
g_tft.print(TmpChar) ;
// porucentage
float Pourcent = (Voltage-3.7)/(4.3-3.7) * 100. ;
sprintf( TmpChar , "%3d%c", (int)Pourcent, '%' ) ;
g_tft.setCursor( 80 , 190 ) ;
g_tft.print(TmpChar) ;
}

