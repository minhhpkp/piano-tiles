#ifndef LTEXTURE_H
#define LTEXTURE_H

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class LTexture {
    public:
        LTexture();
        ~LTexture();
       
        // Deallocate the texture
        void free();

        // Load image from the specified path with *renderer* as the rendering context
        bool loadFromFile(const std::string &path, SDL_Renderer *renderer, const SDL_Color *colorKey = nullptr);

        // Set color modulation
        void setColorMod(uint8_t r, uint8_t g, uint8_t b);

        // Set blending mode
        void setBlendMod(SDL_BlendMode blend);

        // Set alpha modulation
        void setAlpha(Uint8 alpha);

        #ifdef SDL_TTF_MAJOR_VERSION        
        // Create image from font string
        bool loadFromRenderedText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color textColor);
        #endif
        
        /**
         * Render the texture at a given point
         *
         * \param renderer the rendering context
         * \param x the horizontal coordinate of the point
         * \param y the vertical coordinate of the point
         * \param clip the portion of the texture to be rendered
         * \param angle an angle in degrees that indicates the rotation that will be
         *              applied to the render quad, rotating it in a clockwise direction
         * \param center a pointer to a point indicating the point around which
         *               dstrect will be rotated (if NULL, rotation will be done
         *               around the point at the middle of the render quad)
         * \param flip a SDL_RendererFlip value stating which flipping actions should
         *             be performed on the texture
         */
        void render(SDL_Renderer *renderer, int x, int y, SDL_Rect *clip = nullptr, 
                    double angle = 0.0, SDL_Point *center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE) const;

        // Get image width
        int getWidth() const;
        // Get image height;
        int getHeight() const;
    private:
        SDL_Texture *mTexture;
        
        int mWidth, mHeight;
};

#endif // LTEXTURE_H