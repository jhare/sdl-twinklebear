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

  const std::string resPath = getResourcePath("Lesson2");
  SDL_Texture *background = loadTexture(resPath + "background.bmp", ren);
  SDL_Texture *image = loadTexture(resPath + "image.bmp", ren);

  if (background == nullptr || image == nullptr) {
    cleanup(background, image, ren, win);
    SDL_Quit();
    return 1;
  }

  for (int i = 0; i < 3; ++i) {
    //First clear the renderer
    SDL_RenderClear(ren);

    // Cover the thing using the bounding width and height
    int bW, bH;
    SDL_QueryTexture(background, NULL, NULL, &bW, &bH); // pass them by reference
    renderTexture(background, ren, 0, 0);
    renderTexture(background, ren, bW, 0);
    renderTexture(background, ren, 0, bH);
    renderTexture(background, ren, bW, bH);

    // Center the smiley face using half-half formula
    int iW, iH;
    SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
    int x = SCREEN_WIDTH/2 - iW/2;
    int y = SCREEN_HEIGHT/2 - iH/2;
    renderTexture(image, ren, x, y);

    // Show the whole deal
    SDL_RenderPresent(ren);

    //Take a quick break after all that hard work
    SDL_Delay(1000);
  }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
