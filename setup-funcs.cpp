#include "setup-funcs.h"
#include <filesystem>

#ifdef __unix__ /* __unix__ is usually defined by compilers targeting Unix systems */

#define OS_Windows 0

#elif defined(_WIN32) || defined(WIN32) /* _Win32 is usually defined by compilers targeting 32 or   64 bit Windows systems */

#define OS_Windows 1
#include <windows.h>
BOOL IsElevated()
{
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);

        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize))
        {
            fRet = Elevation.TokenIsElevated;
        }
    }

    if (hToken)
    {
        CloseHandle(hToken);
    }

    return fRet;
}
#endif

namespace fs = std::filesystem;

static std::string userProfile = "";
static std::string userRoaming = "";
static std::string userLTspice = "";
static std::string iniFile = "";
static std::string bgFile = "";
static std::string iniLightFilelocal = "resources/light-theme.ini";
static std::string iniDarkFilelocal = "resources/dark-theme.ini";
static std::string bgFilelocal = "resources/LTspice.jpg";

// Extra functions

void overwriteCopy(std::string source, std::string destination)
{
    if (std::filesystem::exists(destination))
    {
        // Delete the existing file
        try
        {
            std::filesystem::remove(destination);
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << "Error removing existing file while copying: " << e.what() << std::endl;
            return;
        }
    }

    // Copy background file to userProfile folder

    fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
}

void replaceColorsSection(const std::string &configFile, const std::string &newContentFile)
{
    std::ifstream configFileStream(configFile);
    std::ofstream tempFileStream("temp.ini");

    std::string line;
    bool inColorsSection = false;

    while (std::getline(configFileStream, line))
    {
        if (line == "[Colors]")
        {
            inColorsSection = true;
            tempFileStream << line << std::endl;

            std::ifstream newContentFileStream(newContentFile);
            std::string newContent;
            newContentFileStream.seekg(0, std::ios::end);
            newContent.reserve(newContentFileStream.tellg());
            newContentFileStream.seekg(0, std::ios::beg);
            newContent.assign((std::istreambuf_iterator<char>(newContentFileStream)),
                              std::istreambuf_iterator<char>());

            tempFileStream << newContent << std::endl;
        }
        else if (inColorsSection && line[0] == '[')
        {
            inColorsSection = false;
        }

        if (!inColorsSection)
        {
            tempFileStream << line << std::endl;
        }
    }

    configFileStream.close();
    tempFileStream.close();

    std::remove(configFile.c_str());
    std::rename("temp.ini", configFile.c_str());
}

void changeIniParameter(const std::string &configFile, std::string parameter, int value)
{
    std::ifstream configFileStream(configFile);
    std::ofstream tempFileStream("temp.ini");

    std::string line;

    while (std::getline(configFileStream, line))
    {
        if (line.find(parameter) != std::string::npos)
        {
            // replace the line in the configFileStream with the new one
            std::string newLine = parameter + "=" + std::to_string(value);
            tempFileStream << newLine << std::endl;
            continue;
        }
        else
        {
            tempFileStream << line << std::endl;
        }
    }

    configFileStream.close();
    tempFileStream.close();

    std::remove(configFile.c_str());
    std::rename("temp.ini", configFile.c_str());
}

void copyFolder(const fs::path &source, const fs::path &destination)
{
    if (!fs::exists(destination))
    {
        fs::create_directory(destination);
    }

    for (const auto &entry : fs::directory_iterator(source))
    {
        const auto &sourcePath = entry.path();
        const auto &destinationPath = destination / sourcePath.filename();

        if (fs::is_directory(sourcePath))
        {
            copyFolder(sourcePath, destinationPath);
        }
        else
        {
            if (fs::exists(destinationPath))
            {
                fs::remove(destinationPath);
            }
            fs::copy_file(sourcePath, destinationPath);
        }
    }
}

// Menu Functions

bool initLib()
{
#if OS_Windows
    if (!IsElevated())
    {
        std::cout << "Please run this program as an administrator." << std::endl;
        return true;
    }

    userProfile = std::getenv("USERPROFILE");
    userRoaming = userProfile + "\\AppData\\Roaming\\";
    userLTspice = userProfile + "\\AppData\\Local\\LTspice\\";

    iniFile = userRoaming + "LTspice.ini";
    bgFile = userProfile + "\\LTspice.jpg";


#else

    std::string wineuser = "";
    // ask user for appdata location in wine folder
    std::cout << "Enter the path to the Wine user directory where LTspice is installed:";
    std::cin >> wineuser;
    std::cout << std::endl;

    // check for trailing slash
    if (wineuser.back() != '/')
    {
        wineuser += "/";
    }

    // check if the directory exists
    if (!fs::exists(wineuser))
    {
        std::cerr << "Error: Wine user directory does not exist." << std::endl;
        return;
    }
    userProfile = wineuser;
    userRoaming = wineuser + "AppData/Roaming/";
    userLTspice = wineuser + "AppData/Local/LTspice/";

    iniFile = userRoaming + "LTspice.ini";
    bgFile = wineuser + "/LTspice.jpg";
#endif

    return false;
}

void doTheThing()
{
    std::cout << "Doing the thing..." << std::endl;
    setDarkTheme();
    loadCustomComponents();
    loadCustomBackground();
}

void setDarkTheme()
{
    replaceColorsSection(iniFile, iniDarkFilelocal);
}

void setLightTheme()
{
    replaceColorsSection(iniFile, iniLightFilelocal);
}

void loadCustomComponents()
{
    // estas carpetas no se si estan en el lugar correcto

    // Copy sub and sym folders to LTspice folder in AppData repleacing files
    copyFolder("resources/sub", userLTspice + "sub");
    copyFolder("resources/sym", userLTspice + "sym");
}

void loadCustomBackground()
{
    changeIniParameter(iniFile, "MDIbackgroundImage", 3);
    overwriteCopy(bgFilelocal, bgFile);
}

void setPenWidth()
{
    changeIniParameter(iniFile, "PenWidth", 2);
}