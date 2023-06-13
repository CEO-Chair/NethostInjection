#include "Entry.h"

void Entry::Load(HMODULE hMod)
{
    static bool loaded = false;

    // Only every load once, since sometimes DllEntry is called twice
    if (loaded) return;

    loaded = true;

    // Always do actual loading in a new thread when being injected as a dll
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InternalLoad, hMod, 0, nullptr);
}

void Entry::InternalLoad(HMODULE hMod)
{
    // Store the module handle for possible? later use in unloading or something like that
    ModuleHandle = hMod;

    // Get the path of the currently module
    char dllPath[MAX_PATH];
    GetModuleFileName(ModuleHandle, dllPath, sizeof(dllPath));

    // Get the directory that this module is contained in
    DllRootDirectory = Util::GetDirectory(dllPath);

    // Set the directory that windows uses to load dll dependencies to the one this dll is located int
    // We do this since we later call LoadLibrary for nethost.dll
    // The default directory is where the executable is located
    SetDllDirectory((DllRootDirectory + "\\").c_str());

    // Only load .NET once, just in case
    if (!IsDotNetLoaded())
    {
        DotNetHost::LoadHost();
        LoadedDotNet = true;
    }
}

const std::string& Entry::GetDllRootDirectory()
{
    return DllRootDirectory;
}

const bool Entry::IsDotNetLoaded()
{
    return LoadedDotNet;
}