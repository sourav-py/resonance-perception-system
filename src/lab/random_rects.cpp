#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;


struct box {
  SDL_Rect rect;
  float visibility; //ranges from 0.0 -> 1.0
};

/*
 * This is an in-progress module.
 * Do not execute the program yet.
 * Need some optimizations/corrections before doing so.
 */

int main(){
  SDL_Window *window = SDL_CreateWindow("White board", 0, 0, 1280, 720, SDL_WINDOW_FULLSCREEN_DESKTOP);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  //Set background to white
  SDL_SetRenderDrawColor(renderer, 191, 191, 191, 255);
  SDL_RenderClear(renderer);

  bool running = true;
  SDL_Event event;

  //Seed the random generator with time
  srand(time(0));

  //Create an array of boxes with random dimensions and positions
  vector<box> boxes;

  for(int i=0;i<10;i++){
    // Generate random dimenstion
    int side = 20 + rand()%150;

    //Generate random position
    int p1 = rand()%1280;
    int p2 = rand()%720;

    SDL_Rect rect = {p1, p2, side, side};
    boxes.push_back({rect,1.0f});
  }

  //Boxes' visibility is conditional
  //Initially, on a key press (snapping)
  bool showBoxes = true;

  Uint32 checkPointTime = SDL_GetTicks();


  /*
   * Let the rendering begin
   */
  while(running){

    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
	running = false;
      else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t)
      {
	showBoxes = true;
	checkPointTime = SDL_GetTicks();
      }

    }

    //Check if 3 seconds have elapsed
    Uint32 currTime = SDL_GetTicks();
    if(currTime - checkPointTime >= 3000)
      showBoxes = false;    

    //Paint the white(off-white) screen
    SDL_SetRenderDrawColor(renderer, 191, 191, 191, 191);
    SDL_RenderClear(renderer);
    
    //Check if the window to show boxes is open
    //to be open for 3 seconds
    if(showBoxes)
    {
      //Set Render color to black (to draw rectangles)
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 

      //Render each rectangle present in the array
      for(auto box : boxes)
	SDL_RenderFillRect(renderer, &box.rect);

    }
    
    SDL_RenderPresent(renderer);

  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
