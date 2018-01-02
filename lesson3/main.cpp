#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "getResourcePath.h"
#include "cleanup.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int TILE_SIZE = 40;

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
  SDL_Texture *texture = IMG_LoadTexture(ren, filePath.c_str());
  if (texture == nullptr) {
    logSDLError(std::cout, "LoadTexture");
  }
  return texture;
}


/**
 * Render texture with a given width and height.
 * @param tex The source texture we want to draw
 * @param ren The renderer we want to draw to
 * @param x The x coordinate to draw to
 * @param y The y coordinate to draw to
 * @param w The width of the texture to draw
 * @param h The height of the texture to draw
 */
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h) {
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  dest.w = w;
  dest.h = h;

  SDL_RenderCopy(ren, tex, NULL, &dest);
}

/**
 * Render image with default width-height
 * @param tex The source texture we want to draw
 * @param ren The renderer we want to draw to
 * @param x The x coordinate to draw to
 * @param y The y coordinate to draw to
 */
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
  int w, h;
  SDL_QueryTexture(tex, NULL, NULL, &w, &h);
  renderTexture(tex, ren, x, y, w, h);
}

int main(int, char**) {
  std::cout << "Resource path is the deal" << getResourcePath() << std::endl;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    logSDLError(std::cout, "IMG_Init");
    SDL_Quit();
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

  const std::string resPath = getResourcePath("Lesson3");
  SDL_Texture *background = loadTexture(resPath + "background.png", ren);
  SDL_Texture *image = loadTexture(resPath + "image.png", ren);
  if (background == nullptr || image == nullptr) {
    cleanup(background, image, ren, win);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  for (int i = 0; i < 3; ++i) {
    //First clear the renderer
    SDL_RenderClear(ren);


    const int xTiles = SCREEN_WIDTH / TILE_SIZE;
    const int yTiles = SCREEN_HEIGHT / TILE_SIZE;

    // jhare: I chose to use j for an index here instead of overwriting the i like in the example. bad bois.
    for (int j=0; j < xTiles * yTiles; ++j) {

      int x = j % xTiles;
      int y = j / xTiles;

      renderTexture(background, ren, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    }

    int iW, iH;
    SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
    int x = SCREEN_WIDTH /2 - iW /2;
    int y = SCREEN_HEIGHT /2 - iH /2;
    renderTexture(image, ren, x, y);

    // Show the whole deal
    SDL_RenderPresent(ren);

    //Take a quick break after all that hard work
    SDL_Delay(2000);
  }

  //cleanup(background, image, ren, win);
  //IMG_Quit();
	SDL_Quit();

	return 0;
}
