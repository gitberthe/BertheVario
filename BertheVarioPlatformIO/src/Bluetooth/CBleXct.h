////////////////////////////////////////////////////////////////////////////////
/// \file CBleXct.h
///
/// \brief
///
/// \date creation     : 30/01/2025
/// \date modification : 30/01/2025
///

#pragma once

#define VARIO_BLE_SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define VARIO_BLE_CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define VARIO_BLE_BUFFER_LIMIT 200

#define BLE_NAME "BertheVario"

////////////////////////////////////////////////////////////////////////////////
/// \brief Bluetooth Xc-Track Vario.
class CBleXct
{
public :
    void Init( const char * name );
    bool IsInitialised() const
        { return m_Characteristic != NULL ; } ;
    void Send() ;
    void DeviceConnect() ;
    void DeviceDisconnect() ;

private :
  unsigned int CalcChesksum( const char * Buff ) ;

  BLEServer *           m_Server = NULL ;
  BLEService *          m_Service = NULL ;
  BLECharacteristic *   m_Characteristic = NULL ;
  int                   m_DeviceCount = 0 ;
  uint8_t               m_CharBuffer[VARIO_BLE_BUFFER_LIMIT] = {0} ;
  int                   m_Pos = 0 ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe d'accet au bluetooth.
class CBleXctServerCallbacks : public BLEServerCallbacks
{

    void onConnect(BLEServer *pServer) ;
    void onDisconnect(BLEServer *pServer) ;
};
