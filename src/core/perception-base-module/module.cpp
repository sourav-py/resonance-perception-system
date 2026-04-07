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


/*
 * ---------------
 *  CUSTOM OBJECT DELCLRATIONS
 *  --------------
 */

struct box {
  SDL_Rect rect;
  float visibility; //ranges from 0.0 -> 1.0
};


/*
 * ----------------
 * HELPER FUNCTIONS
 * ----------------
 */

void UpdatePlayerPosition(SDL_FPoint& player, float delta_time)
{
    const float speed = 200.0f;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    if (keystate[SDL_SCANCODE_W])
        player.y -= speed * delta_time;

    if (keystate[SDL_SCANCODE_S])
        player.y += speed * delta_time;

    if (keystate[SDL_SCANCODE_A])
        player.x -= speed * delta_time;

    if (keystate[SDL_SCANCODE_D])
        player.x += speed * delta_time;

    //Clamp player's position
    //Do not player go out of the screen
    if (player.x < 0) player.x = 0;
    if (player.x > 1280) player.x = 1280;

    if (player.y < 0) player.y = 0;
    if (player.y > 720) player.y = 720;


}

int main(){
  SDL_Window *window = SDL_CreateWindow("White board", 0, 0, 1280, 720, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  //Enable transparency
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  //Set background to white
  SDL_SetRenderDrawColor(renderer, 191, 191, 191, 255);
  SDL_RenderClear(renderer);

  bool running = true;
  SDL_Event event;
  
  //Position variable for the player
  SDL_FPoint player = {640, 360}; 

  //Create an array of boxes with random dimensions and positions
  vector<box> boxes;
  srand(time(0));
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
  bool show_boxes = true;

  Uint32 visibility_checkpoint_time = SDL_GetTicks();
  Uint32 fps_delta_checkpoint_time = SDL_GetTicks(); 


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
	show_boxes = true;
	visibility_checkpoint_time = SDL_GetTicks();
      }

    }

    //Timestamp for this frame
    Uint32 curr_time = SDL_GetTicks();

     //Check if 3 seconds have elapsed
    if(curr_time - visibility_checkpoint_time >= 3000)
      show_boxes = false;


    //Update player's position
    float delta_time = (curr_time - fps_delta_checkpoint_time)/1000.0f;
    UpdatePlayerPosition(player, delta_time);
    

    //Paint the white(off-white) screen
    SDL_SetRenderDrawColor(renderer, 191, 191, 191, 255);
    SDL_RenderClear(renderer);

    //Render the player
    SDL_Rect playerBox = {(int)player.x-5, (int)player.y-5, 20, 20};
    SDL_SetRenderDrawColor(renderer, 66, 135, 245, 255);
    SDL_RenderFillRect(renderer, &playerBox);
    
    //Check if the window to show boxes is open
    //to be open for 3 seconds
    if(show_boxes)
    {
      //Set Render color to black (to draw rectangles)
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 

      //Render each rectangle present in the array
      for(auto box : boxes)
	SDL_RenderFillRect(renderer, &box.rect);

    }
    
    SDL_RenderPresent(renderer);
    fps_delta_checkpoint_time = curr_time;

  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
