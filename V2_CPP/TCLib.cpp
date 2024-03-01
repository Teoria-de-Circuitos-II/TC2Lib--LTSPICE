#include <iostream>
#include <filesystem> // For std::filesystem
#include "setup-funcs.h"

int main()
{
    int choice;

    do
    {
        // Display menu
        std::cout << "\n[1] Just do the thing\n"
                  << "[2] Dark Theme\n"
                  << "[3] White Theme\n"
                  << "[4] Load custom components\n"
                  << "[5] Load custom background\n"
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
        case 0:
            std::cout << "Exiting program. Goodbye!\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}

// // Define your functions here (implementations go separately)
// void doTheThing()
// {
//     // Implement your logic for "Just do the thing"
//     std::cout << "Doing the thing...\n";
// }

// void setDarkTheme()
// {
//     // Implement your logic for setting dark theme
//     std::cout << "Setting dark theme...\n";
// }

// void setWhiteTheme()
// {
//     // Implement your logic for setting white theme
//     std::cout << "Setting white theme...\n";
// }

// void loadCustomComponents()
// {
//     // Implement your logic for loading custom components
//     std::cout << "Loading custom components...\n";
// }

// void loadCustomBackground()
// {
//     // Get the user's profile directory
//     const char *userProfile = std::getenv("USERPROFILE");
//     if (userProfile == nullptr)
//     {
//         std::cerr << "Error: USERPROFILE environment variable not found.\n";
//         return;
//     }

//     // Construct the destination path
//     std::filesystem::path destinationPath(userProfile);
//     destinationPath /= "LTspice.jpg"; // Append the filename

//     // Check if the destination file already exists
//     if (std::filesystem::exists(destinationPath))
//     {
//         // Delete the existing file
//         try
//         {
//             std::filesystem::remove(destinationPath);
//             std::cout << "Existing LTspice.jpg removed.\n";
//         }
//         catch (const std::filesystem::filesystem_error &e)
//         {
//             std::cerr << "Error removing existing file: " << e.what() << std::endl;
//             return;
//         }
//     }

//     // Construct the source path (assuming the executable is in the same directory)
//     std::filesystem::path sourcePath = std::filesystem::current_path() / "resources" / "LTspice.jpg";

//     // Copy the new file
//     try
//     {
//         std::filesystem::copy_file(sourcePath, destinationPath, std::filesystem::copy_options::overwrite_existing);
//         std::cout << "LTspice.jpg copied to " << destinationPath << std::endl;
//     }
//     catch (const std::filesystem::filesystem_error &e)
//     {
//         std::cerr << "Error copying file: " << e.what() << std::endl;
//     }
// }
