
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./constants.h"

int game_is_running = 0;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;


int initialize_window(){
   if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "Error initializing SDL.\n");
        return 0;
    }
    window = SDL_CreateWindow(
        "Maxat",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_WIDTH,
        0
    );
    if(!window){
        fprintf(stderr, "Error in window\n");
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    
    if(!renderer){
        return 0;
    }
    return 1; 
}

void process_input(void){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
        case SDL_QUIT:
        game_is_running = 0;
        break;
        case SDL_KEYDOWN:
        if(event.key.keysym.sym == SDLK_ESCAPE){
game_is_running = 0;
        }
        
        break;
    }
}
    }

 void setup(void){
    ball.x = 20;
    ball.y = 20;
    ball.height = 15;
    ball.height = 15;
    //todo
}   

void update(void){
    //todo
}


void render(void){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
   //Draw a rectangle
   SDL_Rect ball_rect = {
        (int)ball.x,
        (int)ball.y,
        (int)ball.width,
        (int)ball.height
    };
    //fill the rectangle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderFillRect(renderer, &ball_rect);
    //swapping the buffers
    SDL_RenderPresent(renderer); 
    //todo
}



void destroy_window(void){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}
int main (int argc, char *argv[]){
    game_is_running = initialize_window();

    setup();
    while(game_is_running){
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}