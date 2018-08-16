#ifndef SDLGATEWAY_H_INCLUDED
#define SDLGATEWAY_H_INCLUDED

#include "outermeSDL.h"  //uses outermeSDL v1.4 as of right now. Future versions of the header may not be compatible
#include <dirent.h>

#define VERSION_NUMBER "0.12.0b"

#define SAVE_FILE_LINES 0
#define GAME_WINDOW_NAME "Gateway to Legend"
#define CONFIG_FILEPATH "assets/GatewayToLegend.cfg"
#define GLOBALTILES_FILEPATH "tileset/mainTileset48.png"
#define GLOBALSAVE_FILEPATH "saves/GATEWAY_MAIN.txt"
#define MAP_PACKS_SUBFOLDER "map-packs/"
#define MAX_LIST_OF_MAPS 30

#define MAX_SPRITE_MAPPINGS 21  //sprite defines
#define MAX_MAP_PACK_DATA 6 //does not include sprite defines

#define AMENU_MAIN_TEXTCOLOR  0x00, 0xB0, 0xDA, 0xFF
#define AMENU_MAIN_BGCOLOR 0xE4, 0xE9, 0xF3, 0xFF
#define AMENU_MAIN_TITLECOLOR1 0x4D, 0xD2, 0xFF, 0xFF
#define AMENU_MAIN_TITLECOLOR2 0x00, 0xAC, 0xE6, 0xFF
#define AMENU_MAIN_THEME (SDL_Color) {AMENU_MAIN_BGCOLOR}, (SDL_Color) {AMENU_MAIN_TITLECOLOR2}, (SDL_Color) {AMENU_MAIN_TITLECOLOR1},  (SDL_Color) {AMENU_MAIN_TEXTCOLOR}
#define AMENU_GAMEOVER_THEME (SDL_Color) {0x33, 0x33, 0x33, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0xC6, 0xC6, 0xC6, 0xFF}, (SDL_Color) {0xC6, 0xC6, 0xC6, 0xFF}

#define TILE_ID_CURSOR 17
#define TILE_ID_TILDA 125
#define TILE_ID_CUBED 124

#define calcWaitTime(x) x == 0 ? 0 : 1000 / x

typedef struct _mapPack {
    SDL_Texture* mapPackTexture;
    char mainFilePath[MAX_FILE_PATH];
    char name[MAX_FILE_PATH];
    char mapFilePath[MAX_FILE_PATH];
    char tilesetFilePath[MAX_FILE_PATH];
    char saveFilePath[MAX_FILE_PATH];
    char scriptFilePath[MAX_FILE_PATH];
    int initX;
    int initY;
    int initMap;
    int tilesetMaps[MAX_SPRITE_MAPPINGS];
    int numBosses;
} mapPack;

#define MAX_PLAYER_NAME 8
#define MAX_PLAYER_HEALTH 32
#define DEFAULT_PLAYER_HEALTH 12
#define MAX_PLAYER_TECHNIQUES 5

#define MAX_ENEMIES 6

typedef struct _player {
    sprite spr;  //?
    char name[MAX_PLAYER_NAME + 1];  //9 bytes
    int level;  //
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
    bool techUnlocks[MAX_PLAYER_TECHNIQUES];  // 5 bytes
    bool movementLocked;  // 1 byte
    int defeatedBosses[10]; //
	int* disabledScripts;  //
    int nextBossPos;  //
    char* extraData;  //
} player;

typedef struct _enemy {
    sprite spr;
    int invincTimer;
    int HP;
} enemy;

typedef enum  _scriptBehavior {
    script_none,                   //0 default, do nothing
    script_trigger_dialogue,       //1 if player steps in coords & presses interact, trigger a dialogue/text box
    script_trigger_dialogue_once,  //2 same as above, but just once.
    script_force_dialogue,         //3 force a dialog popup
    script_trigger_boss,           //4 if player steps in coords, spawn boss
    script_switch_maps,            //5 triggers a switching of rooms. Map borders do this by default so only use this when you are using some sort of other warp tile. Like a silent use_teleporter
    script_use_gateway,            //6 triggers a playing of an animation followed by a switching of rooms. Only to be used internally for warp gates.
    script_use_teleporter,         //7 teleports to a specified matching teleporter
    script_toggle_door,            //8 if player steps in coords or other action occurs, open a door
    script_animation,              //9 if player steps in coords, do animation
    script_boss_actions,           //10 if boss is still alive, execute boss actions
    script_gain_money,             //11 gives player some money. Please don't abuse also
    script_player_hurt,            //12 hurts the player by <data> amount
    script_placeholder,            //13 ?
} scriptBehavior;

#define ALL_ACTION_DESCRIPTIONS {"Nothing", "Triggers a textbox after Interact is pressed", "Triggers a textbox after Interact is pressed, but only once", "Forces a textbox open", "Spawns the boss", "Switches maps with no flair", "Transports the player to a different map with animations", "Teleports a player to a location on the same map", "Opens some doors", "Moves a character around the screen then opens a textbox", "Defines boss actions", "Gives player some money", "Hurts the player an amount (negative for heal)", "TBD"}

typedef struct _script {
    int mapNum;
    int x;
    int y;
    int w;
    int h;
    scriptBehavior action;
    char data[99];
    bool active;
    bool disabled;
    int lineNum;
} script;

typedef struct _node {
    int x;
    int y;
    void* lastNode;
    bool visited;
    int distance;
} node;

typedef struct _spark {
    SDL_Rect sparkRects[99];
    SDL_Rect boundsRect;
    SDL_Color color;
    int num;
    int maxW;
    int maxH;
    int timer;
    int maxTimer;
    int update;
} spark;

#define drawSprite(spr, flip) drawTile(spr.tileIndex, spr.x, spr.y, spr.w, flip)

int initSounds();  //inits all sounds used. Returns -5 if one can't load
void loadMapPackData(mapPack* loadPack, char* location);  //loads map pack data
void initPlayer(player* player, int x, int y, int w, int h, int mapScreen, int angle, SDL_RendererFlip flip, int tileIndex);  //inits new player struct
void createLocalPlayer(player* playerSprite, char* filePath, int x, int y, int w, int h, int mapScreen, int angle, SDL_RendererFlip flip, int tileIndex, int numScripts);  //creates new local data for player
void createGlobalPlayer(player* playerSprite, char* filePath);  //creates new global data for player
void initEnemy(enemy* enemyPtr, int x, int y, int w, int h, int tileIndex, int HP, entityType type);  //inits an enemy
void initConfig(char* filePath);  //resets config data
void initScript(script* scriptPtr, scriptBehavior action, int mapNum, int x, int y, int w, int h, char* data, int lineNum);  //initializes a new script struct
void initNode(node* nodePtr, int x, int y, node* lastNode, bool visited, int distance);  //initializes a new node
void initSpark(spark* sparkPtr, SDL_Rect boundsRect, SDL_Color color, int num, int maxW, int maxH, int maxTimer, int update);  //initializes a new spark
void loadConfig(char* filePath);  //loads config data into the public variables
void loadLocalPlayer(player* playerSprite, char* filePath, int tileIndex);  //loads from local player save file
void loadGlobalPlayer(player* playerSprite, char* filePath);  //loads from global player save file
void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x);  //loads a tilemap into the specified tilemap matrix and event matrix
void drawAMap(SDL_Texture* tileTexture, int thisTilemap[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, bool hideCollision, bool isEvent, bool updateScreen);
void drawOverTilemap(SDL_Texture* texture, int anEventmap[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, int xOffset, int yOffset, bool drawDoors[], bool drawEnemies, bool rerender);
int aMenu(SDL_Texture* texture, int cursorID, char* title, char** optionsArray, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain, void (*extraDrawing)(void));  //draws a menu using the colors and options presented
void stringInput(char** data, char* prompt, int maxChar, char* defaultStr, bool startCaps);   //gets string input
int intInput(char* prompt, int maxDigits, int defaultVal, int minVal, int maxVal, bool allowNeg);  //gets int input
void saveMapPack(mapPack* writePack);  //saves map pack data to the file
void saveConfig(char* filePath);  //saves config data to the file
void saveLocalPlayer(const player playerSprite, char* filePath);  //saves to local player save file
void saveGlobalPlayer(const player playerSprite, char* filePath);  //saves to global player save file
void getNewKey(char* titleText, SDL_Color bgColor, SDL_Color textColor, int selection);  //changes a key to another key if it can
char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);  //takes a pointer to a char array and gives you what was on the file's line in the unique location
node* BreadthFirst(const int startX, const int startY, const int endX, const int endY, int* lengthOfPath, const bool drawDebug);  //finds the path between (startX, startY) and (endX, endY) using the data in eventmap
int readScript(script* scriptPtr, char* input, int lineNum);  //inits a script from a string input
char** getListOfFiles(int maxStrings, int maxLength, const char* directory, int* strNum);  //gets the list of files in a directory

void drawTextBox(char* input, SDL_Color outlineColor, SDL_Rect textBoxRect, bool redraw);  //draws a textbox
bool executeScriptAction(script* scriptData, player* player);  //executes a script

void SDLCALL playMainMusic();  //play main theme callback
void SDLCALL playOverworldMusic();  //play an overworld theme callback

int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
SDL_Texture* tilesTexture;
int tileIDArray[MAX_SPRITE_MAPPINGS];

bool enemyFlags[MAX_ENEMIES + 1];  //last bool is reloadEnemies
enemy enemies[MAX_ENEMIES];

#define SIZE_OF_SCANCODE_ARRAY 7
SDL_Scancode CUSTOM_SCANCODES[SIZE_OF_SCANCODE_ARRAY];
#define SC_UP CUSTOM_SCANCODES[0]
#define SC_DOWN CUSTOM_SCANCODES[1]
#define SC_LEFT CUSTOM_SCANCODES[2]
#define SC_RIGHT CUSTOM_SCANCODES[3]
#define SC_INTERACT CUSTOM_SCANCODES[4]
#define SC_MENU CUSTOM_SCANCODES[5]
#define SC_SPECIAL CUSTOM_SCANCODES[6]

#define MUSIC(x) musicArray[x > MAX_MUSIC ? 0 : x - 1]
#define PAUSE_SOUND audioArray[0]
#define UNSHEATH_SOUND audioArray[1]
#define OPTION_SOUND audioArray[2]
#define PING_SOUND audioArray[3]
#define STEP_SOUND(x) audioArray[x < 1 || x > 4 ? 4 : x + 3]
#define SWING_SOUND audioArray[7]
#define GATEWAYSTART_SOUND audioArray[8]
#define GATEWAYEND_SOUND audioArray[9]
#define DOOROPEN_SOUND audioArray[10]
#define CASH_SOUND audioArray[11]
#define PLAYERHURT_SOUND audioArray[12]
#define ENEMYHURT_SOUND audioArray[13]
#define TELEPORT_SOUND audioArray[14]
#define HEAL_SOUND audioArray[15]
#define DASH_SOUND(x) audioArray[x < 1 || x > 4 ? 16 : x + 15]

#define PAUSE_FILE "assets/audio/pause.ogg"
#define UNSHEATH_FILE "assets/audio/unsheath.ogg"
#define OPTION_FILE "assets/audio/unsheath_bass.ogg"
#define PING_FILE "assets/audio/menu.ogg"
#define SWING_FILE "assets/audio/swing.ogg"
#define STEP_FILE "assets/audio/step"  //this is like this on purpose, for some strcat() stuff
#define GATEWAYSTART_FILE "assets/audio/gateway1.ogg"
#define GATEWAYEND_FILE "assets/audio/gateway2.ogg"
#define DOOROPEN_FILE "assets/audio/doorOpen.ogg"
#define CASH_FILE "assets/audio/coin.ogg"
#define PLAYERHURT_FILE "assets/audio/playerHurt.ogg"
#define ENEMYHURT_FILE "assets/audio/enemyHurt.ogg"
#define TELEPORT_FILE "assets/audio/teleport.ogg"
#define HEAL_FILE "assets/audio/heal.ogg"
#define DASH_FILE "assets/audio/dash"  //same as above

#define PAUSE_CHANNEL channelArray[0]
#define UNSHEATH_CHANNEL channelArray[1]
#define OPTION_CHANNEL channelArray[2]
#define PING_CHANNEL channelArray[3]
#define STEP_CHANNEL(x) channelArray[x < 1 || x > 3 ? 4 : x + 3]
#define SWING_CHANNEL channelArray[7]
#define GATEWAY_CHANNEL channelArray[8]
#define DOOROPEN_CHANNEL channelArray[9]
#define CASH_CHANNEL channelArray[10]
#define PLAYERHURT_CHANNEL channelArray[11]
#define ENEMYHURT_CHANNEL channelArray[12]
#define TELEPORT_CHANNEL channelArray[13]
#define HEAL_CHANNEL channelArray[14]
#define DASH_CHANNEL channelArray[15]

#define MAIN_MUSIC_FILE "assets/audio/mainTheme.mp3"
#define OVERWORLD1_MUSIC_FILE "assets/audio/GtL3.mp3"
#define OVERWORLD2_MUSIC_FILE "assets/audio/GtL5.mp3"
#define OVERWORLD3_MUSIC_FILE "assets/audio/.mp3"
#define BOSS_MUSIC_FILE "assets/audio/gatewayIntoAction.mp3"
#define FANFARE_MUSIC_FILE "assets/audio/GtL4.mp3"
#define GAMEOVER_MUSIC_FILE "assets/audio/gatewayToRetry.mp3"

#define SPARK_COLOR_RED ((SDL_Color) {0xD8, 0x22, 0x0A, 0xD0})
#define SPARK_COLOR_BRIGHT_RED ((SDL_Color) {0xFF, 0x3F, 0x3F, 0xD0})
#define SPARK_COLOR_ORANGE ((SDL_Color) {0xFF, 0x8C, 0x11, 0xD0})
#define SPARK_COLOR_BLUE ((SDL_Color) {0x44, 0x8C, 0xFF, 0xD0})
#define SPARK_COLOR_GREEN ((SDL_Color) {0x00, 0xBA, 0x34, 0xD0})
#define SPARK_COLOR_GRAY ((SDL_Color) {0x60, 0x65, 0x70, 0xD0})
#define SPARK_COLOR_SILVER ((SDL_Color) {0xD4, 0xD8, 0xDD, 0xD0})
#define SPARK_GATEWAY ((SDL_Color) {0x48, 0x00, 0x96, 0xA0})

#define SPARK_BOSS ((SDL_Color) {0, 0, 0, 0})

int FPS, targetTime, startTime, frame;
bool doorFlags[4];
bool noclip;

int _globalInt1, _globalInt2, _globalInt3;  //for general use purposes

char mainFilePath[MAX_FILE_PATH], mapFilePath[MAX_FILE_PATH - 9], tileFilePath[MAX_FILE_PATH - 9],
saveFilePath[MAX_FILE_PATH - 9], scriptFilePath[MAX_FILE_PATH - 9];

int maxBosses;
int maxScripts;
bool bossLoaded;
int musicIndex;
sprite animationSpr;

#define MAX_SPARKS 8
bool sparkFlag, theseSparkFlags[MAX_SPARKS];
spark theseSparks[MAX_SPARKS];

/*
Spark IDs:
0 - Ability
1 - Hurt / Heal
2 - Hit
3 - Money
4 - Gateway
5 - Teleporter
6 - Command
7 - Boss Defeat
*/

#endif // SDLSEEKERS_H_INCLUDED
