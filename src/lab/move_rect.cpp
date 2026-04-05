#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

int main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window = SDL_CreateWindow(
      "SDL Movement",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      800, 600,
      SDL_WINDOW_SHOWN
      );

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  bool running = true;
  SDL_Event event;

  // Rectangle position
  int x = 100, y = 100;

  while (running) {
    // Handle events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
	running = false;
      }

      if (event.type == SDL_KEYDOWN) {
	switch (event.key.keysym.sym) {
	  case SDLK_UP:    y -= 10; break;
	  case SDLK_DOWN:  y += 10; break;
	  case SDLK_LEFT:  x -= 10; break;
	  case SDLK_RIGHT: x += 10; break;
	}
      }
    }

    // Clear screen (black)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw rectangle (white)
    SDL_Rect rect = {x, y, 100, 100};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);

    //Draw a small rectangle on top of the existing rectangle
    // Small rectangle (antenna base test)
    SDL_Rect smallRect = {x + 20, y - 20, 20, 20};

    // Set color (RED so it's clearly visible)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    // Draw it
    SDL_RenderFillRect(renderer, &smallRect);


    // Show on screen
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
