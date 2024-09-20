////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.cpp
///
/// \brief Variable globale du projet
///
/// \date creation     : 02/03/2024
/// \date modification : 20/09/2024
///

#include "../BertheVario.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
CGlobalVar::CGlobalVar()
{
strcpy(m_LatChar, "0000000") ;
strcpy(m_LonChar, "00000000") ;
m_TerrainPosCur.m_Nom = "pos-cur" ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Relance la sequence d'enregistrement pour un nouveau fichier
void CGlobalVar::RelancerEnregistrementFichier()
{
Serial.println("CGlobalVar::RelancerEnregistrementFichier()") ;
beeper(7000, 300) ;
xTaskCreatePinnedToCore( TacheRelanceIgc, "RelanceIgc", RELANCE_IGC_STACK_SIZE, NULL  , RELANCE_IGC_PRIORITY , NULL, RELANCE_IGC_CORE );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui relance un nouveau fichier igc
void CGlobalVar::TacheRelanceIgc( void * param )
{
// reset pour reboot avec boutons G/D.
g_GlobalVar.m_FinDeVol.InitFinDeVol() ;

// reset debut de vol vitesse
g_GlobalVar.m_PileVit.ResetVit() ;

// pour stabilisation gps
g_GlobalVar.RazGpsPos() ;

// demande d'arret des taches
g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run = false ;
g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run = false ;

// attente arret des taches
while( !g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Stopped || !g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Stopped )
    delay( 100 ) ;

// reinit variables de boucle de taches
g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Stopped = false ;
g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Stopped = false ;

// raz screen
g_GlobalVar.ScreenRaz() ;

// relance tache
g_GlobalVar.LanceTacheGps(false) ;

while( true )
    vTaskDelete(NULL) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief fonction qui permet de rebooter.
/// Attention la abort() est bugguee et ne redemarre pas defois
void CGlobalVar::Reboot()
{
g_GlobalVar.BeepError(true) ;

g_GlobalVar.StopAll() ;

ESP.restart();

/*while ( true )
    {
    g_GlobalVar.m_MutexCore.RelacherMutex() ;
    g_GlobalVar.m_MutexVariable.RelacherMutex() ;
    g_GlobalVar.m_MutexI2c.RelacherMutex() ;
    g_TermicMap.m_MutexMap.RelacherMutex() ;
    g_GlobalVar.m_ZonesAerAll.m_Mutex.RelacherMutex() ;
    Serial.print("CGlobalVar::Reboot()") ;
    delay( 100 ) ;
    abort() ;
    }*/

/*float * p = NULL ;
*p = 1 ;*/
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui deinitialise le wifi et bluetooth
void CGlobalVar::DeInit()
{
/*esp_bluedroid_deinit() ;
esp_bluedroid_disable() ;
esp_wifi_deinit() ; */
}

///////////////////////////////////////////////////////////////////////////////
/// \brief fonction qui met en route les alimentation imu/gps
void CGlobalVar::InitAlim()
{
#ifdef _LG_DEBUG_
 Serial.println("Init des alimentations" ) ;
#endif

// init des alimentations
pinMode(POWER_PIN, OUTPUT);
digitalWrite(POWER_PIN, POWER_PIN_STATE); // turn on POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)

// Init mesure tension batterie
pinMode(VOLTAGE_DIVISOR_PIN, INPUT);
analogReadResolution(12);

// wait for devices power on
//delay(2000);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui initialise le "Wire" avec les pin qui vont bien
/// voir aussi CSDCard::InitSDCard() pour les pin et le "SPI"
void CGlobalVar::InitI2C()
{
#ifdef _LG_DEBUG_
 Serial.println("Init I2C" ) ;
#endif

//Wire.begin();

delay(100);
Wire.flush();
Wire.begin(VARIO_SDA_PIN, VARIO_SCL_PIN);
 //Wire.setClock(400000); //Increase I2C data rate to 400kHz
 // d'apres la FAQ 100khz evite un plantage du MS5611 capteur de pression
 // mais je pense que c'est plus le mutex qui fait que ca plante plus
Wire.setClock(100000); //Increase I2C data rate to 100kHz
delay (250);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Permet de generer un son.
void CGlobalVar::beeper( int frequence , int DurationMs )
{
g_GlobalVar.m_MutexI2c.PrendreMutex() ;
 tone(SPEAKER_PIN, frequence, DurationMs ) ;
g_GlobalVar.m_MutexI2c.RelacherMutex() ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction statique de signalement que tout va bien.
void CGlobalVar::BeepOk()
{
g_GlobalVar.beeper( 7000 , 100 ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction statique de signalement d'erreur.
void CGlobalVar::BeepError(bool small)
{
delay( 500 ) ;
beeper(7000, 300) ;
delay( 500 ) ;
beeper(7000, 300) ;
if ( small )
    return ;
delay( 500 ) ;
beeper(7000, 300) ;
delay( 500 ) ;
beeper(7000, 300) ;
delay( 500 ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la tension batterie
float CGlobalVar::GetVoltage()
{
float Voltage = 0. ;

// mesure du voltage
for (int i = 0; i < 10; i++)
    Voltage += analogRead(VOLTAGE_DIVISOR_PIN);
Voltage /= 10;
Voltage /= 1000 / 2 ;

#ifdef GNU_VARIO_GRIS
 Voltage -= 0.5 ;
#endif

return Voltage ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie l'angle signe en degres entre A et B.
float CGlobalVar::GetDiffAngle( float AngleA , float AngleB )
{
// coordonnes  cartersiennes vecteur angle
float u1 = cosf(AngleA*T_PI/180.) ;
float u2 = sinf(AngleA*T_PI/180.) ;
float v1 = cosf(AngleB*T_PI/180.) ;
float v2 = sinf(AngleB*T_PI/180.) ;

// produit vectoriel
float z3 = u1*v2 - u2*v1 ;

// produit scalaire
float & x1 = u1 ;
float & x2 = u2 ;
float & y1 = v1 ;
float & y2 = v2 ;
float produit_scalaire = x1*y1 + x2*y2 ;

// angle entre les 2 vecteurs
float Angle = acosf( produit_scalaire ) * 180./T_PI ;

// signe de l'angle
if ( z3 < 0. )
    return Angle ;
return -Angle ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la derive en degres
float CGlobalVar::GetDeriveDeg()
{
return GetDiffAngle( m_CapGpsDeg , m_Mpu9250.m_CapMagnetique ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static watch dog de 20s de la fonction loop().
void CGlobalVar::TacheWatchDog(void* param)
{

while( g_GlobalVar.m_TaskArr[WATCH_DOG_NUM_TASK].m_Run )
    {
    delay( 2000 ) ;

    // mesure du temps
    unsigned long temps = millis() - g_GlobalVar.m_MillisWatchDog ;

    // watch dog de 20s
    if ( temps/1000 > 20 )
        CGlobalVar::Reboot() ;

    /*// verif alti
    if ( g_GlobalVar.m_AltiGps < -100 )
        CGlobalVar::Reboot() ;

    // verif latitude
    if ( fabs ( g_GlobalVar.m_LatFloat ) > 95.  )
        CGlobalVar::Reboot() ;

    // verif longitude
    if ( fabs ( g_GlobalVar.m_LonFloat ) > 185.  )
        CGlobalVar::Reboot() ;*/
    }

g_GlobalVar.m_TaskArr[WATCH_DOG_NUM_TASK].m_Stopped = true ;
}
