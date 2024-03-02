#include <iostream>
#include "setup-funcs.h"

int main()
{
    int choice;

    if (initLib())
    {
        return -1;
    }

    do
    {
        // Display menu
        std::cout << "\n[1] Just do the thing\n"
                  << "[2] Dark Theme\n"
                  << "[3] White Theme\n"
                  << "[4] Load custom components\n"
                  << "[5] Load custom background\n"
                  << "[6] Adjust line width\n"
                  << "[0] Exit\n"
                  << "Enter your choice: ";
        std::cin >> choice;

        // Call the corresponding function based on user input
        switch (choice)
        {
        case 1:
            doTheThing();
            break;
        case 2:
            setDarkTheme();
            break;
        case 3:
            setLightTheme();
            break;
        case 4:
            loadCustomComponents();
            break;
        case 5:
            loadCustomBackground();
            break;
        case 6:
            setPenWidth();
            break;
        case 0:
            std::cout << "Exiting program. Goodbye!\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
