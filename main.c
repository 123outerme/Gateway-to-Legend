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
    initSDL(filePath);
    player person;
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(mainRenderer);
    initPlayer(&person, 0, 0, TILE_SIZE, TILE_ID_PLAYER);
    drawSprite(person.spr, person.flip);
    SDL_RenderPresent(mainRenderer);
    SDL_Event e;
    bool quit = false;
    player* playerSprite = &person;
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)  //while there are events in the queue
        {
            SDL_RenderClear(mainRenderer);
            if (e.type == SDL_QUIT)
                quit = true;
            const Uint8* keyStates = SDL_GetKeyboardState(NULL);
            if (!playerSprite->movementLocked && (checkSKUp || checkSKDown || checkSKLeft || checkSKRight))
            {
                if (!((checkSKUp && checkSKDown) || (checkSKLeft && checkSKRight)))
                {
                    SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}));
                    //drawTile(tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE], playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, SDL_FLIP_NONE);
                    if (playerSprite->spr.y > 0 && checkSKUp)
                        playerSprite->spr.y -= 4;
                    if (playerSprite->spr.y < SCREEN_HEIGHT - playerSprite->spr.h && checkSKDown)
                        playerSprite->spr.y += 4;
                    if (playerSprite->spr.x > 0 && checkSKLeft)
                        playerSprite->spr.x -= 4;
                    if (playerSprite->spr.x < SCREEN_WIDTH - playerSprite->spr.w && checkSKRight)
                        playerSprite->spr.x += 4;
                    if (checkSKLeft)
                        playerSprite->flip = SDL_FLIP_HORIZONTAL;
                    if (checkSKRight)
                        playerSprite->flip = SDL_FLIP_NONE;
                }
            }
            if (checkSKMenu)
                    quit = true;
            drawSprite(person.spr, person.flip);
            SDL_RenderPresent(mainRenderer);
        }
    }
    printf("Quit successfully\n");
    closeSDL();
}
