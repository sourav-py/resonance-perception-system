#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

const int NUM_BOXES=25;
const int BOX_SIZE=50;
const int RESONANCE_PROXIMITY=200; // Radius of the player's sense that activates on 'snapping'

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


//Update player's position based on the keyboard's keystates
void updatePlayerPosition(SDL_FPoint& player, float delta_time)
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

//Update the visibility of the boxes based on user' position and the 
//pre-defined 'resonance radar'
void updateBoxesVisibility(vector<box> &boxes, SDL_FPoint player){
  for(auto &box : boxes)
  {
    float dist = sqrt((box.rect.x - player.x)*(box.rect.x - player.x) + (box.rect.y - player.y)* (box.rect.y - player.y));
    if(dist <= RESONANCE_PROXIMITY)
      box.visibility = 1.0f;
    else
      box.visibility = 0.0f;

  }

}


int main(){
  SDL_Window *window = SDL_CreateWindow("White board", 0, 0, 1280, 720, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  //Enable transparency
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  //Set background to white
  SDL_SetRenderDrawColor(renderer, 191, 191, 191, 255);
  SDL_RenderClear(renderer);

  bool running = true;
  SDL_Event event;
  
  //Position variable for the player
  SDL_FPoint player = {640, 360}; 

  //Create an array at positions
  vector<box> boxes;
  srand(time(0));

  for(int i = 0; i < NUM_BOXES; i++){

    int side = BOX_SIZE;

    // Generate random position for the box 
    int p1 = rand()%1280;
    int p2 = rand()%720;

    //Consider a screen-wide grid where each cell is BOX_SIZE * BOX_SIZE
    //Each random box should fit in one of the grid cells
    p1 = p1 - p1%50;
    p2 = p2 - p2%50;

    SDL_Rect rect = {p1, p2, side, side};
    boxes.push_back({rect,1.0f});
  }

  //Boxes' visibility is conditional
  //Initially, on a key press (snapping)
  bool show_boxes = true;

  Uint32 visibility_checkpoint_time = SDL_GetTicks();
  Uint32 fps_delta_checkpoint_time = SDL_GetTicks(); 


  //Declaring outside for debugging purpose
  //variable is needed, defining outside the loop is not
  float delta_time;


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
	updateBoxesVisibility(boxes,player);
	visibility_checkpoint_time = SDL_GetTicks();
      }

    }

    //Timestamp for this frame
    Uint32 curr_time = SDL_GetTicks();

     //Check if 3 seconds have elapsed
    if(curr_time - visibility_checkpoint_time >= 3000)
      show_boxes = false;


    //Update player's position
    delta_time = (curr_time - fps_delta_checkpoint_time)/1000.0f;
    updatePlayerPosition(player, delta_time);
    

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

      //Render each rectangle present in the array
      for(auto box : boxes)
      {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, (int)255*box.visibility); 
	SDL_RenderFillRect(renderer, &box.rect);
      }

    }
    
    SDL_RenderPresent(renderer);
    fps_delta_checkpoint_time = curr_time;

  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
