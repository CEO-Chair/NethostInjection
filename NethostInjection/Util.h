#pragma once
#include "framework.h"
#include <string>

class Util
{
public:
    static std::wstring StringToWide(const std::string& str)
    {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    static std::string WideToString(const std::wstring& wstr)
    {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }

    static std::string GetDirectory(const std::string& path)
    {
        size_t wfound = path.find_last_of("/\\");
        if (wfound != -1) return (path.substr(0, wfound));
        size_t lfound = path.find_last_of("/");
        if (lfound != -1) return (path.substr(0, lfound));
        return path;
    }
};