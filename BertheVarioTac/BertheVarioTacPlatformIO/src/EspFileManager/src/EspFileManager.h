#pragma once 

/*
 * Auther: Muhammad Arslan
 * Dated: 15-08-2024
 * Discription: This library provides an easy way to manage the files on the flash/sd card using webpage.
 */

#include <Arduino.h>
#include <SD.h>

#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// #define EN_DEBUG
#define DEBUG_SERIAL Serial
#include <ArDebugger.h>

#define LIBRARY_VERSION "1.0.0"

class EspFileManager
{
private:
    uint8_t sd_cs, sd_sck, sd_miso, sd_mosi;
    bool memory_ready = false;
    String str_data = "";
    fs::SDFS *_storage;
    AsyncWebServer *_server;
public:
    EspFileManager(/* args */);
    ~EspFileManager();

    // void begin(AsyncWebServer *server, FS *fs);

    bool initSDCard(SDFS *storage, uint8_t _cs);
    void setFileSource(SDFS *storage);
    void listDir(const char * dirname, uint8_t levels);

    void setServer(AsyncWebServer *server);
};

