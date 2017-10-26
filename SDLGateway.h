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
    int mapScreen;  //8 bytes
    SDL_RendererFlip flip;  //
    bool movementLocked;  // 1 byte
    char* extraData;
} player;

typedef enum {
    script_none,              //0 default, do nothing
    script_trigger_dialogue,  //1 if player steps in coords, trigger a dialogue/text box
    script_trigger_boss,      //2 if player steps in coords, spawn boss
    script_switch_rooms,      //3 triggers a switching of rooms. Map borders do this by default so only use this when you are using some sort of other warp tile
    script_use_portal,        //4 triggers a playing of an animation followed by a switching of rooms. Only to be used internally for warp gates.
    script_open_door,         //4 if player steps in coords or other action occurs, open a door
    script_animation,         //5 if player steps in coords, do animation
    script_boss_actions,      //6 if boss is still alive, execute boss actions
    script_gain_exp,          //7 gives player some EXP. Don't abuse please
    script_placeholder,       //8 ???
} scriptBehavior;

typedef struct {
    int mapNum;
    int x;
    int y;
    int w;
    int h;
    scriptBehavior action;
    char* data;
    bool active;
} script;

#define drawSprite(spr, flip) drawTile(spr.tileIndex, spr.x, spr.y, spr.w, flip)

void initPlayer(player* player, int x, int y, int size, int mapScreen, int tileIndex);  //inits new player struct
void initConfig(char* filePath);  //resets config data
void initScript(script* scriptPtr, int mapNum, int x, int y, int w, int h, scriptBehavior action, char* data);  //initializes a new script struct
void loadPlayerData(player* player, char* filePath, bool forceNew);  //loads data from filePath. If not, or forceNew = true, inits new sprite.
void loadConfig(char* filePath);  //loads config data into the public variables
void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x);  //loads a tilemap into the specified tilemap matrix and event matrix
int aMenu(SDL_Texture* texture, int cursorID, char* title, char* opt1, char* opt2, char* opt3, char* opt4, char* opt5, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain);  //draws a menu using the colors and options presented
SDL_Keycode getKey();  //like waitForKey but without waiting.
void saveConfig(char* filePath);  //saves config data to the file
char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);  //takes a pointer to a char array and gives you what was on the file's line in the unique location
int readScript(script* scriptPtr, char* input);  //inits a script from a string input
char** getListOfFiles(const size_t maxStrings, const size_t maxLength, const char* directory, int* strNum);  //gets the list of files in a directory

void drawATilemap(SDL_Texture* texture, bool eventLayerFlag, int startX, int startY, int endX, int endY, bool updateScreen);  //draws a tilemap from a given tileset texture.
void drawATile(SDL_Texture* texture, int id, int xCoord, int yCoord, int width, SDL_RendererFlip flip);  //draws a tile from a given tileset texture
void drawTextBox(char* input, SDL_Color outlineColor, SDL_Rect textBoxRect, bool redraw);  //draws a textbox
bool executeScriptAction(script* scriptData, player* player);  //executes a script

int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
SDL_Texture* tilesTexture;

#define SIZE_OF_SCANCODE_ARRAY 6
int CUSTOM_SCANCODES[SIZE_OF_SCANCODE_ARRAY];
#define SC_UP CUSTOM_SCANCODES[0]
#define SC_DOWN CUSTOM_SCANCODES[1]
#define SC_LEFT CUSTOM_SCANCODES[2]
#define SC_RIGHT CUSTOM_SCANCODES[3]
#define SC_INTERACT CUSTOM_SCANCODES[4]
#define SC_MENU CUSTOM_SCANCODES[5]

int FPS, targetTime;

#endif // SDLSEEKERS_H_INCLUDED
