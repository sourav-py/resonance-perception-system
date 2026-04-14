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

//Preferences
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int NUM_BOXES=25;
const int BOX_SIZE=50;
const int RESONANCE_PROXIMITY=200; // Radius of the player's sense that activates on 'snapping'

//  --------------------------
//  CUSTOM OBJECT DELCLRATIONS
//  --------------------------


// Object to define a standard 'obstacle' in the sandbox
struct Box {
  SDL_Rect rect;
  float visibility; //ranges from 0.0 -> 1.0
};

// Object to define the state of the game at any given time
struct GameState {
  bool running = true;
  SDL_FPoint player; //player position
  vector<Box> boxes;
  bool show_boxes = true;
  Uint32 visibility_checkpoint_time; // The reference timestamp to update the visibility of the 'boxes' 
};


// ----------------
// HELPER FUNCTIONS
// ----------------

// Initialize the set of variables required for the POC
GameState initializeGameState(){

  SDL_FPoint player = {640, 360}; 

  //Create an array at positions
  vector<Box> boxes;
  srand(time(0));

  for(int i = 0; i < NUM_BOXES; i++){

    // Generate random position for the box 
    int p1 = rand()%SCREEN_WIDTH;
    int p2 = rand()%SCREEN_HEIGHT;

    //Consider a screen-wide grid where each cell is BOX_SIZE * BOX_SIZE
    //Each random box should fit in one of the grid cells
    p1 = p1 - p1%50;
    p2 = p2 - p2%50;

    SDL_Rect rect = {p1, p2, BOX_SIZE, BOX_SIZE};
    boxes.push_back({rect,1.0f});
  }

   GameState state = {true, player, boxes, true, SDL_GetTicks()};
   return state; 
}

//Function to render the game state
void render(SDL_Renderer *renderer, GameState &state)
{
  //Enable transparency
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

   //Paint the white(off-white) screen
  SDL_SetRenderDrawColor(renderer, 191, 191, 191, 255);
  SDL_RenderClear(renderer);

  //Render the player
  SDL_Rect playerBox = {(int)state.player.x-5, (int)state.player.y-5, 20, 20};
  SDL_SetRenderDrawColor(renderer, 66, 135, 245, 255);
  SDL_RenderFillRect(renderer, &playerBox);
  
  //Check if the window to show boxes is open
  //to be open for 3 seconds
  if(state.show_boxes)
  {
    for(const auto &box : state.boxes)
    {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, (int)255*box.visibility); 
      SDL_RenderFillRect(renderer, &box.rect);
    }
  }
  
  SDL_RenderPresent(renderer);
} 




//Update player's position based on the keyboard's keystates
void updatePlayerPosition(SDL_FPoint& player, float delta_time) {
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
    if (player.x > SCREEN_WIDTH) player.x = SCREEN_WIDTH;

    if (player.y < 0) player.y = 0;
    if (player.y > SCREEN_HEIGHT) player.y = SCREEN_HEIGHT;


}

//Update the visibility of the boxes based on user' position and the 
//pre-defined 'resonance radar'
void updateBoxesVisibility(vector<Box> &boxes, SDL_FPoint player){
  for(auto &box : boxes)
  {
    float dist = sqrt((box.rect.x - player.x)*(box.rect.x - player.x) + (box.rect.y - player.y)* (box.rect.y - player.y));
    if(dist <= RESONANCE_PROXIMITY)
      box.visibility = 1.0f;
    else
      box.visibility = 0.0f;

  }

}

//Handle user's input and update game's state accordingly
void HandleInput(SDL_Event &event, GameState &state)
{
  if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
    state.running = false;
  else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t)
  {
    state.show_boxes = true;
    updateBoxesVisibility(state.boxes,state.player);
    state.visibility_checkpoint_time = SDL_GetTicks();
  }
}

int main(){

  SDL_Window *window = SDL_CreateWindow("White board", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
 
  GameState state = initializeGameState();
  SDL_Event event;
  Uint32 fps_delta_checkpoint_time = SDL_GetTicks(); 

  // Let the rendering begin
  while(state.running){

    while(SDL_PollEvent(&event))
    {
	HandleInput(event, state);   
    }

    //Timestamp for this frame
    Uint32 curr_time = SDL_GetTicks();

     //Check if 3 seconds have elapsed
    if(curr_time - state.visibility_checkpoint_time >= 3000)
      state.show_boxes = false;


    //Update player's position
    float delta_time = (curr_time - fps_delta_checkpoint_time)/1000.0f;
    updatePlayerPosition(state.player, delta_time);
    
    render(renderer, state);

    fps_delta_checkpoint_time = curr_time;

  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
