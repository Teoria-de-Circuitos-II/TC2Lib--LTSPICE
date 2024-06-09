//Requirement
//Install MSYS2 https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe
//pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
//-
// How to compile?
// g++ -static TCLib.cpp setup-funcs.cpp -o ..\TCLib.exe
#include <fcntl.h>
#include "setup-funcs.h"
// #include <cmrc/cmrc.hpp>


#if OS_Windows
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>

    struct termios orig_termios;

    void disableRawMode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    }

    void enableRawMode() {
        tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(disableRawMode);

        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }
#endif

// CMRC_DECLARE(TCLib);

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// Music engine
ma_engine engine;
float vol = 0.2;
// cmrc::embedded_filesystem fs = cmrc::TCLib::get_filesystem();

int main()
{
    if (!hasResources()) {
        std::cout << "Error: Missing resources. Please make sure the resources folder is in the same directory as the executable." << std::endl;
        std::string userInput;
        std::getline(std::cin, userInput);
        return -1;
    }

    int opt_i=0;
    #if OS_Windows
        UINT8 key=0;
    #else
        uint8_t key=0;
    #endif
    int choice;
    bool break_only = true;
    std::string last_op = "Hit enter to apply";

    if (initLib())
    {
        return -1;
    }
    clear_screen();
    music();
    print_menu(opt_i, last_op);

    #if !OS_Windows
        enableRawMode();
    #endif
    do
    {
        #if OS_Windows
        
        while ((key != '\n')&&(key != '\r'))
        {
 
            key=_getch();

            if (key == 0 || key == 224) {
            // Arrow key detected
            char arrow = _getch(); // Read the second character

            switch (arrow) {
                case 80: // Down arrow
                    opt_i = (++opt_i + 10) % 10;
                    clear_screen();
                    print_menu(opt_i, last_op);
                    break;
                case 72: // Up arrow
                    opt_i = (--opt_i + 10) % 10;
                    clear_screen();
                    print_menu(opt_i, last_op);
                    break;
            }
            }
        }
        key=0;
        #else

            char c;
            while (read(STDIN_FILENO, &c, 1) == 1) {
                if (c == '\x1b') {
                    char seq[3];
                    if (read(STDIN_FILENO, &seq[0], 1) != 1) return -1;
                    if (read(STDIN_FILENO, &seq[1], 1) != 1) return -1;

                    if (seq[0] == '[') {
                        switch (seq[1]) {
                            case 'A': // Up
                                opt_i = (--opt_i + 10) % 10;
                                clear_screen();
                                print_menu(opt_i, last_op);
                                break;
                            case 'B': // Down
                                opt_i = (++opt_i + 10) % 10;
                                clear_screen();
                                print_menu(opt_i, last_op);
                                break;
                        }
                }
                } else if (c == '\n') {
                    break;
                }
            }
        #endif

        choice=opt_i;

        // Call the corresponding function based on user input
        switch (choice)
        {
        case 0:
            doTheThing();
            last_op = "All patches applied";
            break;
        case 1:
            setDarkTheme();
            last_op = "Dark theme applied";
            break;
        case 2:
            setLightTheme();
            last_op = "Light theme applied";
            break;
        case 3:
            loadCustomComponents();
            last_op = "Custom components loaded";
            break;
        case 4:
            loadCustomBackground();
            last_op = "Custom background applied";
            break;
        case 5:
            setPenWidth();
            last_op = "Pen width fix applied";
            break;
        case 6:
            printWeather();
            break;
        case 7:
            vol = vol * 1.4;
            ma_engine_set_volume(&engine, vol);
            break;
        case 8:
            vol = vol * 0.6;
            ma_engine_set_volume(&engine, vol);
            break;
        case 9:
            last_op = "Exiting program. Goodbye!";
            break;
        default:
            last_op = "Select a valid option";
        }

        clear_screen();
        print_menu(opt_i, last_op);

    } while (choice != 9);

    // End music engine
    ma_engine_uninit(&engine);
    return 0;
}
