#include "setup-funcs.h"
#include <filesystem>
#include "cmrc/cmrc.hpp"
#ifdef __unix__ /* __unix__ is usually defined by compilers targeting Unix systems */

#define OS_Windows 0

#elif defined(_WIN32) || defined(WIN32) /* _Win32 is usually defined by compilers targeting 32 or   64 bit Windows systems */

#define OS_Windows 1
#include <windows.h>
#include <conio.h>
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

namespace fsys = std::filesystem;

extern cmrc::embedded_filesystem fs;

/// Credits: https://stackoverflow.com/a/13059195
/// https://stackoverflow.com/questions/13059091/
struct membuf : std::streambuf
{
    membuf(char const *base, size_t size)
    {
        char *p(const_cast<char *>(base));
        this->setg(p, p, p + size);
    }
    virtual ~membuf() = default;
};

/// Credits: https://stackoverflow.com/a/13059195
/// https://stackoverflow.com/questions/13059091/
struct memstream : virtual membuf, std::istream
{

    memstream(char const *base, char *const end)
        : membuf(base, reinterpret_cast<uintptr_t>(end) - reinterpret_cast<uintptr_t>(base)), std::istream(static_cast<std::streambuf *>(this)) {}

    memstream(char const *base, size_t size)
        : membuf(base, size), std::istream(static_cast<std::streambuf *>(this)) {}
};

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
            std::cerr << "  Error removing existing file while copying: " << e.what() << std::endl;
            return;
        }
    }

    // Copy background file to userProfile folder
    auto resourceFile = fs.open(source);
    auto memostream = memstream(const_cast<char *>(resourceFile.begin()),
                                const_cast<char *>(resourceFile.end()));
    auto outstream = std::ofstream(destination, std::ios::binary);
    outstream << memostream.rdbuf();
    outstream.close();
}

void replaceColorsSection(const std::string &configFile, const std::string &newContentFile)
{
    std::ifstream configFileStream(configFile);
    std::ofstream tempFileStream("temp.ini");
    auto resourceFile = fs.open(newContentFile);
    std::string line;
    bool inColorsSection = false;

    while (std::getline(configFileStream, line))
    {
        if (line.find("[Colors]") != std::string::npos)
        {
            inColorsSection = true;
            tempFileStream << line << std::endl;

            // std::ifstream newContentFileStream(newContentFile);
            auto newContentFileStream = memstream(const_cast<char *>(resourceFile.begin()),
                                                  const_cast<char *>(resourceFile.end()));
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

void copyFolder(const fsys::path &source, const fsys::path &destination)
{
    if (!fsys::exists(destination))
    {
        fsys::create_directory(destination);
    }

    for (const auto &entry : fsys::directory_iterator(source))
    {
        const auto &sourcePath = entry.path();
        const auto &destinationPath = destination / sourcePath.filename();

        if (fsys::is_directory(sourcePath))
        {
            copyFolder(sourcePath, destinationPath);
        }
        else
        {
            if (fsys::exists(destinationPath))
            {
                fsys::remove(destinationPath);
            }
            fsys::copy_file(sourcePath, destinationPath);
        }
    }
}

// Menu Functions

void print_menu(std::string last_op)
{
#if OS_Windows
    _setmode(_fileno(stdout), _O_WTEXT);
    std::cout << L"\n\n"
                 L"  ████████╗ ██████╗              ██╗     ██╗██████╗ \n"
                 L"  ╚══██╔══╝██╔════╝              ██║     ██║██╔══██╗\n"
                 L"     ██║   ██║         █████╗    ██║     ██║██████╔╝\n"
                 L"     ██║   ██║         ╚════╝    ██║     ██║██╔══██╗\n"
                 L"     ██║   ╚██████╗              ███████╗██║██████╔╝\n"
                 L"     ╚═╝    ╚═════╝              ╚══════╝╚═╝╚═════╝ \n";
    _setmode(_fileno(_stdout), _O_TEXT);
#else
    std::cout << "\n\n"
                 "  ████████╗ ██████╗              ██╗     ██╗██████╗ \n"
                 "  ╚══██╔══╝██╔════╝              ██║     ██║██╔══██╗\n"
                 "     ██║   ██║         █████╗    ██║     ██║██████╔╝\n"
                 "     ██║   ██║         ╚════╝    ██║     ██║██╔══██╗\n"
                 "     ██║   ╚██████╗              ███████╗██║██████╔╝\n"
                 "     ╚═╝    ╚═════╝              ╚══════╝╚═╝╚═════╝ \n";
#endif
    // Display menu
    std::cout << "\n  [1] Apply all (recomended)\n"
              << "  [2] Apply Dark Theme\n"
              << "  [3] Apply White Theme\n"
              << "  [4] Load custom components\n"
              << "  [5] Load custom background\n"
              << "  [6] Adjust line width\n"
              << "  [0] Exit\n\n"
              << "  " + last_op + "\n\n"
              << "  Select and option: ";
}

void color(std::string text_color)
{
#if OS_Windows
    text_color = "Color 0" + text_color;
    std::system(text_color.c_str());
#else
    return;
#endif
}

void clear_screen()
{
#if OS_Windows
    std::system("cls");
#else
    // Assume POSIX
    std::system("clear");
#endif
}

bool initLib()
{
#if OS_Windows
    if (!IsElevated())
    {
        std::cout << "  Please run this program as an administrator." << std::endl;
        std::cout << "  Press any key to exit..." << std::endl;
        _getch(); // Wait for user input
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
    std::cout << "  Enter the path to the Wine user directory where LTspice is installed:";
    std::cin >> wineuser;
    std::cout << std::endl;

    // check for trailing slash
    if (wineuser.back() != '/')
    {
        wineuser += "/";
    }

    // check if the directory exists
    if (!fsys::exists(wineuser))
    {
        std::cerr << "  Error: Wine user directory does not exist." << std::endl;
        return true;
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
    setDarkTheme();
    loadCustomComponents();
    loadCustomBackground();
    setPenWidth();
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
    copyFolder("sub", userLTspice + "lib/sub");
    copyFolder("sym", userLTspice + "lib/sym");
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