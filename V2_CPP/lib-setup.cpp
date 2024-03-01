#include "setup-funcs.h"

int main() {
    int choice;
    bool exitMenu = false;

    while (!exitMenu) {
        std::cout << " _____ _____   _     _ _     \n|_   _/  __ \\ | |   (_) |    \n  | | | /  \\/ | |    _| |__  \n  | | | |     | |   | | '_ \\ \n  | | | \\__/\\ | |___| | |_) |\n  \\_/  \\____/ \\_____/_|_.__/ \n " << std::endl;
        std::cout << "Menu Options:" << std::endl;
        std::cout << "[1] Do the thing" << std::endl;
        std::cout << "[2] Set dark theme" << std::endl;
        std::cout << "[3] Set light theme" << std::endl;
        std::cout << "[4] Load custom components" << std::endl;
        std::cout << "[5] Load custom background" << std::endl;
        std::cout << "[6] Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
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
                // loadCustomBackground();
                break;
            case 6:
                exitMenu = true;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }

        std::cout << std::endl;
    }

    return 0;
}
