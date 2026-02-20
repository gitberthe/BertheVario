////////////////////////////////////////////////////////////////////////////////
/// \file CBleXct.cpp
///
/// \brief
///
/// \date creation     : 30/01/2025
/// \date modification : 17/02/2025
///

#include "../BertheVario213.h"

#ifdef XC_TRACK
CBleXctServerCallbacks varioBleServerCallbacks ;

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CBleXctServerCallbacks::onConnect(BLEServer *pServer)
{
g_GlobalVar.m_BleXct.DeviceConnect() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CBleXctServerCallbacks::onDisconnect(BLEServer *pServer)
{
g_GlobalVar.m_BleXct.DeviceDisconnect() ;
}

/*****************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CBleXct::Init( const char * name )
{
// init device
BLEDevice::init(name);

// init server
m_Server = BLEDevice::createServer();
m_Server->setCallbacks(&varioBleServerCallbacks);

// create the BLE Service
m_Service = m_Server->createService(VARIO_BLE_SERVICE_UUID);

// create the BLE Characteristic
m_Characteristic = m_Service->createCharacteristic(VARIO_BLE_CHARACTERISTIC_UUID_TX, NIMBLE_PROPERTY::NOTIFY);
//characteristic->createDescriptor("2902");

// start the service
m_Service->start();

// start advertising
m_Server->getAdvertising()->addServiceUUID(m_Service->getUUID());
m_Server->getAdvertising()->start();

//resetBuffer();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CBleXct::DeviceConnect()
{
m_DeviceCount++ ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CBleXct::DeviceDisconnect()
{
m_DeviceCount-- ;
if ( m_DeviceCount < 0 )
    m_DeviceCount = 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Envoie un buffer au bleutooth.
void CBleXct::Send()
{
// driver non initialise
if ( ! IsInitialised() )
    return ;

m_Pos = 0 ;

/*
// message test "$LK8EX1,98974,197,0,99,999*32\r\n"
const unsigned char Message[] = "$LK8EX1,98974,197,0,99,999" ;
sprintf( (char*) m_CharBuffer , "%s*%X\r\n" , Message , CalcChesksum(Message) ) ;
m_Pos = strlen( (char*) m_CharBuffer ) ;
*/

//$VARIO,pressure,vario*ch pressure(float) is in hPa and vario in dm/s or empty string.
//$VARIO,1013.25,,*45
char MessageVario[100] = "" ;
sprintf( MessageVario , "$VARIO,%.2f,," , g_GlobalVar.m_MS5611Press.GetPressureMb() ) ;
sprintf( (char*) m_CharBuffer , "%s*%X\r\n" , MessageVario , CalcChesksum(MessageVario) ) ;
m_Pos = strlen( (char*) m_CharBuffer ) ;

// write and notify
m_Characteristic->setValue( m_CharBuffer, m_Pos );
m_Characteristic->notify();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul la checksum du buffer
unsigned int CBleXct::CalcChesksum( const char * Buff )
{
int Len = strlen( Buff ) ;
unsigned int checksum , ai, bi;                                               // Calculating checksum for data string
for (checksum = 0, ai = 1; ai < Len ; ai++)
    {
    bi = (unsigned char)Buff[ai];
    checksum ^= bi;
    }
return checksum ;
}

#endif
