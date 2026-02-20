////////////////////////////////////////////////////////////////////////////////
/// \file FileMgr.cpp
///
/// \brief Fonction du file manager
///
/// \date creation     : 04/04/2024
/// \date modification : 12/01/2025
///

#include "../BertheVario213.h"

const word filemanagerport = 8080;
// we want a different port than the webserver
ESPFMfGK * pfilemgr = NULL ; // (filemanagerport);

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction du serveur de fichier
void addFileSystems()
{
  // This adds the Storage into the Filemanager. You have to at least call one of those.
  // If you don't, begin() will fail. Because a Filemanager without files is useless.

  /*  //<-- Addd space there like this
  if (FFat.begin(true)) {
    if (!filemgr.AddFS(FFat, "Flash/FFat", false)) {
      Serial.println(F("Adding FFAT failed."));
    }
  } else {
    Serial.println(F("FFat File System not inited."));
  }
  */

  /*
  if (SD_MMC.begin("/sdcard", true)) {
    if (!filemgr.AddFS(SD_MMC, "SD-MMC-Card", false)) {
      Serial.println(F("Adding SD_MMC failed."));
    }
  } else {
    Serial.println(F("SD_MMC File System not inited."));
  }
  */

  /*
  //const byte SS = 5;  // D8
  //if (SD.begin(SS))
  //{ */
    if (! pfilemgr->AddFS(SD, "SD-Card", true)) {
      Serial.println(F("Adding SD failed."));
    } /*
  //} else {
  //  Serial.println(F("SD File System not inited."));
  //}
  */
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction du serveur de fichier
uint32_t checkFileFlags(fs::FS &fs, String filename, uint32_t flags) {

  // this will hide files
  //filename.toLowerCase();
  if ( filename.endsWith(".hgt") || filename.endsWith(".HGT") ) {
    // no other flags, file is invisible and nothing allowed
    return ESPFMfGK::flagIsNotVisible;
  }

  // Checks if target file name is valid for action. This will simply allow everything by returning the queried flag
  if (flags & ESPFMfGK::flagIsValidAction) {
    return flags & (~ESPFMfGK::flagIsValidAction);
  }

  // Checks if target file name is valid for action.
  if (flags & ESPFMfGK::flagIsValidTargetFilename) {
    return flags & (~ESPFMfGK::flagIsValidTargetFilename);
  }

  // Default actions
  uint32_t defaultflags = ESPFMfGK::flagCanDelete | // ESPFMfGK::flagCanRename | // ESPFMfGK::flagCanGZip |  // ^t
                          ESPFMfGK::flagCanDownload | ESPFMfGK::flagCanUpload | ESPFMfGK::flagCanEdit | // ^t
                          ESPFMfGK::flagAllowPreview;

  // can gzip igc
  //if ( filename.endsWith(".igc") ) {
  //  defaultflags |= ESPFMfGK::flagCanGZip ;
  //}

  // editable files.
  const String extedit[] PROGMEM = { ".txt" } ; //, ".igc" };

  // I simply assume, that editable files are also allowed to be previewd
  for (int i = 0; i < sizeof(extedit) / sizeof(extedit[0]); i++) {
    if (filename.endsWith(String(extedit[i]))) {
      defaultflags |= ESPFMfGK::flagCanEdit | ESPFMfGK::flagAllowPreview;
      break;
    }
  }

  return defaultflags;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction du serveur de fichier
void setupFilemanager()
{
  // See above.
  pfilemgr->checkFileFlags = checkFileFlags;

  pfilemgr->WebPageTitle = "FileManager";
  pfilemgr->BackgroundColor = "white";
  pfilemgr->textareaCharset = "accept-charset=\"utf-8\"";

  if ((WiFi.status() == WL_CONNECTED) && (pfilemgr->begin())) {
    Serial.print(F("Open Filemanager with http://"));
    Serial.print(WiFi.localIP());
    Serial.print(F(":"));
    Serial.print(filemanagerport);
    Serial.print(F("/"));
    Serial.println();
  } else {
    Serial.print(F("Filemanager: did not start"));
  }
}
