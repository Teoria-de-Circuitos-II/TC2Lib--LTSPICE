// How to compile?
// g++ -o ..\TCLib TCLib.cpp setup-funcs.cpp
#include <fcntl.h>
#include "setup-funcs.h"
// #include <cmrc/cmrc.hpp>

#if OS_Windows
    #include <conio.h>
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
    #if OS_Windows
        UINT8 key=0;
    #else
        uint8_t key=0;
    #endif
    int choice;
    bool break_only = true;
    std::string last_op = "Hit enter to apply";
    int opt_i=0;

    if (initLib())
    {
        return -1;
    }
    clear_screen();
    music();
    print_menu(opt_i, last_op);

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
