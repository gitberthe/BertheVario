#include "EspFileManager.h"
// #include "page.h"
#include "webPage.h"
#include "FS.h"
#include <SD.h>
#include "SPI.h"


EspFileManager::EspFileManager(/* args */)
{

}

EspFileManager::~EspFileManager()
{
}


bool EspFileManager::initSDCard(fs::SDFS *storage, uint8_t _cs)
{
    sd_cs = _cs;
    _storage = storage;

    if (!_storage->begin(sd_cs))
    // if (!_storage->begin())
    {
        DEBUGLF("Card Mount Failed");
        memory_ready = false;
        return false;
    }

    uint8_t cardType = _storage->cardType();

    if (cardType == CARD_NONE)
    {
        DEBUGLF("No SD card attached");
        memory_ready = false;
        return false;
    }

    memory_ready = true;
    DEBUGF("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        DEBUGLF("MMC");
    }
    else if (cardType == CARD_SD)
    {
        DEBUGLF("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        DEBUGLF("SDHC");
    }
    else
    {
        DEBUGLF("UNKNOWN");
    }

    if(memory_ready)
    {
        // uint32_t cardSize = _storage->cardSize() / (1024 * 1024);
        // DEBUGX("SD Card Size: %lluMB\n", cardSize);
        // DEBUGL2("SD Card Size: ", cardSize);
    }
    return false;
}

void EspFileManager::setFileSource(fs::SDFS *storage)
{   
    _storage = storage;
}

void EspFileManager::listDir(const char * dirname, uint8_t levels)
{
    DEBUGX("Listing directory: %s\n", dirname);

    File root = _storage->open(dirname);
    if(!root){
        DEBUGLF("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        DEBUGLF("Not a directory");
        return;
    }

    bool first_files = true;
    str_data = "";
    File file = root.openNextFile();
    while(file){
        if (first_files)
            first_files = false;
        else 
            str_data += ":";

        if(file.isDirectory()){
            // DEBUGF("  DIR : ");
            // DEBUGL(file.name());
            str_data += "1,";
            str_data += file.name();
            // if(levels){
            //     listDir(file.path(), levels -1);
            // }
        } else {
            // DEBUG("  FILE: ");
            // DEBUG(file.name());
            // DEBUG("  SIZE: ");
            // DEBUGL(file.size());
            str_data += "0,";
            str_data += file.name();
        }
        file = root.openNextFile();
    }

    file.close();
    // DEBUGL2("Folder string ", str_data);
}

void EspFileManager::setServer(AsyncWebServer *server)
{
    if (server == nullptr) {
        DEBUGLF("Server is null!");
        return;
    }
    _server = server;

    _server->on("/file", HTTP_GET, [&](AsyncWebServerRequest *request){ 
            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", html_page, html_page_len);
            response->addHeader("Content-Encoding", "gzip");
            request->send(response);
            // request->send(200, "text/html", html_page); 
            // request->send(200, "text/plain", "Test route working");
        });

    server->on("/get-folder-contents", HTTP_GET, [&](AsyncWebServerRequest *request){
        DEBUGL2("path:", request->arg("path").c_str());
        listDir(request->arg("path").c_str(), 0);
        request->send(200, "text/plain", str_data);
    });

    server->on("/upload", HTTP_POST, [&](AsyncWebServerRequest *request)
        { request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"File upload complete\"}"); }, [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        String file_path;

        file_path = "/";
        file_path += filename;

        if(!index)
        {
            DEBUGX("UploadStart: %s\n", file_path.c_str());
            if(_storage->exists(file_path)) 
            {
                _storage->remove(file_path);
            }
        }

        File file = _storage->open(file_path, FILE_APPEND);
        if(file) 
        {
            if(file.write(data, len) != len)
            {
                DEBUGLF("File write failed");
            }
            file.close();
        }
        if(final)
        {
            DEBUGX("UploadEnd: %s, %u B\n", file_path.c_str(), index+len);
        } 
    });

    server->on("/delete", HTTP_GET, [&](AsyncWebServerRequest *request){
        String path;
        if (request->hasParam("path")) 
        {
            path = request->getParam("path")->value();
        } 
        else 
        {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Path not provided\"}");
            return;
        }

        DEBUGL2("Deleting File: ", path);
        if (_storage->exists(path)) 
        {
            _storage->remove(path);
            request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"File deleted successfully\"}");
        } 
        else 
        {
            request->send(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
        } 
    });

    server->on("/download", HTTP_GET, [&](AsyncWebServerRequest *request){
        String path;
        if (request->hasParam("path")) 
        {
            path = request->getParam("path")->value();
        } 
        else 
        {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Path not provided\"}");
            return;
        }
        DEBUGL2("Downloading File: ", path);
        if (_storage->exists(path)) 
        {
            request->send(*_storage, path, String(), true);
        } 
        else 
        {
            request->send(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
        } 
    });
}
