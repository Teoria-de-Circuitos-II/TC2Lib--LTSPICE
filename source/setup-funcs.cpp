#include "setup-funcs.h"
#include <filesystem>
#include <unistd.h>
#include "miniaudio.h"
#include <algorithm>
#include <map>
#include <fstream>
#include <locale>
#include <codecvt>
#include <string>
#include <cstdio>

// #include "cmrc/cmrc.hpp"

#if OS_Windows

#include <windows.h>
#include <fcntl.h>
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
extern ma_engine engine;
extern float vol;
namespace fsys = std::filesystem;

// extern cmrc::embedded_filesystem fs;

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
static std::string examples = "resources/Examples/";
static std::string iniFile = "";
static std::string bgFile = "";
static std::string iniLightFilelocal = "resources/light-theme.ini";
static std::string iniDarkFilelocal = "resources/dark-theme.ini";
static std::string bgFilelocal = "resources/LTspice.jpg";
static std::string refBjtCmp = "resources/bjt.ini";
static std::string refDioCmp = "resources/dio.ini";
static std::string refJftCmp = "resources/jft.ini";
static std::string refMosCmp = "resources/mos.ini";

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

    fsys::copy_file(source, destination, fsys::copy_options::overwrite_existing);

    // auto resourceFile = fs.open(source);
    // auto memostream = memstream(const_cast<char *>(resourceFile.begin()),
    //                             const_cast<char *>(resourceFile.end()));
    // auto outstream = std::ofstream(destination, std::ios::binary);
    // outstream << memostream.rdbuf();
    // outstream.close();
}

void replaceColorsSection(const std::string &configFile, const std::string &newContentFile)
{
    std::wifstream configFileStream(configFile, std::ios::binary);
    configFileStream.imbue(std::locale(configFileStream.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

    std::wofstream tempFileStream("temp.ini", std::ios::binary);
    tempFileStream.imbue(std::locale(tempFileStream.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

    // auto resourceFile = fs.open(newContentFile);
    std::wstring line;
    bool inColorsSection = false;

    while (std::getline(configFileStream, line))
    {
        if (line.find(L"[Colors]") != std::string::npos)
        {
            inColorsSection = true;
            tempFileStream << line << std::endl;

            std::wifstream newContentFileStream(newContentFile, std::ios::binary);
            newContentFileStream.imbue(std::locale(newContentFileStream.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));
            // auto newContentFileStream = memstream(const_cast<char *>(resourceFile.begin()),
            //                                       const_cast<char *>(resourceFile.end()));
            std::wstring newContent((std::istreambuf_iterator<wchar_t>(newContentFileStream)), std::istreambuf_iterator<wchar_t>());

            // newContentFileStream.seekg(0, std::ios::end);
            // newContent.reserve(newContentFileStream.tellg());
            // newContentFileStream.seekg(0, std::ios::beg);
            // newContent.assign((std::istreambuf_iterator<char>(newContentFileStream)),
            //                   std::istreambuf_iterator<char>());

            tempFileStream << newContent << std::endl;
        }
        else if (inColorsSection && line[0] == L'[')
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
    fsys::copy_file("temp.ini", configFile);
    std::remove("temp.ini");
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
    fsys::copy_file("temp.ini", configFile.c_str());
    std::remove("temp.ini");
}

void addCmp(const std::string &referenceFile, const std::string &configFile)
{
    std::wifstream configFileStream(configFile, std::ios::binary);
    configFileStream.imbue(std::locale(configFileStream.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

    std::ifstream referenceFileStream(referenceFile);

    std::wofstream tempFileStream("temp.ini", std::ios::binary);
    tempFileStream.imbue(std::locale(tempFileStream.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

    std::wstring line;
    std::string line8;
    std::map<std::wstring, int> cmpMap;
    bool cmp_already_exists = false;
    bool found_flag = false;
    bool pasted_flag = false;
    
    while (std::getline(configFileStream, line))
    {        
        if (line.find(L"[Custon Components]") != std::string::npos)
        {
            found_flag = !found_flag;
        }
        else if (found_flag && !pasted_flag)
        {
            while (std::getline(referenceFileStream, line8))
            {
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring line8_to_16 = converter.from_bytes(line8);

                std::wstring line_lower;
                // Allocate the destination space
                line_lower.resize(line8_to_16.size());

                // Convert the source string to lower case
                // storing the result in destination string
                std::transform(line8_to_16.begin(),
                                line8_to_16.end(),
                               line_lower.begin(),
                               tolower);

                if (line_lower.find(L".model") != std::string::npos)
                {
                    auto last = line8_to_16.find_first_of(L" ", line_lower.find(L".model") + 7);
                    auto first = line_lower.find(L".model") + 7;
                    std::wstring modelName = line8_to_16.substr(first, last - first);
                    //printf(modelName.c_str());
                    cmpMap[modelName]++;
                }

                tempFileStream << line8_to_16 << std::endl;
            }
            pasted_flag = true;
        }
        else if (found_flag && pasted_flag)
        {
            continue;
        }
        else if (pasted_flag)
        {
            std::wstring line_lower;
            // Allocate the destination space
            line_lower.resize(line.size());

            // Convert the source string to lower case
            // storing the result in destination string
            std::transform(line.begin(),
                           line.end(),
                           line_lower.begin(),
                           tolower);
            if (line_lower.find(L".model") != std::string::npos)
            {
                cmp_already_exists = false;
                auto last = line.find_first_of(L" ", line_lower.find(L".model") + 7);
                auto first = line_lower.find(L".model") + 7;
                std::wstring modelName = line.substr(first, last - first);
                if (cmpMap.find(modelName) != cmpMap.end())
                {
                    cmp_already_exists = true;
                }
            }

            if (!cmp_already_exists)
            {
                tempFileStream << line << std::endl;
            }
        }
    }

    if (!pasted_flag)
    {
        configFileStream.clear();
        configFileStream.seekg(0, std::ios::beg);
        while (std::getline(referenceFileStream, line8))
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring line8_to_16 = converter.from_bytes(line8);
            if (line8_to_16.find(L".model") != std::string::npos)
            {
                auto last = line8_to_16.find_first_of(L" ", line8_to_16.find(L".model") + 7);
                auto first = line8_to_16.find(L".model") + 7;
                std::wstring modelName = line8_to_16.substr(first, last - first);
                // printf(modelName.c_str());
                cmpMap[modelName]++;
            }
            tempFileStream << line8_to_16 << std::endl;
        }
        while (std::getline(configFileStream, line))
        {
            std::wstring line_lower;
            // Allocate the destination space
            line_lower.resize(line.size());

            // Convert the source string to lower case
            // storing the result in destination string
            std::transform(line.begin(),
                           line.end(),
                           line_lower.begin(),
                           tolower);
            if (line_lower.find(L".model") != std::wstring::npos)
            {
                cmp_already_exists = false;
                auto last = line.find_first_of(L" ", line_lower.find(L".model") + 7);
                auto first = line_lower.find(L".model") + 7;
                std::wstring modelName = line.substr(first, last - first);
                if (cmpMap.find(modelName) != cmpMap.end())
                {
                    cmp_already_exists = true;
                }
            }

            if (!cmp_already_exists)
            {
                tempFileStream << line << std::endl;
            }
        }
    }

    configFileStream.close();
    referenceFileStream.close();
    tempFileStream.close();

    std::remove(configFile.c_str());
    fsys::copy_file("temp.ini", configFile.c_str());
    std::remove("temp.ini");
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

void print_menu(int opt_i, std::string last_op)
{
    std::string menuItems[] = {
        "Apply default patches",
        "Apply Dark Theme",
        "Apply White Theme",
        "Load custom components and examples",
        "Load custom background",
        "Adjust line width",
        "Credits",
        "Volume +",
        "Volume -",
        "Exit"};
    int menu_length = 10;

#if OS_Windows
    _setmode(_fileno(stdout), _O_WTEXT);
    std::wcout << L"\033[36m\n\n"
                  L"  ████████╗ ██████╗              ██╗     ██╗██████╗ \n"
                  L"  ╚══██╔══╝██╔════╝              ██║     ██║██╔══██╗\n"
                  L"     ██║   ██║         █████╗    ██║     ██║██████╔╝\n"
                  L"     ██║   ██║         ╚════╝    ██║     ██║██╔══██╗\n"
                  L"     ██║   ╚██████╗              ███████╗██║██████╔╝\n"
                  L"     ╚═╝    ╚═════╝              ╚══════╝╚═╝╚═════╝ \n";
    // Credits
    std::wcout << L"          Agustín Gullino, Javier Petrucci\n\n \033[0m";
    _setmode(_fileno(stdout), _O_TEXT);
#else
    std::cout << "\n\n"
                 "  ████████╗ ██████╗              ██╗     ██╗██████╗ \n"
                 "  ╚══██╔══╝██╔════╝              ██║     ██║██╔══██╗\n"
                 "     ██║   ██║         █████╗    ██║     ██║██████╔╝\n"
                 "     ██║   ██║         ╚════╝    ██║     ██║██╔══██╗\n"
                 "     ██║   ╚██████╗              ███████╗██║██████╔╝\n"
                 "     ╚═╝    ╚═════╝              ╚══════╝╚═╝╚═════╝ \n";
    // Credits
    std::cout << "          Agustín Gullino, Javier Petrucci\n\n \033[0m";
#endif

    // Display menu
    std::cout << "\n\n";
    std::cout << "    Use arrows to navigate.";
    std::cout << "\n\n";
    for (int i = 0; i < menu_length; i++)
    {

        if (i == opt_i)
        {
            std::cout << "\033[31m -> "; // Set color and print arrow next to the selected option
        }
        else
        {
            std::cout << "    ";
        }

        std::cout << menuItems[i];
        std::cout << "\n";

        if (i == opt_i)
        {
            std::cout << "\033[0m"; // Reset color to default
        }
    }

    std::cout << "\n\n    \033[32m->" + last_op + "<-\033[0m \n\n";
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
#if OS_Windows
// Convert an ANSI string to a wide Unicode String
std::wstring ansi2unicode(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
#endif
bool initLib()
{
#if OS_Windows
    if (!IsElevated())
    {

        _setmode(_fileno(stdout), _O_WTEXT);
        std::wcout << L"\n\n"
                      L"                            ████████                          \n"
                      L"                          ██        ██                        \n"
                      L"                        ██            ██                      \n"
                      L"                        ██            ██                      \n"
                      L"                      ██    ████████    ██                    \n"
                      L"                    ██    ████████████    ██                  \n"
                      L"                    ██    ████████████    ██                  \n"
                      L"                  ██      ████████████      ██                \n"
                      L"                ██          ████████          ██              \n"
                      L"                ██          ████████          ██              \n"
                      L"              ██            ████████            ██            \n"
                      L"            ██                ████                ██          \n"
                      L"            ██                ████                ██          \n"
                      L"          ██                                        ██        \n"
                      L"        ██                    ████                    ██      \n"
                      L"        ██                  ████████                  ██      \n"
                      L"      ██                  ████████████                  ██    \n"
                      L"    ██                    ████████████                    ██  \n"
                      L"    ██                      ████████                      ██  \n"
                      L"    ██                        ████                        ██  \n"
                      L"      ██                                                ██    \n"
                      L"        ████████████████████████████████████████████████      \n";

        _setmode(_fileno(stdout), _O_TEXT);

        std::cout << std::endl
                  << std::endl
                  << "  TCLib for Windows ";
        std::cout << std::endl
                  << std::endl
                  << "  ERROR: Please run this program as an administrator. " << std::endl
                  << std::endl;
        std::cout << "  Press any key to exit..." << std::endl;
        _getch(); // Wait for user input
        return true;
    }

    std::string userProfilewindows = std::getenv("USERPROFILE");
    std::wstring userProfileunicode = ansi2unicode(userProfilewindows);
    std::string userProfile = utf8_encode(userProfileunicode);
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

    // Copy sub and sym folders to LTspice folder in AppData repleacing files
    copyFolder("sub", userLTspice + "lib/sub");
    copyFolder("sym", userLTspice + "lib/sym");
    copyFolder(examples, userLTspice + "examples");

    addCmp(refBjtCmp, userLTspice + "lib/cmp/standard.bjt");
    addCmp(refDioCmp, userLTspice + "lib/cmp/standard.dio");
    addCmp(refJftCmp, userLTspice + "lib/cmp/standard.jft");
    addCmp(refMosCmp, userLTspice + "lib/cmp/standard.mos");
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

int music()
{
    ma_result result;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS)
    {
        printf("Failed to initialize audio engine.");
        return -1;
    }
    ma_engine_set_volume(&engine, vol);
    ma_engine_play_sound(&engine, "resources/winrar_music.mp3", NULL);

    return 0;
}

void printWeather()
{

    clear_screen();

#if OS_Windows
    _setmode(_fileno(stdout), _O_WTEXT);

    std::wcout << L"\033[34m";

    std::wcout << L"\n\n"
                  L"                                 ████                                 \n"
                  L"                             ▒████▒▓████                              \n"
                  L"                          ▒████▓      █████░                          \n"
                  L"                      ░▓████▒            ▓████▓                       \n"
                  L"                  ░██████░                  ▒██████                   \n"
                  L"          ▒▓█████████▒                          ▓█████████▓▒          \n"
                  L"  ████████████▓▒░                                    ░▒▓████████████  \n"
                  L"   ██▒                                                         ███   \n"
                  L"   ▓██░   █▓          █████████████    █████████   ████████    ▒██    \n"
                  L"    ██▓   ████▓            ███      ▓███      ░█  ██▒   ███    ███    \n"
                  L"    ▓██   ████████        ░██▒     ▓██▒                ▓██    ░██░    \n"
                  L"    ░██░  ███████████     ███      ███               ▓██▒     ▒██     \n"
                  L"     ██▒  ████████▒       ███      ███             ▒██░       ▓██     \n"
                  L"     ██▒  █████          ▒██▒      ███▓    ███▓  ░██▓   ▓▓    ▓██     \n"
                  L"     ██▓  █▓             ███░       ▓████████░  █████████     ▓██     \n"
                  L"     ██▓                                                      ███     \n"
                  L"     ██▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░███     \n"
                  L"     █████████▓▒▒█████▓▓▓▓▓▓███████                           ▓██     \n"
                  L"     ███████▒      ███      ███████             ▓██░█         ▓██     \n"
                  L"     ████████████▒ ▒██ ▓███████████          ▓█████ ███       ▓██     \n"
                  L"    ▒███████████░  ██▒      ███████         ░██▓    ░░░░      ▒██     \n"
                  L"    ▓████████▓   ██████████  ██████       ███▓ ██▒ ████▓      ░██░    \n"
                  L"    ▓███████▒  ████████████  ██████        ████▓  ▓████        ██▒    \n"
                  L"    ████████   ░░░░▒██  ░   ███████         ▒██░ █▓  █         ██▓    \n"
                  L"    ████████████████████▓██████████             ███░           ███    \n"
                  L"    ███▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░███    \n"
                  L"    ██▓                ▓███▒░█     ███████████████████████████████    \n"
                  L"    ▓██              ░█░   ░█▒     ███▓   ██████   ░█████████████░    \n"
                  L"     ███             ██  ░▓██      ███▓▒  █████    ░████████████▓     \n"
                  L"      ███            ▓████▓        ████▓  ███▓  █░ ░███████████▓      \n"
                  L"       ▓███            ░████▓      ████▓  ██░ ░██░ ░██████████░       \n"
                  L"         ███▓       █ ██▓  ██      ████▓  █▒         ████████         \n"
                  L"          ░███▓    ░█▒    ▒█       ████▓  ███████░ ░███████           \n"
                  L"            ░███▓  ▒▓░████▒        █████  ███████░ ▒█████             \n"
                  L"               ████                ████████████████████               \n"
                  L"                 ████              ██████████████████                 \n"
                  L"                   ▓███▒           ███████████████▒                   \n"
                  L"                     ▒███▓         █████████████░                     \n"
                  L"                        ████       ███████████                        \n"
                  L"                          ████░    ████████▓                          \n"
                  L"                            ▓███▒  ██████▒                            \n"
                  L"                              ▒████████░                              \n"
                  L"                                ░████                                 \n";

    std::wcout << L"\033[33m ";
    std::wcout << L"Credits:\n\n";
    std::wcout << L"\033[31m ";
    std::wcout << L"TC-Lib: Agustín Gullino, Javier Petrucci\n\n";
    std::wcout << L"\033[32m ";
    std::wcout << L"Patcher: Agustín Fisher, Agustín Gullino, Javier Petrucci";

    _setmode(_fileno(stdout), _O_TEXT);
#else
    std::cout << "\n\n"
                 "  ████████╗ ██████╗              ██╗     ██╗██████╗ \n"
                 "  ╚══██╔══╝██╔════╝              ██║     ██║██╔══██╗\n"
                 "     ██║   ██║         █████╗    ██║     ██║██████╔╝\n"
                 "     ██║   ██║         ╚════╝    ██║     ██║██╔══██╗\n"
                 "     ██║   ╚██████╗              ███████╗██║██████╔╝\n"
                 "     ╚═╝    ╚═════╝              ╚══════╝╚═╝╚═════╝ \n";
    // Credits
    std::cout << "          Agustín Gullino, Javier Petrucci\n\n \033[0m";
    std::cout << "  TC-Lib: Agustín Gullino, Javier Petrucci\n";
    std::cout << "  Patcher: Agustín Fisher, Agustín Gullino, Javier Petrucci\n";
#endif

    std::string userInput;
#if OS_Windows
    std::getline(std::cin, userInput);
#else
    char c;
    read(STDIN_FILENO, &c, 1);
#endif
    // std::string command = "curl -m 10 -s \"wttr.in/?1qF&lang=zh\"";
    // char buffer[128];
    // std::deque<std::string> forecast;

    // FILE *pipe = popen(command.c_str(), "r");
    // if (!pipe)
    // {
    //     std::cerr << "popen() failed!";
    //     return;
    // }

    // while (fgets(buffer, sizeof buffer, pipe) != NULL)
    // {
    //     if (forecast.size() == 27)
    //     {
    //         forecast.pop_front();
    //     }
    //     forecast.push_back(buffer);
    // }

    // pclose(pipe);

    // for (const auto &line : forecast)
    // {
    //     #if OS_Windows

    //         std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    //         std::wstring wide = converter.from_bytes(line);

    //         _setmode(_fileno(stdout), _O_WTEXT);
    //         std::cout << line;
    //         _setmode(_fileno(stdout), _O_TEXT);
    //     #else
    //         std::cout << line;
    //     #endif
    // }
}

int hasResources()
{
    std::filesystem::path folderPath = "resources";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}