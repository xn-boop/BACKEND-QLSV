/*
 * ============================================================
 *  main.cpp
 *  Entry point for the Student Management System v3.0
 *
 *  Build (Windows):  g++ src/*.cpp -I include -std=c++17 -o sms.exe
 *  Run:              sms.exe
 *  Or use:           build.bat  (double-click to compile & run)
 *  VS Code:          Ctrl+Shift+B  (uses .vscode/tasks.json)
 * ============================================================
 */

#include "../include/StudentManager.h"

#include <iostream>
#include <limits>
using namespace std;

// ─────────────────────────────────────────────
//  Utility: clear terminal screen (cross-platform)
// ─────────────────────────────────────────────
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ─────────────────────────────────────────────
//  Utility: pause until Enter is pressed
// ─────────────────────────────────────────────
void pause()
{
    cout << "\n  Press Enter to return to menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ─────────────────────────────────────────────
//  Display main menu
// ─────────────────────────────────────────────
void showMenu()
{
    clearScreen();
    cout << "\n";
    cout << "  ==========================================\n";
    cout << "      STUDENT MANAGEMENT SYSTEM  v3.0\n";
    cout << "  ==========================================\n\n";
    cout << "    1.  Add Student\n";
    cout << "    2.  View Students\n";
    cout << "    3.  Search Student\n";
    cout << "    4.  Update Student\n";
    cout << "    5.  Delete Student\n";
    cout << "    6.  Sort Students\n";
    cout << "    7.  Student Analytics  (console)\n";
    cout << "    8.  Save Data\n";
    cout << "    9.  Export Analytics Report  [C++]\n";
    cout << "   10.  Exit\n\n";
    cout << "  ------------------------------------------\n";
    cout << "  Enter Choice : ";
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
int main()
{
    StudentManager manager;

    // Step 1: Admin authentication
    clearScreen();
    if (!manager.adminLogin())
        return 1;

    // Step 2: Auto-load data from file
    cout << "\n  Loading student records...\n";
    manager.loadFromFile();
    pause();

    // Step 3: Main menu loop
    int choice = 0;

    while (true)
    {
        showMenu();

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n  [!] Invalid input. Please enter a number 1-10.\n";
            pause();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice)
        {
            case 1:  clearScreen(); manager.addStudent();      pause(); break;
            case 2:  clearScreen(); manager.viewStudents();    pause(); break;
            case 3:  clearScreen(); manager.searchStudent();   pause(); break;
            case 4:  clearScreen(); manager.updateStudent();   pause(); break;
            case 5:  clearScreen(); manager.deleteStudent();   pause(); break;
            case 6:  clearScreen(); manager.sortMenu();        pause(); break;
            case 7:  clearScreen(); manager.showAnalytics();   pause(); break;
            case 8:  clearScreen(); manager.saveData();        pause(); break;
            case 9:  clearScreen(); manager.exportReport();    pause(); break;

            case 10:
                if (manager.hasUnsavedChanges())
                {
                    clearScreen();
                    cout << "\n  [i] Unsaved changes detected. Saving before exit...\n";
                    manager.saveData();
                }
                clearScreen();
                cout << "\n  Thank you for using Student Management System. Goodbye!\n\n";
                return 0;

            default:
                cout << "\n  [!] Invalid choice. Enter 1-10.\n";
                pause();
        }
    }

    return 0;
}
