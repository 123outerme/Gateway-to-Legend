#ifndef OUTERMESDL_H_INCLUDED
#define OUTERMESDL_H_INCLUDED

/* ++ outermeSDL version 1.4 - last update 1/1/2018 ++

 error code -5: Sounds failed to load
 error code -4: TTFs failed to load
 error code -3: SDL_mixer failed to load
 error code -2: SDL_ttf failed to initialize
 error code -1: SDL_img failed to initialize
  error code 0: No error
  error code 1: SDL system failed to initialize
  error code 2: Window could not be created
  error code 3: Renderer failed to initialize
  error code 4: TTF font failed to load
  error code 5: tilemap failed to load
  error code 6: Extraneous media could not load/optimize
*/

#define SDL_MAIN_HANDLED 1
#include "SDL2/SDL.h"       //This is included because it's an SDL2 program... duh
#include "SDL2/SDL_image.h" //This is included so we can use PNGs.
#include "SDL2/SDL_ttf.h"   //This is included for text stuff
#include "SDL2/SDL_mixer.h" //This is included for audio
#include <stdio.h>         //This is included because it's fundamental always. Even if it ain't needed
#include <string.h>        //This is included for strncat and other string functions
#include <stdlib.h>        //This is included for calloc, rand(), and a lot of other stuff
#include <math.h>          //This is included for log10
#include <time.h>          //This is included for time() as the seed for rand()
#include <ctype.h>         //This is included for toupper, etc.

#define bool char
#define false 0
#define true 1

#define printBool(bool) (bool ? "true" : "false")
#define drawASprite(tileset, spr) drawATile(tileset, spr.tileIndex, spr.x, spr.y, spr.w, spr.h, spr.angle, spr.flip)

//#define SAVE_FILE_NAME "SAVUVUTU.bin"
//#define SAVE_FILE_LINES 54
//#define CONFIG_FILE_NAME "sorceryConfig.ini"
#define FONT_FILE_NAME "assets/Px437_ITT_BIOS_X.ttf"
#define FRAMERATE 60
#define SCREEN_WIDTH TILE_SIZE * 20
#define SCREEN_HEIGHT TILE_SIZE * 15
#define TILE_SIZE 48
#define WIDTH_IN_TILES SCREEN_WIDTH / TILE_SIZE
#define HEIGHT_IN_TILES SCREEN_HEIGHT / TILE_SIZE

#define ANYWHERE_QUIT -1

#define MAX_SOUNDS 19
#define MAX_MUSIC 7
#define MAX_FILE_PATH 260

typedef enum
{
    type_na,  //0
    type_generic, //1
    type_chest,  //2
    type_npc,  //3
    type_boss,  //4
    type_player,  //5
    type_enemy  //6
} entityType;

typedef struct
{
    int x;  //
    int y;  //
    int w;  //
    int h;  //
    int tileIndex;  //
    int angle;  //
    SDL_RendererFlip flip;  //
    SDL_Rect* clipRect;  // 16? bytes (4 ints)
    entityType type;
} sprite;

int initSDL(char* windowName, char* tilesetFilePath, char* fontFilePath, char* iconPath, int windowWidth, int windowHeight, int fontSize);  //inits SDL and related stuff
bool loadIMG(char* imgPath, SDL_Texture** dest);  //loads an image from a file into a texture
bool loadTTFont(char* filePath, TTF_Font** dest, int sizeInPts);  //loads a .ttf file into an SDL font
int* loadTextTexture(char* text, SDL_Texture** dest, int maxW, SDL_Color color, int isBlended);  //loads a texture from inputted text
void initSprite(sprite* spr, int x, int y, int w, int h, int tileIndex, int angle, SDL_RendererFlip flip, entityType type);  //initializes a new sprite
void drawATilemap(SDL_Texture* texture, int map[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, int xOffset, int yOffset, int hideTileNumOf, bool updateScreen);  //draws a tilemap from a given tileset texture.
void drawATile(SDL_Texture* texture, int id, int xCoord, int yCoord, int width, int height, int angle, SDL_RendererFlip flip);  //draws a tile from a given tileset texture
void drawText(char* input, int x, int y, int maxW, int maxH, SDL_Color color, bool render);  //draws text to the screen
SDL_Keycode getKey(bool useMouse);  //gets a keycode that was pressed, or none if none was pressed during collection
SDL_Keycode waitForKey(bool useMouse);  //waits for a player to press any key, returns the key that was pressed
void closeSDL();  //closes SDL and related stuff
int intSeqSearch(int value, int array[], size_t arraySize);  //finds an int in an int array
char* removeChar(char input[], char removing, size_t length, bool foreToBack);  //removes the the first specified character from a string
int createFile(char* filePath);  //creates a file if it doesn't exist; if it does, clears it out
int checkFile(char* filePath, int desiredLines);  //checks if a file exists with a certain number of lines. 0 for any.
int appendLine(char* filePath, char* stuff);  //appends a line to a file
char* readLine(char* filePath, int lineNum, char** output);  //reads a certain line from a file

char* intToString(int value, char * result);  //turns inputted int into a string
int digits(int num);  //gets the number of digits an int has
int toPowerOf10(int power);  //gets 10 ^ input
void* freeThisMem(void* x);  //frees memory of any type/object/whatever and nulls its pointer.

extern SDL_Window* mainWindow;
extern SDL_Texture* tilesetTexture;
extern SDL_Renderer* mainRenderer;
extern TTF_Font* mainFont;
extern char customError[256];
//extern char mainFilePath[MAX_FILE_PATH], mapFilePath[MAX_FILE_PATH - 9], tileFilePath[MAX_FILE_PATH - 9],
//saveFilePath[MAX_FILE_PATH - 9], scriptFilePath[MAX_FILE_PATH - 9];
extern int tilemap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
extern Mix_Chunk* audioArray[MAX_SOUNDS];
extern Mix_Music* musicArray[MAX_MUSIC];
extern int channelArray[MAX_SOUNDS - 1];
extern int soundVolume, musicVolume;
extern bool canDrawTiles;
extern bool canDrawText;

#endif // OUTERMESDL_H_INCLUDED
