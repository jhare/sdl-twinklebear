#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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

SDL_Texture* renderText(const std::string &message, const std::string &fontFile, SDL_Color color, int fontSize, SDL_Renderer *renderer) {
  TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
  if (font == nullptr) {
    logSDLError(std::cout, "TTF_OpenFont");
    return nullptr;
  }

  SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
  if (surf == nullptr) {
    TTF_CloseFont(font);
    logSDLError(std::cout, "TTF_RenderText");
    return nullptr;
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
  if (texture == nullptr) {
    logSDLError(std::cout, "CreateTexture");
  }

  SDL_FreeSurface(surf);
  TTF_CloseFont(font);

  return texture;
}

int main(int, char**) {
  std::cout << "Resource path is the deal" << getResourcePath() << std::endl;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

  if (TTF_Init() != 0) {
    logSDLError(std::cout, "TTF_Init");
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

  const std::string resPath = getResourcePath("Lesson6");
  SDL_Color color = { 255, 255, 255, 255 };
  SDL_Texture *image = renderText("TTF fonts are cool!", resPath + "sample.ttf", color, 64, ren);

  if (image == nullptr) {
    cleanup(ren, win);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  int iW, iH;
  SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
  int x = SCREEN_WIDTH / 2 - iW / 2;
  int y = SCREEN_HEIGHT / 2 - iH / 2;

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym){
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
    renderTexture(image, ren, x, y);
    SDL_RenderPresent(ren);
  }

  cleanup(image, ren, win);
	SDL_Quit();

	return 0;
}
