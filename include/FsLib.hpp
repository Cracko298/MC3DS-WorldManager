#pragma once
#include "Directory.hpp"
#include "InputFile.hpp"
#include "OutputFile.hpp"
#include <3ds.h>
#include <string>

namespace FsLib
{
    // Opens and mounts sdmc
    bool Initialize(void);
    // Exits and closes all open FS_Archives.
    void Exit(void);
    // Returns internal error string with slightly more information than a bool.
    const char *GetErrorString(void);
    // Processes path string for use internally with FsLib. No need to use yourself, but I'm not gonna stop you.
    bool ProcessPath(const std::u16string &PathIn, FS_Archive *ArchiveOut, std::u16string &PathOut);
    // Opens the respective Archive type. Might add more if I can remember how later.
    bool OpenSaveData(const std::u16string &DeviceName); // This is only really useful for old *hax
    bool OpenExtData(const std::u16string &DeviceName, uint32_t ExtDataID);
    bool OpenSharedExtData(const std::u16string &DeviceName, uint32_t SharedExtDataID);
    bool OpenSystemSaveData(const std::u16string &DeviceName, uint32_t UniqueID);
    bool OpenGamecardSaveData(const std::u16string &DeviceName);
    bool OpenUserSaveData(const std::u16string &DeviceName, FS_MediaType MediaType, uint32_t LowerID, uint32_t UpperID);
    // More here maybe some day.
    // Controls archive associate with DeviceName. Only needed for save data. Extdata does not need this.
    bool ControlDevice(const std::u16string &DeviceName);
    // Closes archive associated with DeviceName.
    bool CloseDevice(const std::u16string &DeviceName);
} // namespace FsLib
