// Author: Michael Aaron Brenner 
// School: Kaisesrslautern High School
// ConwaysGameOfLife_CLI.cpp : This file contains the 'main' function. Program execution begins and ends there.
// This program is meant to be a replica/my creation of Conway's Game of Life (https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <random>
#include <Windows.h>

using namespace std;

// NOTE: Need way of loading premade map/screen/layout. Have it be read from some file or have a layout in this file that can be easily changed.
// Might be easier if converted into a Qt application... However that would kind of suck.
// Get input from console cursur. should return an X and Y value that can be used to place a cell.
int nScreenWidth;
int nScreenHeight;
int nCellSpawnChance;
int speed; //in milliseconds
int nGeneration = 0;
int xx = 0; int yy = 0;
COORD CursorPosition = { xx, yy };
void ScreenBorderCreation(wchar_t* screen);

int RunSimulation(bool edit, bool spawn) {
    
    // Sets the screen and height to the console window size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    nScreenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    nScreenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    // Create Screen Buffer and setup console window
    wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    SetConsoleTitle(L"Conway's Game of Life");
    DWORD dwBytesWritten = 0;

    bool bEditStage = edit;
    bool bSpawnStage = spawn;
    bool bLifeBegins = false;

    // A vector (aka a list) containing x and y coordinates to be used to check the surrounding locations around a given cell.
    vector<pair<int, int>> vCellSurroundings;
    int xCellSuround[] = { -1, 0, 1 };
    int yCellSuround[] = { -nScreenWidth, 0, nScreenWidth };
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            vCellSurroundings.push_back(make_pair(xCellSuround[a], yCellSuround[b]));
        }
    }
    ScreenBorderCreation(screen);

    while (true) {

        //Storage of neighboring cells per generation
        vector<int> vNeighborhood;
        vNeighborhood.clear(); //Resets storage
        
        if (bEditStage) {
            //WASD movement of cursor/Cell Placement Location
            if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
                if (CursorPosition.Y >= 0 && CursorPosition.Y <= nScreenHeight) {
                    CursorPosition.Y += 1;
                    SetConsoleCursorPosition(hConsole, CursorPosition);
                    Sleep(250);
                }
            }
            if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
                if (CursorPosition.X > 0 && CursorPosition.X <= nScreenWidth) {
                    CursorPosition.X -= 1;
                    SetConsoleCursorPosition(hConsole, CursorPosition);
                    Sleep(250);
                }
            }
            if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
                if (CursorPosition.Y > 0 && CursorPosition.Y <= nScreenHeight) {
                    CursorPosition.Y -= 1;
                    SetConsoleCursorPosition(hConsole, CursorPosition);
                    Sleep(250);
                }
            }
            if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
                if (CursorPosition.X >= 0 && CursorPosition.X <= nScreenWidth) {
                    CursorPosition.X += 1;
                    SetConsoleCursorPosition(hConsole, CursorPosition);
                    Sleep(250);
                }
            }

            //Placement of Cells
            if (GetAsyncKeyState((unsigned short)'C') & 0x8000) {
                if (screen[CursorPosition.Y * nScreenWidth + CursorPosition.X] == ' ') {
                    screen[CursorPosition.Y * nScreenWidth + CursorPosition.X] = 'X';
                }else if (screen[CursorPosition.Y * nScreenWidth + CursorPosition.X] == 'X') {
                    screen[CursorPosition.Y * nScreenWidth + CursorPosition.X] = ' ';
                }
                Sleep(250);
            }

            swprintf_s(screen, nScreenWidth, L"WASD to move cursor @ C key to place a cell in current location (Press again to remove) @ P key to run simulation");

            if (GetAsyncKeyState((unsigned short)'P') & 0x8000) {
                bEditStage = false;
                bLifeBegins = true;
                //recreates top border
                for (int s = 0; s < nScreenWidth; s++) {
                    screen[0 * nScreenWidth + s] = '@';
                }
            }
        }
        else {

            if (GetAsyncKeyState((unsigned short)'R') & 0x8000) {
                ScreenBorderCreation(screen);
                bSpawnStage = true;
                bLifeBegins = false;
            }
            // Rules to Conway's Game of Life:
            // 1.) Any live Cell with more than three live neighbors dies (Overpopulation)
            // 2.) Any live Cell with fewer than 2 neighbors dies (Underpopulation)
            // [' ', 'X', ' ',      [' ', 'X', ' ',
            //  ' ', 'X', ' ',  ->   ' ', ' ', ' ',
            //  ' ', ' ', ' ']       ' ', ' ', ' ']
            // 3.) Any Dead Cell with exactly three live neighbors becomes a living cell (Repopulation)
            // 4.) Any live cell (X) with two or three live neighbors survive to the next generation

            for (int x = 0; x < nScreenWidth; x++) {
                for (int y = 0; y < nScreenHeight; y++) {
                    // Cell Spawn Stage:
                    //      Will Create borders out of @ symbols.
                    //      Will randomly place a cell somewhere in the map.
                    if (bSpawnStage) {
                        // Random Placement of Cells
                        if (x > 0 && x < nScreenWidth - 1 && y > 0 && y < nScreenHeight - 1) {
                            random_device rd;
                            mt19937 gen(rd());
                            uniform_int_distribution<> dist(1, nCellSpawnChance);
                            int nCellSpawn = dist(gen);
                            if (nCellSpawn == 1) {
                                screen[y * nScreenWidth + x] = 'X';
                            }
                            else {
                                screen[y * nScreenWidth + x] = ' ';
                            }
                        }
                        if (x == nScreenWidth - 1 && y == nScreenHeight - 1) {
                            bSpawnStage = false;
                            bLifeBegins = true;
                        }
                    }
                    // Application of rules on Cells now that life has begun
                    if (bLifeBegins) {
                        if (y > 0 && x > 0 && y < nScreenHeight - 1 && x < nScreenWidth - 1) {
                            int counter = 0;
                            for (int i = 0; i <= 8; i++) {
                                if (vCellSurroundings[i].first != vCellSurroundings[i].second) {
                                    if (screen[(y * nScreenWidth + x) + vCellSurroundings[i].first + vCellSurroundings[i].second] == 'X') {
                                        counter++;
                                    }
                                }
                            }
                            vNeighborhood.push_back(counter);
                        }
                        if (y == nScreenHeight - 2 && x == nScreenWidth - 2) {
                            int i = 0;
                            for (int xx = 0; xx < nScreenWidth; xx++) {
                                for (int yy = 0; yy < nScreenHeight; yy++) {
                                    if (yy > 0 && xx > 0 && yy < nScreenHeight - 1 && xx < nScreenWidth - 1) {
                                        if (vNeighborhood[i] > 3) {
                                            screen[yy * nScreenWidth + xx] = ' ';
                                        }
                                        if (vNeighborhood[i] < 2) {
                                            screen[yy * nScreenWidth + xx] = ' ';
                                        }
                                        if (vNeighborhood[i] == 3) {
                                            screen[yy * nScreenWidth + xx] = 'X';
                                        }
                                        i++;
                                    }
                                }
                            }
                        }
                      //WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
                    
                    }
                }
            }
            //Displays Current Generation in top left corner
            
            swprintf_s(screen, nScreenWidth, L"Generation: %d , Cell Spawn Chance: %f , Speed(ms): %d", nGeneration, 1.0f/(float)nCellSpawnChance, speed);
            nGeneration++;
            // Control Speed of Generations
            Sleep(speed);
        }
        // Updates Screen
        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    return 0;
}

int main()
{
    int nChoice;
    bool Edit{};
    bool Spawn{};

    //User Input/Program Configurations:
    cout << "What would you like to do?\n1.) Craete\n2.) Random\nChoice: ";
    cin >> nChoice;
    if (nChoice == 1) {
        Edit = true;
        Spawn = false;
    }
    if (nChoice == 2) {
        cout << "How often would you like a cell to spawn at the start? (The higher the number, the fewer cells spawn)\nRecommended is 5.\nSpawn Chance: ";
        cin >> nCellSpawnChance;
        Edit = false;
        Spawn = true;
    }
    cout << "How long would like a Generation to last? (Recommended durration is 150 milliseconds)\nDurration: ";
    cin >> speed;
        
    RunSimulation(Edit, Spawn);
    
    return 0;
}


//Creation of borders
void ScreenBorderCreation(wchar_t *screen) {
    for (int x = 0; x < nScreenWidth; x++) {
        for (int y = 0; y < nScreenHeight; y++) {
            if (x == 0 || y == 0) {
                screen[y * nScreenWidth + x] = '@';
            }
            else if (x == nScreenWidth - 1 || y == nScreenHeight - 1) {
                screen[y * nScreenWidth + x] = '@';
            }
            if (y > 0 && x > 0 && y < nScreenHeight - 1 && x < nScreenWidth - 1) {
                screen[y * nScreenWidth + x] = ' ';
            }
        }
    }
}
