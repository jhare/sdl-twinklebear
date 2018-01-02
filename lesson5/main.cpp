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

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dest, SDL_Rect *clip = nullptr) {
  SDL_RenderCopy(ren, tex, clip, &dest);
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip = nullptr) {
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;

  if (clip != nullptr) {
    dest.w = clip->w;
    dest.h = clip->h;
  } else {
    SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h); // otherwise read full wxh
  }

  renderTexture(tex, ren, dest, clip);
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

  const std::string resPath = getResourcePath("Lesson5");
  SDL_Texture *image = loadTexture(resPath + "image.png", ren);
  if ( image == nullptr) {
    cleanup(image, ren, win);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  int iW = 100, iH = 100;
  int x = SCREEN_WIDTH /2 - iW /2;
  int y = SCREEN_HEIGHT /2 - iH /2;

  SDL_Rect clips[4];
  for (int i = 0; i < 4; ++i) {
    clips[i].x = i / 2 * iW;
    clips[i].y = i % 2 * iH;
    clips[i].w = iW;
    clips[i].h = iH;
  }
  int useClip = 0;

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym){
          case SDLK_1:
            useClip = 0;
            break;
          case SDLK_2:
            useClip = 1;
            break;
          case SDLK_3:
            useClip = 2;
            break;
          case SDLK_4:
            useClip = 3;
            break;
          case SDLK_ESCAPE:
            quit = true;
            break;
          default:
            break;
        }
      }

      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_RenderClear(ren);
    // Show the whole deal
    renderTexture(image, ren, x, y, &clips[useClip]);
    SDL_RenderPresent(ren);
  }

  cleanup(image, ren, win);
  IMG_Quit();
	SDL_Quit();

	return 0;
}
