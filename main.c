//#include "outermeSDL.h"
#include "SDLSeekers.h"

#define checkSKUp keyStates[SC_UP]
#define checkSKDown keyStates[SC_DOWN]
#define checkSKLeft keyStates[SC_LEFT]
#define checkSKRight keyStates[SC_RIGHT]
#define checkSKInteract keyStates[SC_INTERACT]
#define checkSKMenu keyStates[SC_MENU]

#define TILE_ID_PLAYER 16

int main(int argc, char* argv[])
{
    SC_UP = SDL_SCANCODE_W;
    SC_DOWN = SDL_SCANCODE_S;
    SC_LEFT = SDL_SCANCODE_A;
    SC_RIGHT = SDL_SCANCODE_D;
    SC_MENU = SDL_SCANCODE_ESCAPE;
    char* filePath = "";
    filePath = readLine("maps/map.bin", 0, &filePath);
    filePath = removeChar(filePath, '\n', 1024, false);
    printf("%s\n", filePath);
    loadMapFile("maps/map.bin", tilemap, 1, WIDTH_IN_TILES, HEIGHT_IN_TILES);
    initSDL(filePath);
    player person;
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(mainRenderer);
    initPlayer(&person, 0, 0, TILE_SIZE, TILE_ID_PLAYER);
    drawSprite(person.spr, person.flip);
    SDL_RenderPresent(mainRenderer);
    SDL_Event e;
    bool quit = false;
    int frame = 0;
    player* playerSprite = &person;
    time_t startTime = time(NULL);
    time_t lastTime = time(NULL) - 1;
    time_t now = time(NULL) + 1;
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)  //while there are events in the queue
        {
            SDL_RenderClear(mainRenderer);
            if (e.type == SDL_QUIT)
                quit = true;
        }
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        if (!playerSprite->movementLocked && (checkSKUp || checkSKDown || checkSKLeft || checkSKRight) && frame % 30 == 0)
        {
                SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}));
                //drawTile(tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE], playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, SDL_FLIP_NONE);
                if (playerSprite->spr.y > 0 && checkSKUp)
                    playerSprite->spr.y -= 6;
                if (playerSprite->spr.y < SCREEN_HEIGHT - playerSprite->spr.h && checkSKDown)
                    playerSprite->spr.y += 6;
                if (playerSprite->spr.x > 0 && checkSKLeft)
                    playerSprite->spr.x -= 6;
                if (playerSprite->spr.x < SCREEN_WIDTH - playerSprite->spr.w && checkSKRight)
                    playerSprite->spr.x += 6;
                if (checkSKLeft)
                    playerSprite->flip = SDL_FLIP_HORIZONTAL;
                if (checkSKRight)
                    playerSprite->flip = SDL_FLIP_NONE;
        }
        if (checkSKMenu)
                quit = true;
        drawSprite(person.spr, person.flip);
        SDL_RenderPresent(mainRenderer);
        frame++;
        if (time(NULL) > startTime)
            now = time(NULL);
        if (time(NULL) - 1 > lastTime)
            lastTime = time(NULL);
        if (lastTime == now)
        printf("Framerate: %d\n", frame / ((int) now - (int) startTime));

    }
    printf("Quit successfully\n");
    closeSDL();
}
