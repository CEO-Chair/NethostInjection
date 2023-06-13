#include "DotNetHost.h"

bool DotNetHost::LoadHost()
{
    // Load the hostfxr library and get its exports
    if (!LoadHostFXR())
    {
        return false;
    }

    //Initialize hostfxr with the path to the managed dll runtimeconfig
    hostfxr_handle context = nullptr;
    int rc = HostFXRInitFn((Util::StringToWide(Entry::GetDllRootDirectory()) + L"\\ManagedLibrary\\ManagedLibrary.runtimeconfig.json").c_str(), nullptr, &context);

    if (rc != 0 || context == nullptr)
    {
        return false;
    }

    // Retrieve the LoadAssemblyAndGetFunctionPointer function from hostfxr
    void* loadAssemblyFn;
    rc = HostFXRGetRuntimeDelegateFn(
        context,
        hdt_load_assembly_and_get_function_pointer,
        &loadAssemblyFn);

    if (rc != 0 || loadAssemblyFn == nullptr)
    {
        return false;
    }

    LoadAssemblyAndGetFunctionPointer = (load_assembly_and_get_function_pointer_fn)loadAssemblyFn;

    // Close the hostfxr context
    HostFXRCloseFn(context);

    return LoadAndStartManagedAssembly();
}

bool DotNetHost::LoadAndStartManagedAssembly()
{
    std::wstring managedLibraryPath = (Util::StringToWide(Entry::GetDllRootDirectory()) + L"\\ManagedLibrary\\ManagedLibrary.dll");

    // Actually load the library and get the entry function

    EntryDelegate entry = NULL;
    int rc = LoadAssemblyAndGetFunctionPointer(
        managedLibraryPath.c_str(),
        // Specify the fully qualified type name and assembly
        L"ManagedLibrary.Loader, ManagedLibrary",
        // Function to get pointer of inside of the specified type
        L"Entry",
        // Delegate that defines the signature of the entry function
        // This is not required, but it can be useful later when we want to pass parameters or something to the entry function
        L"ManagedLibrary.Loader+EntryDelegate, ManagedLibrary",
        NULL,
        (void**)&entry);

    if (rc != 0)
    {
        return false;
    }

    entry();

    return true;
}

bool DotNetHost::LoadHostFXR()
{
    // Since we keep nethost.dll in the same directory as our dll, we delay the loading of it until here
    // We also tell the linker to delay the loading of nethost.dll in the properties
    LoadLibrary((Entry::GetDllRootDirectory() + "\\nethost.dll").c_str());

    // Get the hostfxr.dll path from nethost.dll
    char_t hostFXRPath[MAX_PATH];
    size_t bufferSize = sizeof(hostFXRPath) / sizeof(char_t);
    int rc = get_hostfxr_path(hostFXRPath, &bufferSize, nullptr);
    if (rc != 0) return false;

    // Load the hostfxr library based on the path we got from nethost.dll
    HMODULE lib = LoadLibraryW(hostFXRPath);

    // Get the important exports from hostfxr.dll
    HostFXRInitFn = (hostfxr_initialize_for_runtime_config_fn)GetExport(lib, "hostfxr_initialize_for_runtime_config");
    HostFXRGetRuntimeDelegateFn = (hostfxr_get_runtime_delegate_fn)GetExport(lib, "hostfxr_get_runtime_delegate");
    HostFXRCloseFn = (hostfxr_close_fn)GetExport(lib, "hostfxr_close");

    // Return false if any of these functions is null
    return (HostFXRInitFn && HostFXRGetRuntimeDelegateFn && HostFXRCloseFn);
}

void* DotNetHost::GetExport(HMODULE hMod, const char* name)
{
    return GetProcAddress(hMod, name);
}
