#include "ltexture.h"
#include <SDL_image.h>
#include <SDL.h>
#include <string>
#include <SDL_ttf.h>

LTexture::LTexture() {
    // Initialise
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}
LTexture::~LTexture() {
    free();
}

void LTexture::free() {
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = mHeight = 0;
    }
}

bool LTexture::loadFromFile(const std::string &path, SDL_Renderer *renderer, const SDL_Color *colorKey /*= nullptr*/) {
    free();

    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        printf("Failed to load image %s. IMG Error: %s\n", path.c_str(), IMG_GetError());
    }
    else {
        if (colorKey != nullptr) 
            SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, colorKey->r, colorKey->g, colorKey->b ) );

        mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (mTexture == nullptr) {
            printf("Failed to create texture. SDL Error: %s\n", SDL_GetError());
        }
        else {
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        SDL_FreeSurface(loadedSurface);
    }

    return mTexture != nullptr;
}

#ifdef SDL_TTF_MAJOR_VERSION
bool LTexture::loadFromRenderedText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color textColor) {
    free();
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (textSurface == nullptr) {
        printf("Failed to render text surface. SDL_ttf Error: %s\n", TTF_GetError());
    } else {
        mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (mTexture == nullptr) {
            printf("Failed to create texture from rendered text. SDL Error: %s\n", SDL_GetError());
        } else {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        SDL_FreeSurface(textSurface);
    }
    return mTexture != nullptr;
}
#endif

void LTexture::setColorMod(uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetTextureColorMod(mTexture, r, g, b);
}

void LTexture::setBlendMod(SDL_BlendMode blend){
    SDL_SetTextureBlendMode(mTexture, blend);
}

void LTexture::setAlpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(SDL_Renderer *renderer, int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip) const {
    // Set rendering space
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    if (clip != nullptr) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
}

// Get image width
int LTexture::getWidth() const {
    return mWidth;
}
// Get image height;
int LTexture::getHeight() const {
    return mHeight;
}