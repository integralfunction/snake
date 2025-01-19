#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdlib.h>

#include "editbox.h"

#define DEFAULT_PTSIZE 18.0f
#define DEFAULT_TEXT "The quick brown fox jumped over the lazy dog"
// Newlines
// #define DEFAULT_TEXT    "The quick brown fox\njumped over the lazy dog"
// Combining characters
// #define DEFAULT_TEXT    "\xc5\xab\xcc\x80\x20\xe1\xba\x83\x20\x6e\xcc\x82\x20\x48\xcc\xa8\x20\x6f\xcd\x9c\x75"
// Chinese text
// #define DEFAULT_TEXT    "\xe5\xad\xa6\xe4\xb9\xa0\xe6\x9f\x90\xe8\xaf\xbe\xe7\xa8\x8b\xe5\xbf\x85\xe8\xaf\xbb\xe7\x9a\x84"
#define WIDTH 640
#define HEIGHT 480

typedef enum
{
    TextEngineNone,
    TextEngineSurface,
    TextEngineRenderer
} TextEngine;

typedef enum
{
    TextRenderSolid,
    TextRenderShaded,
    TextRenderBlended
} TextRenderMethod;

typedef struct
{
    bool done;
    SDL_Window *window;
    SDL_Surface *window_surface;
    SDL_Renderer *renderer;
    TTF_Font *font;
    TTF_Text *caption;
    SDL_Rect captionRect;
    SDL_Texture *message;
    SDL_FRect messageRect;
    TextEngine textEngine;
    SDL_FRect textRect;
    EditBox *edit;
} Scene;

static void DrawScene(Scene *scene)
{
    SDL_Renderer *renderer = scene->renderer;

    /* Clear the background to background color */
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    TTF_DrawRendererText(scene->caption, (float)scene->captionRect.x, (float)scene->captionRect.y);
    SDL_RenderTexture(renderer, scene->message, NULL, &scene->messageRect);
    SDL_RenderPresent(renderer);

    if (scene->window_surface)
    {
        SDL_UpdateWindowSurface(scene->window);
    }
}

int main(int argc, char *argv[])
{
    TTF_Font *font = NULL;
    SDL_Surface *text = NULL;
    Scene scene;
    float ptsize;
    int i;
    SDL_Event event;
    TTF_TextEngine *engine = NULL;
    TextRenderMethod rendermethod = TextRenderShaded;
    int renderstyle = TTF_STYLE_NORMAL;
    char *message, string[128];

    SDL_zero(scene);
    scene.textEngine = TextEngineRenderer;

    /* Open the font file with the requested point size */
    ptsize = 0.0f;
    ptsize = DEFAULT_PTSIZE;
    font = TTF_OpenFont(argv[0], ptsize);
    scene.font = font;

    /* Create a window */
    scene.window = SDL_CreateWindow("showfont demo", WIDTH, HEIGHT, 0);

    scene.renderer = SDL_CreateRenderer(scene.window, NULL);
    engine = TTF_CreateRendererTextEngine(scene.renderer);
    if (!engine)
    {
        SDL_Log("Couldn't create renderer text engine: %s\n", SDL_GetError());
        Cleanup(2);
    }

    message = DEFAULT_TEXT;

    text = TTF_RenderText_Solid(font, message, 0, *forecol);

    scene.messageRect.x = (float)((WIDTH - text->w) / 2);
    scene.messageRect.y = (float)((HEIGHT - text->h) / 2);
    scene.messageRect.w = (float)text->w;
    scene.messageRect.h = (float)text->h;
    scene.message = SDL_CreateTextureFromSurface(scene.renderer, text);
    SDL_Log("Font is generally %d big, and string is %d big\n",
            TTF_GetFontHeight(font), text->h);

    /* Wait for a keystroke, and blit text on mouse press */
    while (!scene.done)
    {
        DrawScene(&scene);
    }
    SDL_DestroySurface(text);
    TTF_DestroyText(scene.caption);
    TTF_CloseFont(font);
    TTF_DestroyRendererTextEngine(engine);

    SDL_DestroyTexture(scene.message);
    Cleanup(0);

    /* Not reached, but fixes compiler warnings */
    return 0;
}
