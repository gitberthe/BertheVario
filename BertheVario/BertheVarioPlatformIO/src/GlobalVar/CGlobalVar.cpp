////////////Ķ////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.cpp
///
/// \brief Variable globale du projet
///
/// \date creation   : 02/03/2024
/// \date 26/10/2025 : MakeEmptySound()
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
/// \brief Test la presence du hgt pdd pour verif fichier hgt carte sd
void CGlobalVar::TestPddGroundZ()
{
// puy de dome pour test fichier hgt
float Lat = 45.7722397 ;
float Lon = 2.964199  ;
//Lon = 0 ;  Lat = 39 ; // pour test
float hauteur = m_Hgt2Agl.GetGroundZ( Lon , Lat ) ;
if ( hauteur < 1440. || hauteur > 1460. )
    {
    Serial.print( "pb test pdd z : " ) ;
    Serial.println( hauteur ) ;
    BeepError(true) ;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief met a jour l'altitude sol sans mutex accet carte SD et l'altitude baro recalee
void CGlobalVar::MiseAJourAltitudesSolEtBaroRec()
{
// mise a jour altitude sol (pas en ihm mais rando vol cause multi-tache)
if ( GetEtatAuto() == ECRAN_0_Vz )
    m_AltitudeSolHgt = m_Hgt2Agl.GetGroundZ( m_TerrainPosCur.m_Lon , m_TerrainPosCur.m_Lat ) ;

// mise a jour altitude baro recalee sans mesure capteur faite par TacheVzCapMag()
m_TerrainPosCur.m_AltiBaroRec = m_MS5611Press.GetAltiRecMetres() ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Pour un gain memoire
void CGlobalVar::GainMemoire()
{
//RazGpsPos() ;
//m_PileVit.ResetVit() ;
m_HistoVol.m_HistoDir.SetSize(0) ;
m_HistoVol.m_HistoDir.FreeExtra() ;
m_Config.FreeVect() ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Relance la sequence d'enregistrement pour un nouveau fichier
void CGlobalVar::RelancerAttenteVol()
{
//Serial.println("CGlobalVar::RelancerAttenteVol()") ;
//beeper(7000, 300) ;
xTaskCreatePinnedToCore( TacheRelanceAttenteVol, "RelanceIgc", RELANCE_IGC_STACK_SIZE, NULL , RELANCE_IGC_PRIORITY ,
    GetTaskHandle(RELANCE_IGC_NUM_TASK), RELANCE_IGC_CORE );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui relance un nouveau fichier igc
void CGlobalVar::TacheRelanceAttenteVol( void * param )
{
// on vide la file d'attente du son
//g_GlobalVar.MakeEmptySound() ;

// reset pour reboot avec boutons G/D.
g_GlobalVar.m_FinDeVol.InitFinDeVol() ;

// reset debut de vol vitesse
//g_GlobalVar.m_PileVit.ResetVit() ;

// raz deco
g_GlobalVar.m_TerrainPosDeco.m_Lat =
g_GlobalVar.m_TerrainPosDeco.m_Lon =
g_GlobalVar.m_TerrainPosDeco.m_AltiBaroRec = 0 ;

// demande d'arret des taches
g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run = false ;
g_GlobalVar.m_TaskArr[ATTENTE_VOL_NUM_TASK].m_Run = false ;

// attente arret des taches
while( g_GlobalVar.IsRunning(IGC_NUM_TASK) || g_GlobalVar.IsRunning( ATTENTE_VOL_NUM_TASK ) )
    delay( DELAY_ATTENTE_IDLE ) ;

// reinit variables de boucle de taches
g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[ATTENTE_VOL_NUM_TASK].m_Run = true ;

// raz screen
//g_GlobalVar.ScreenRaz() ;

// relance tache
g_GlobalVar.LanceTacheGps(false) ;

DeleteTask( RELANCE_IGC_NUM_TASK ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
bool CGlobalVar::VerifyAllChecksumGood()
{
CLGArrayPtr<CLGString> FileHgtArray ;
FileHgtArray.SetDeleteObjet(true) ;

DoChar( 0 , 30 , "scan dir chk" ) ;

// destruction des petits fichiers
File RootDir = SD.open(REP_HGT_FILE) ;
if ( !RootDir )
    {
    Serial.println("Failed to open root dir");
    return false ;
    }

// pour toute la directorie
while( true )
    {
    // ouverture fichier
    File Fichier = RootDir.openNextFile() ;

    // si fin de liste
    if ( ! Fichier )
        break ;

    // si directorie
    if( Fichier.isDirectory() )
        {
        Fichier.close() ;
        continue ;
        }

    // si petit fichier
    FileHgtArray.Add( new CLGString( Fichier.name() ) ) ;

    Fichier.close() ;
    }
RootDir.close() ;

// si chk pas bon
FileHgtArray.Shuffle() ;
for ( int ifch = 0 ; ifch < FileHgtArray.GetSize() ; ifch++ )
    {
    char TmpChar[50] ;
    sprintf( TmpChar , "%d/%d \n%s" , ifch , FileHgtArray.GetSize() , (const char*)(*FileHgtArray[ifch]) ) ;
    DoChar( 0 , 30 , TmpChar ) ;
    if ( !CSDCard::VerifyChecksumGood(*FileHgtArray[ifch]) )
        {
        DoChar( 30 , 100 , "NOK" ) ;
        FileHgtArray.DeleteAll() ;
        return false ;
        }
    }

FileHgtArray.DeleteAll() ;
return true ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief fonction qui permet de rebooter.
/// Attention la abort() est bugguee et ne redemarre pas defois
void CGlobalVar::Reboot()
{
// si en vol ou pas au sol
if ( g_GlobalVar.m_FinDeVol.IsInFlight() || ! g_GlobalVar.m_FinDeVol.IsAtGround() )
    {
    CBoutons::SetWant2Reboot( false ) ;
    return ;
    }

// si pas bon ecran
if ( g_GlobalVar.GetEtatAuto() == EtatsAuto::ECRAN_0_Vz || g_GlobalVar.GetEtatAuto() == EtatsAuto::ECRAN_6_Sys ||
     g_GlobalVar.GetEtatAuto() == EtatsAuto::ECRAN_1_Histo )
    {
    CBoutons::SetWant2Reboot( false ) ;
    return ;
    }

g_GlobalVar.DoChar(0,160," reboot...");

// beep
g_GlobalVar.BeepError(true) ;

// raz ecran
//g_GlobalVar.ScreenOff() ;

// on stop les taches
g_GlobalVar.StopAll() ;

// reboot
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

//delay(100);
Wire.flush();
Wire.begin(VARIO_SDA_PIN, VARIO_SCL_PIN);
 //Wire.setClock(400000); //Increase I2C data rate to 400kHz
 // d'apres la FAQ 100khz evite un plantage du MS5611 capteur de pression
 // mais je pense que c'est plus le mutex qui fait que ca plante plus
Wire.setClock(100000); //Increase I2C data rate to 100kHz
//delay (250);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Permet de generer un son. Par l'intermediaire du serveur de son.
/// La tache appelante n'est donc pas bloquée.
void CGlobalVar::beeper( int frequence , int DurationMs , int Volume )
{
/* g_GlobalVar.m_MutexI2c.PrendreMutex() ;
 tone(SPEAKER_PIN, frequence, DurationMs ) ;
g_GlobalVar.m_MutexI2c.RelacherMutex() ; */
g_GlobalVar.m_MutexQueue.PrendreMutex() ;
 g_GlobalVar.m_Req.m_Frequence = frequence ;
 g_GlobalVar.m_Req.m_DelayMs   = DurationMs ;
 g_GlobalVar.m_Req.m_Cycle     = Volume ;

 g_GlobalVar.PostSoundRequest( & g_GlobalVar.m_Req ) ;
g_GlobalVar.m_MutexQueue.RelacherMutex() ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction statique de signalement que tout va bien.
void CGlobalVar::BeepOk()
{
beeper( 7000 , 100 ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction statique de signalement d'erreur.
void CGlobalVar::BeepError(bool small)
{
beeper(7000, 300) ;
beeper( SOUND_DELAY_ONLY , 500 ) ;
beeper(7000, 300) ;
if ( small )
    return ;
beeper( SOUND_DELAY_ONLY , 500 ) ;
beeper(7000, 300) ;
beeper( SOUND_DELAY_ONLY , 500 ) ;
beeper(7000, 300) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la tension batterie
float CGlobalVar::GetVoltage()
{
float Voltage = 0. ;

// mesure du voltage
for (int i = 0; i < 10; i++)
    Voltage += analogRead(VOLTAGE_DIVISOR_PIN);
Voltage /= 10;        // 10 mesures
Voltage /= 1000 / 2 ; // pont diviseur par 2

Voltage *= 4.3 / 4.62 ;  // tension lipo 1S à 4.3 v max

//#ifdef GNU_VARIO_GRIS
// Voltage -= 0.5 ;
//#endif

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
