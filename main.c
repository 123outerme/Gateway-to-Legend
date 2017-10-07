//#include "outermeSDL.h"
#include "SDLSeekers.h"

#define checkSKUp keyStates[SC_UP]
#define checkSKDown keyStates[SC_DOWN]
#define checkSKLeft keyStates[SC_LEFT]
#define checkSKRight keyStates[SC_RIGHT]
#define checkSKInteract keyStates[SC_INTERACT]
#define checkSKMenu keyStates[SC_MENU]

#define TILE_ID_PLAYER 16
#define PIXELS_MOVED 6
#define ARRAY_OF_COLLIDED_TILES {8,9,10,11,12,13,14,15}
#define SIZE_OF_COLLISION_ARRAY 8

#define CONFIG_FILEPATH "SDLSeekers.cfg"
#define GLOBALSAVE_FILEPATH "SDLSeekers.txt"

int mainLoop(player* playerSprite);
bool checkCollision(player* player, int moveX, int moveY);
void drawEventmap(int startX, int startY, int endX, int endY, bool updateScreen);

int main(int argc, char* argv[])
{
    //loading map pack stuff
    char* mainFilePath = "map-packs/main.txt";
    char* dummy = "";
    char mapFilePath[100];
    char tileFilePath[100];
    char* saveFilePath[100];
    uniqueReadLine(&mapFilePath, 100, mainFilePath, 1);
    printf("%s\n", mapFilePath);
    uniqueReadLine(&tileFilePath, 100, mainFilePath, 2);
    printf("%s\n", tileFilePath);
    {
        int initCode = initSDL(tileFilePath);
        if (initCode != 0)
            return initCode;
    }
    uniqueReadLine(&saveFilePath, 100, mainFilePath, 3);
    if (checkFile(saveFilePath, 0))
        /*load save file*/;
    else
        createFile(saveFilePath);
    /*load global save file*/
    printf("%s\n", saveFilePath);
    //done loading map-pack specific stuff
    if (checkFile(CONFIG_FILEPATH, 6))  //load config
        loadConfig(CONFIG_FILEPATH);
    else
        initConfig(CONFIG_FILEPATH);
    player person;
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(mainRenderer);
    int gameState = 0;
    bool quitGame = false;
    while(!quitGame)
    {
        int choice = 0;
        switch(gameState)
        {
        case 0:  //main menu
            initPlayer(&person, 9.5 * TILE_SIZE, 7 * TILE_SIZE, TILE_SIZE, TILE_ID_PLAYER);
            choice = aMenu("Main Menu", "Go", "Quit", " ", " ", " " , 2, 1, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, true, false);
            if (choice == 1)
                gameState = 1;
            else
                quitGame = true;
            break;
        case 1:  //main game loop
            loadMapFile(mapFilePath, tilemap, eventmap, 0, HEIGHT_IN_TILES, WIDTH_IN_TILES);
            choice = mainLoop(&person);
            if (choice == ANYWHERE_QUIT)
                quitGame = true;
            if (choice == 1)
                gameState = 2;
            break;
        case 2:  //overworld menu
            choice = aMenu("Overworld Menu", "Back", "Menu", "Quit", " ", " " , 3, 1, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, true, false);
            if (choice == 1)
                gameState = 1;
            if (choice == 2)
                gameState = 0;
            if (choice == 3)
                quitGame = true;
            break;
        }
    }
    printf("Quit successfully\n");
    closeSDL();
}

int mainLoop(player* playerSprite)
{
    SDL_Event e;
    bool quit = false, drawFlag = true;
    int frame = 0, framerate;
    int exitCode = 0;
    char whatever[5] = "    \0";
    time_t startTime = time(NULL);
    time_t lastTime = startTime - 1;
    time_t now = startTime + 1;
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        drawTilemap(0, 0, 20, 15, false);
        //drawEventmap(0, 0, 20, 15, false);
        //drawTile(tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE + 1 * (playerSprite->spr.x % TILE_SIZE > .5 * TILE_SIZE)], (playerSprite->spr.x / TILE_SIZE  + 1 * (playerSprite->spr.x % TILE_SIZE > .5 * TILE_SIZE)) * TILE_SIZE, (playerSprite->spr.y / TILE_SIZE) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
        while(SDL_PollEvent(&e) != 0)  //while there are events in the queue
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                exitCode = ANYWHERE_QUIT;
            }
        }
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        if (!playerSprite->movementLocked && (checkSKUp || checkSKDown || checkSKLeft || checkSKRight) && frame % 28 == 0)
        {
            int lastY = playerSprite->spr.y;
            int lastX = playerSprite->spr.x;
                //SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}));
                //drawTile(tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE], (playerSprite->spr.x / TILE_SIZE) * TILE_SIZE, (playerSprite->spr.y / TILE_SIZE) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
                if (playerSprite->spr.y > 0 && checkSKUp)
                    playerSprite->spr.y -= PIXELS_MOVED;
                if (playerSprite->spr.y < SCREEN_HEIGHT - playerSprite->spr.h && checkSKDown)
                    playerSprite->spr.y += PIXELS_MOVED;
                if (playerSprite->spr.x > 0 && checkSKLeft)
                    playerSprite->spr.x -= PIXELS_MOVED;
                if (playerSprite->spr.x < SCREEN_WIDTH - playerSprite->spr.w && checkSKRight)
                    playerSprite->spr.x += PIXELS_MOVED;
                if (checkSKLeft)
                    playerSprite->flip = SDL_FLIP_HORIZONTAL;
                if (checkSKRight)
                    playerSprite->flip = SDL_FLIP_NONE;
                /*if (checkCollision(playerSprite, tilemap[playerSprite->spr.y / TILE_SIZE + checkSKDown][playerSprite->spr.x / TILE_SIZE + checkSKRight], 0, 0))
                {
                    if (SDL_HasIntersection(&((SDL_Rect) {.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}), &((SDL_Rect) {.x = (playerSprite->spr.x / TILE_SIZE) * TILE_SIZE + checkSKRight, .y = (playerSprite->spr.y / TILE_SIZE) * TILE_SIZE + checkSKDown, .w = TILE_SIZE, .h = TILE_SIZE})))
                    {
                        playerSprite->spr.y = lastY;
                        playerSprite->spr.x = lastX;
                    }
                }*/
                exitCode = checkCollision(playerSprite, checkSKRight + -1 * checkSKLeft, checkSKDown + -1 * checkSKUp);
                if(exitCode)
                {
                    playerSprite->spr.y = lastY;
                    playerSprite->spr.x = lastX;
                    //printf("%d\n", exitCode);
                }
        }
        if (checkSKMenu)
        {
            quit = true;
            exitCode = 1;
        }
        frame++;
        if (time(NULL) > startTime)
            now = time(NULL);
        if (time(NULL) - 1 > lastTime)
            lastTime = time(NULL);
        if (lastTime == now)
        {
            if (drawFlag)
                framerate = frame / ((int) now - (int) startTime);
            drawFlag = false;
        }
        else
            drawFlag = true;
        drawText(intToString(framerate, whatever), 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF}, false);
        //printf("Framerate: %d\n", frame / ((int) now - (int) startTime));

        drawSprite(playerSprite->spr, playerSprite->flip);
        SDL_RenderPresent(mainRenderer);
    }
    return exitCode;
}

bool checkCollision(player* player, int moveX, int moveY)
{
    if (moveX || moveY)
    {
        int collideID = 0;
        int thisX = player->spr.x;
        int thisY = player->spr.y;
        if (1 == eventmap[thisY / TILE_SIZE][thisX / TILE_SIZE])
            collideID += 1;
        if (1 == eventmap[thisY / TILE_SIZE][thisX / TILE_SIZE + (thisX % TILE_SIZE != 0)])
            collideID += 2;
        if (1 == eventmap[thisY / TILE_SIZE + (thisY % TILE_SIZE != 0)][thisX / TILE_SIZE])
            collideID += 4;
        if (1 == eventmap[thisY / TILE_SIZE + (thisY % TILE_SIZE != 0)][thisX / TILE_SIZE + (thisX % TILE_SIZE != 0)])
            collideID += 8;
        if ((collideID == 1 && moveX < 0 && moveY > 0) || ((collideID == 2 || collideID == 10) && moveX > 0 && moveY > 0) || (collideID == 4 && moveX < 0 && moveY < 0) || ((collideID == 8) && moveX > 0 && moveY < 0))
        {  //manually adding y sliding
            collideID = 0;
            player->spr.x -= moveX * PIXELS_MOVED;
        }
        if (((collideID == 1 || collideID == 3) && moveX > 0 && moveY < 0) || ((collideID == 2 || collideID == 3) && moveX < 0 && moveY < 0) || ((collideID == 4 || collideID == 12) && moveX > 0 && moveY > 0) || ((collideID == 8 || collideID == 12) && moveX < 0 && moveY > 0))
        {  //manually adding x sliding
            collideID = 0;
            player->spr.y -= moveY * PIXELS_MOVED;
        }
        /*drawTile(7, (thisX / TILE_SIZE) * TILE_SIZE, (thisY / TILE_SIZE) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
        drawTile(15, (thisX / TILE_SIZE + (thisX % TILE_SIZE != 0)) * TILE_SIZE, (thisY / TILE_SIZE) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
        drawTile(2, (thisX / TILE_SIZE) * TILE_SIZE, (thisY / TILE_SIZE + (thisY % TILE_SIZE != 0)) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
        drawTile(9, (thisX / TILE_SIZE + (thisX % TILE_SIZE != 0)) * TILE_SIZE, (thisY / TILE_SIZE + (thisY % TILE_SIZE != 0)) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
        SDL_Delay(30);*/
        return collideID;
    }
    return false;
}

void drawEventmap(int startX, int startY, int endX, int endY, bool updateScreen)
{
    for(int dy = startY; dy < endY; dy++)
        for(int dx = startX; dx < endX; dx++)
            drawTile(127 - eventmap[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
    if (updateScreen)
        SDL_RenderPresent(mainRenderer);
}
