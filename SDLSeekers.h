#ifndef SDLSEEKERS_H_INCLUDED
#define SDLSEEKERS_H_INCLUDED

#include "outermeSDL.h"
#include <dirent.h>

#define SAVE_FILE_LINES 0
#define PLAYER_NAME_LIMIT 8

typedef struct {
    sprite spr;  //?
    char name[PLAYER_NAME_LIMIT + 1];  //9 bytes
    int level;  //
    int experience;  //
    int money;  //
    int HP;  //
    int maxHP;  //
    int worldNum;  //
    int mapScreen;  //8 bytes
    int lastScreen;  //8 bytes
    int overworldX;  //
    int overworldY;  //
    SDL_RendererFlip flip;  //
    bool movementLocked;  // 1 byte
} player;

#define drawSprite(spr, flip) drawTile(spr.tileIndex, spr.x, spr.y, spr.w, flip)

void initPlayer(player* player, int x, int y, int size, int tileIndex);  //inits new player struct
void initConfig(char* filePath);  //resets config data
void loadPlayerData(player* player, char* filePath, bool forceNew);  //loads data from filePath. If not, or forceNew = true, inits new sprite.
void loadConfig(char* filePath);  //loads config data into the public variables
void loadMapFile(char* filePath, int* tilemapData[], int* eventmapData[], const int lineNum, const int y, const int x);  //loads a tilemap into the specified tilemap matrix and event matrix
int aMenu(char* title, char* opt1, char* opt2, char* opt3, char* opt4, char* opt5, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain);  //draws a menu using the colors and options presented
void saveConfig(char* filePath);  //saves config data to the file
char* uniqueReadLine(char* output[], int outputLength, const char* filePath, int lineNum);  //takes a pointer to a char array and gives you what was on the file's line in the unique location
char** getListOfFiles(const size_t maxStrings, const size_t maxLength, const char* directory, int* strNum);  //gets the list of files in a directory

#define SIZE_OF_SCANCODE_ARRAY 6
int CUSTOM_SCANCODES[SIZE_OF_SCANCODE_ARRAY];
#define SC_UP CUSTOM_SCANCODES[0]
#define SC_DOWN CUSTOM_SCANCODES[1]
#define SC_LEFT CUSTOM_SCANCODES[2]
#define SC_RIGHT CUSTOM_SCANCODES[3]
#define SC_INTERACT CUSTOM_SCANCODES[4]
#define SC_MENU CUSTOM_SCANCODES[5]

int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];

#endif // SDLSEEKERS_H_INCLUDED
