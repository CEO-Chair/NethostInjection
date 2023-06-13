#pragma once
#include "framework.h"
#include <string>
#include "Util.h"
#include "DotNetHost.h"

class Entry
{
private:
    static inline bool LoadedDotNet = false;

    static inline HMODULE ModuleHandle = nullptr;

    static inline std::string DllRootDirectory = "";

    static void InternalLoad(HMODULE hMod);

public:
    static void Load(HMODULE hMod);

    static const std::string& GetDllRootDirectory();

    static const bool IsDotNetLoaded();
};