#include "setup-funcs.h"
#include <filesystem>

#ifdef __unix__ /* __unix__ is usually defined by compilers targeting Unix systems */

#define OS_Windows 0

#elif defined(_WIN32) || defined(WIN32) /* _Win32 is usually defined by compilers targeting 32 or   64 bit Windows systems */

#define OS_Windows 1

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

void setPenWidth(const std::string &configFile, int Width)
{
    std::ifstream configFileStream(configFile);
    std::ofstream tempFileStream("temp.ini");

    std::string line;
    bool inColorsSection = false;

    while (std::getline(configFileStream, line))
    {
        if (line.find("PenWidth"))
        {
            inColorsSection = true;

            // replace the line in the configFileStream with the new one
            std::string newLine = "PenWidth=" + std::to_string(Width);
            tempFileStream << newLine << std::endl;
        }
        break;
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

void initLib()
{
    if (OS_Windows == 1)
    {
        userProfile = std::getenv("USERPROFILE");
        userRoaming = userProfile + "\\AppData\\Roaming\\";
        userLTspice = userProfile + "\\AppData\\Local\\LTspice\\";

        iniFile = userRoaming + "LTspice.ini";
        bgFile = userProfile + "\\LTspice.jpg";
    }
    else if (OS_Windows == 0)
    {
        userProfile = std::getenv("HOME");
        std::string wineAppdata = "";
        // ask user for appdata location in wine folder
        std::cout << "Ingresar la ubicacion de la carpeta AppData en la instalacion de Wine que tenga LTSpice instalado: ";
        std::cin >> wineAppdata;
        std::cout << std::endl;

        // check for trailing slash
        if (wineAppdata.back() != '/')
        {
            wineAppdata += "/";
        }

        // check if the directory exists
        if (!fs::exists(wineAppdata))
        {
            std::cerr << "Error: Wine AppData directory does not exist." << std::endl;
            return;
        }

        userRoaming = userProfile + "AppData/Roaming/";
        userLTspice = userProfile + "AppData/Local/LTspice/";

        iniFile = userRoaming + "LTspice.ini";
        bgFile = userProfile + "/LTspice.jpg";
    }
    else
    {
        std::cerr << "Error: OS not supported." << std::endl;
    }
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
    overwriteCopy(bgFilelocal, bgFile);
}