#include <iostream>
#include <SDL2/SDL.h>
#include "getResourcePath.h"
#include "cleanup.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/**
 * Log an SDL error with some error message to the output stream of our choice
 * @param os The output stream to write the message to
 * @param msg The error message to write, format will be msg error: SDL_GetError()
 */
void logSDLError(std::ostream &os, const std::string &msg) {
  os << msg << " error: " << SDL_GetError() << std::endl; 
}

class Rectangle {
    int width, height;
  public:
    void set_values (int,int);
    int area (void);
} rect;

/**
 * Loads a BMP image into a texture on the rendering device
 * @param filePath the BMP image file to load
 * @param ren The renderer to load the texture into
 * @return the loaded texture, or nullptr if something went wrong
 */
SDL_Texture* loadTexture(const std::string &filePath, SDL_Renderer *ren) {
  SDL_Texture *texture = nullptr;
  SDL_Surface *loadedImage = SDL_LoadBMP(filePath.c_str());

  if (loadedImage != nullptr) {
    texture = SDL_CreateTextureFromSurface(ren, loadedImage);
    SDL_FreeSurface(loadedImage);

    if (texture == nullptr) {
      logSDLError(std::cout, "SDL_CreateTextureFromSurface");
    }
  } else {
    logSDLError(std::cout, "LoadBMP");
  }

  return texture;
}

/**
 * Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
 * the texture's width-height ratio.
 * @param tex The source texture we want to draw
 * @param ren The renderer we want to drwa to
 * @param x The x coordinate to draw to
 * @param y The y coordiante to draw to
 */
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;

  SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);
  SDL_RenderCopy(ren, tex, NULL, &dest);
}

int main(int, char**) {
  std::cout << "Resource path is the deal" << getResourcePath() << std::endl;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

  SDL_Window *win = SDL_CreateWindow("jhare-sdl-lesson2", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (win == nullptr) {
    logSDLError(std::cout, "CreateWindow");
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == nullptr) {
    cleanup(win);
    logSDLError(std::cout, "CreateRenderer");
    SDL_Quit();
    return 1;
  }

  std::string imagePath = getResourcePath("Lesson1") + "hello.bmp";
  SDL_Surface *bmp = SDL_LoadBMP(imagePath.c_str());
  if (bmp == nullptr) {
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
  SDL_FreeSurface(bmp); // we don't need this any more
  if (tex == nullptr) {
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  //A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
  for (int i = 0; i < 3; ++i) {
    //First clear the renderer
    SDL_RenderClear(ren);
    //Draw the texture
    SDL_RenderCopy(ren, tex, NULL, NULL);
    //Update the screen
    SDL_RenderPresent(ren);
    //Take a quick break after all that hard work
    SDL_Delay(1000);
  }

  SDL_DestroyTexture(tex);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
