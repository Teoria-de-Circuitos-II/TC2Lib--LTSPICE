#include <iostream>
#include <fcntl.h>
#include "setup-funcs.h"
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(TCLib);

#include <stdio.h>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// Music engine
ma_engine engine;
cmrc::embedded_filesystem fs = cmrc::TCLib::get_filesystem();

int main()
{
    int choice;
    std::string last_op = "";

    if (initLib())
    {
        return -1;
    }
    clear_screen();
    music();
    print_menu(last_op);

    do
    {
        std::cin >> choice;
        clear_screen();

        // Call the corresponding function based on user input
        switch (choice)
        {
        case 1:
            doTheThing();
            last_op = "All patches applied.";
            break;
        case 2:
            setDarkTheme();
            last_op = "Dark theme applied.";
            break;
        case 3:
            setLightTheme();
            last_op = "Light theme applied.";
            break;
        case 4:
            loadCustomComponents();
            last_op = "Custom components loaded.";
            break;
        case 5:
            loadCustomBackground();
            last_op = "Custom background applied.";
            break;
        case 6:
            setPenWidth();
            last_op = "Pen width fix applied.";
            break;
        case 0:
            std::cout << "  Exiting program. Goodbye!\n";
            break;
        default:
            std::cout << "  Invalid choice. Please try again.\n";
        }

        print_menu(last_op);

    } while (choice != 0);

    // End music engine
    ma_engine_uninit(&engine);

    return 0;
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

    ma_engine_play_sound(&engine, "resources/winrar_music.mp3", NULL);

    return 0;
}
