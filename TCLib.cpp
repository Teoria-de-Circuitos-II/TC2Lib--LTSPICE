#include <iostream>
#include <fcntl.h>
#include "setup-funcs.h"

#include <stdio.h>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

//Music engine
ma_engine engine;

int main()
{
    int choice;
    std::string last_op = "";

    if (initLib())
    {
        return -1;
    }

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
            last_op="All patches applied.";
            break;
        case 2:
            setDarkTheme();
            last_op="Dark theme applied.";
            break;
        case 3:
            setLightTheme();
            last_op="Light theme applied.";
            break;
        case 4:
            loadCustomComponents();
            last_op="Custom components loaded.";
            break;
        case 5:
            loadCustomBackground();
            last_op="Custom background applied.";
            break;
        case 6:
            setPenWidth();
            last_op="Pen width fix applied.";
            break;
        case 0:
            std::cout << "  Exiting program. Goodbye!\n";
            break;
        default:
            std::cout << "  Invalid choice. Please try again.\n";
        }

        print_menu(last_op);

    } while (choice != 0);

    //End music engine
    ma_engine_uninit(&engine);

    return 0;
}


int music()
{
    ma_result result;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine.");
        return -1;
    }

    ma_engine_play_sound(&engine, "resources\\winrar_music.mp3", NULL);

    return 0;
}

void print_menu(std::string last_op){

        _setmode(_fileno(stdout), _O_WTEXT);
    std::wcout <<
        L"\n\n"
        L"  ████████╗ ██████╗              ██╗     ██╗██████╗ \n"
        L"  ╚══██╔══╝██╔════╝              ██║     ██║██╔══██╗\n"
        L"     ██║   ██║         █████╗    ██║     ██║██████╔╝\n"
        L"     ██║   ██║         ╚════╝    ██║     ██║██╔══██╗\n"
        L"     ██║   ╚██████╗              ███████╗██║██████╔╝\n"
        L"     ╚═╝    ╚═════╝              ╚══════╝╚═╝╚═════╝ \n";
    _setmode(_fileno(stdout), _O_TEXT);

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
