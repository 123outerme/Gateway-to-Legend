#ifndef SDLSEEKERS_H_INCLUDED
#define SDLSEEKERS_H_INCLUDED

#include "outermeSDL.h"  //uses outermeSDL v1.4 as of right now. Future versions of the header may not be compatible
#include <dirent.h>

#define VERSION_NUMBER "1.0.0a"

#define SAVE_FILE_LINES 0
#define PLAYER_NAME_LIMIT 8

#define WINDOW_NAME "Gateway to Legend"
#define CONFIG_FILEPATH "GatewayToLegend.cfg"
#define GLOBALTILES_FILEPATH "tileset/mainTileset48.png"
#define GLOBALSAVE_FILEPATH "saves/GATEWAY_MAIN.txt"
#define MAP_PACKS_SUBFOLDER "map-packs/"
#define MAX_LIST_OF_MAPS 30
#define MAX_CHAR_IN_FILEPATH MAX_PATH

#define AMENU_MAIN_TEXTCOLOR  0x00, 0xB0, 0xDA
#define AMENU_MAIN_BGCOLOR 0xE4, 0xE9, 0xF3
#define AMENU_MAIN_TITLECOLOR1 0x4D, 0xD2, 0xFF
#define AMENU_MAIN_TITLECOLOR2 0x00, 0xAC, 0xE6

#define calcWaitTime(x) x == 0 ? 0 : 1000 / x

typedef struct {
    sprite spr;  //?
    char name[PLAYER_NAME_LIMIT + 1];  //9 bytes
    int level;  //
    int experience;  //
    int money;  //
    int HP;  //
    int maxHP;  //
    int mapScreen;  //8 bytes
    int xVeloc;  //
    int yVeloc;  //
    int lastDirection;  //
    int invincCounter; //
    int animationCounter;
    int lastMap;
    int lastX;
    int lastY;
    bool movementLocked;  // 1 byte
    char* extraData;
} player;

typedef struct {
    sprite spr;
    int invincTimer;
    int HP;
} enemy;

typedef enum {
    script_none,                   //0 default, do nothing
    script_trigger_dialogue,       //1 if player steps in coords & presses interact, trigger a dialogue/text box
    script_trigger_dialogue_once,  //2 same as above, but just once.
    script_trigger_boss,           //3 if player steps in coords, spawn boss
    script_switch_maps,            //4 triggers a switching of rooms. Map borders do this by default so only use this when you are using some sort of other warp tile. Like a silent use_teleporter
    script_use_gateway,            //5 triggers a playing of an animation followed by a switching of rooms. Only to be used internally for warp gates.
    script_use_teleporter,         //6 teleports to a specified matching teleporter
    script_toggle_door,            //7 if player steps in coords or other action occurs, open a door
    script_animation,              //8 if player steps in coords, do animation
    script_boss_actions,           //9 if boss is still alive, execute boss actions
    script_gain_exp,               //19 gives player some EXP. Don't abuse please
    script_gain_money,             //11 gives player some money. Please don't abuse also
    script_player_hurt,            //12 hurts the player by <data> amount
    script_placeholder,            //13 ?
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

typedef struct {
    int x;
    int y;
    void* lastNode;
    bool visited;
    int distance;
} node;

#define drawSprite(spr, flip) drawTile(spr.tileIndex, spr.x, spr.y, spr.w, flip)

int initSounds();  //inits all sounds used. Returns -5 if one can't load
void initPlayer(player* player, int x, int y, int w, int h, int mapScreen, int angle, SDL_RendererFlip flip, int tileIndex);  //inits new player struct
void createLocalPlayer(player* playerSprite, char* filePath, int x, int y, int w, int h, int mapScreen, int angle, SDL_RendererFlip flip, int tileIndex);  //creates new local data for player
void createGlobalPlayer(player* playerSprite, char* filePath);  //creates new global data for player
void initEnemy(enemy* enemyPtr, int x, int y, int w, int h, int tileIndex, int HP, entityType type);  //inits an enemy
void initConfig(char* filePath);  //resets config data
void initScript(script* scriptPtr, scriptBehavior action, int mapNum, int x, int y, int w, int h, char* data);  //initializes a new script struct
void initNode(node* nodePtr, int x, int y, node* lastNode, bool visited, int distance);  //initializes a new node
void loadConfig(char* filePath);  //loads config data into the public variables
void loadLocalPlayer(player* playerSprite, char* filePath, int tileIndex);  //loads from local player save file
void loadGlobalPlayer(player* playerSprite, char* filePath);  //loads from global player save file
void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x);  //loads a tilemap into the specified tilemap matrix and event matrix
int aMenu(SDL_Texture* texture, int cursorID, char* title, char** optionsArray, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain);  //draws a menu using the colors and options presented
SDL_Keycode getKey();  //like waitForKey but without waiting.
void saveConfig(char* filePath);  //saves config data to the file
void saveLocalPlayer(const player playerSprite, char* filePath);  //saves to local player save file
void saveGlobalPlayer(const player playerSprite, char* filePath);  //saves to global player save file
void getNewKey(char* titleText, SDL_Color bgColor, SDL_Color textColor, int selection);  //changes a key to another key if it can
char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);  //takes a pointer to a char array and gives you what was on the file's line in the unique location
node* BreadthFirst(const int startX, const int startY, const int endX, const int endY, int* lengthOfPath, const bool drawDebug);  //finds the path between (startX, startY) and (endX, endY) using the data in eventmap
int readScript(script* scriptPtr, char* input);  //inits a script from a string input
char** getListOfFiles(int maxStrings, int maxLength, const char* directory, int* strNum);  //gets the list of files in a directory

void drawTextBox(char* input, SDL_Color outlineColor, SDL_Rect textBoxRect, bool redraw);  //draws a textbox
bool executeScriptAction(script* scriptData, player* player);  //executes a script

int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
SDL_Texture* tilesTexture;

#define SIZE_OF_SCANCODE_ARRAY 7
int CUSTOM_SCANCODES[SIZE_OF_SCANCODE_ARRAY];
#define SC_UP CUSTOM_SCANCODES[0]
#define SC_DOWN CUSTOM_SCANCODES[1]
#define SC_LEFT CUSTOM_SCANCODES[2]
#define SC_RIGHT CUSTOM_SCANCODES[3]
#define SC_INTERACT CUSTOM_SCANCODES[4]
#define SC_MENU CUSTOM_SCANCODES[5]
#define SC_ATTACK CUSTOM_SCANCODES[6]

#define MUSIC(x) audioArray[x > 2 ? 1 : x - 1]
#define UNSHEATH_SOUND audioArray[2]
#define OPTION_SOUND audioArray[3]
#define PING_SOUND audioArray[4]
#define STEP_SOUND(x) audioArray[x < 1 || x > 4 ? 6 : x + 4]
#define SWING_SOUND audioArray[8]
#define GATEWAYSTART_SOUND audioArray[9]
#define GATEWAYEND_SOUND audioArray[10]
#define DOOROPEN_SOUND audioArray[11]
#define CASH_SOUND audioArray[12]
#define PLAYERHURT_SOUND audioArray[13]
#define ENEMYHURT_SOUND audioArray[14]

#define UNSHEATH_FILE "audio/unsheath.ogg"
#define OPTION_FILE "audio/unsheath_bass.ogg"
#define PING_FILE "audio/menu.ogg"
#define SWING_FILE "audio/swing.ogg"
#define STEP_FILE "audio/step"  //this is like this on purpose, for some strcat() stuff
#define GATEWAYSTART_FILE "audio/gateway1.ogg"
#define GATEWAYEND_FILE "audio/gateway2.ogg"
#define DOOROPEN_FILE "audio/doorOpen.ogg"
#define CASH_FILE "audio/coin.ogg"
#define PLAYERHURT_FILE "audio/playerHurt.ogg"
#define ENEMYHURT_FILE "audio/enemyHurt.ogg"

#define MUSIC_CHANNEL(x) channelArray[x > 2 ? 1 : x - 1]
#define UNSHEATH_CHANNEL channelArray[2]
#define OPTION_CHANNEL channelArray[3]
#define PING_CHANNEL channelArray[4]
#define STEP_CHANNEL(x) channelArray[x < 1 || x > 3 ? 6 : x + 4]
#define SWING_CHANNEL channelArray[8]
#define GATEWAY_CHANNEL channelArray[9]
#define DOOROPEN_CHANNEL channelArray[10]
#define CASH_CHANNEL channelArray[11]
#define PLAYERHURT_CHANNEL channelArray[12]
#define ENEMYHURT_CHANNEL channelArray[13]



/*
Sound effects should play upon walking
killing enemies [x]
swinging the sword [x]
pausing [ ]
moving menu cursors [x]
selecting options [x]
going through the portals [x]
walking [x]
opening doors [x]
teleporting [x]
getting hurt [x]
etc.
*/

int FPS, targetTime;
bool doorFlags[3];

#endif // SDLSEEKERS_H_INCLUDED
