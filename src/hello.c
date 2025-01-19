#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

/* Constants */
const Sint32 DEFAULT_WINDOW_WIDTH = 800;
const Sint32 DEFAULT_WINDOW_HEIGHT = 600;
const char *kFontName = "Inter-VariableFont.ttf";

struct LTexture gTextTexture;

// why typedef here?
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    Sint32 windowWidth;
    Sint32 windowHeight;
} AppState;

struct LTexture
{
    // Contains texture data
    SDL_Texture *mTexture;

    // Texture dimensions
    int mWidth;
    int mHeight;
};

void init(struct LTexture *textTexture)
{
    textTexture->mTexture = NULL;
    textTexture->mWidth = 0;
    textTexture->mHeight = 0;
}

void destroy(struct LTexture *textTexture)
{
    SDL_DestroyTexture(textTexture->mTexture);
    textTexture->mWidth = 0;
    textTexture->mHeight = 0;
}

int getWidth(struct LTexture *textTexture)
{
    return textTexture->mWidth;
}
int getHeight(struct LTexture *textTexture)
{
    return textTexture->mHeight;
}

SDL_AppResult loadFromRenderedText(struct LTexture *textTexture, void *appstate, char *textMessage, SDL_Color textColor)
{
    AppState *as = ((AppState *)appstate);

    // Clean up texture if it already exists
    destroy(textTexture);

    SDL_Surface *textSurface = TTF_RenderText_Blended(as->font, textMessage, 0, textColor);
    if (!textSurface)
    {
        SDL_Log("SDL could not load textSurface! SDL error: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    textTexture->mTexture = SDL_CreateTextureFromSurface(as->renderer, textSurface);
    if (!textTexture->mTexture)
    {
        SDL_Log("SDL could not load mTexture! SDL error: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    textTexture->mWidth = textSurface->w;
    textTexture->mHeight = textSurface->h;

    SDL_DestroySurface(textSurface);

    return SDL_APP_CONTINUE;
}

void render(struct LTexture *a, void *appstate, float x, float y)
{
    AppState *as = ((AppState *)appstate);
    const SDL_FRect dstRect = {
        .x = x,
        .y = y,
        .w = a->mWidth,
        .h = a->mHeight,
    };

    SDL_RenderTexture(as->renderer, a->mTexture, NULL, &dstRect);
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Set appstate = to our AppState struct before anything else, we then pollute the struct as neccesary.
    AppState *as = SDL_calloc(1, sizeof(AppState));
    if (!as)
    {
        return SDL_APP_FAILURE;
    }
    *appstate = as; // derefrenceing a void **appstate; thus *appstate will be a void *appstate pointer;

    /* Initialize SDL */
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Initialize SDL_ttf */
    if (!TTF_Init())
    {
        SDL_Log("Couldn't initialize TTF: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Have the window width&height be the default, may change if resized
    as->windowWidth = DEFAULT_WINDOW_WIDTH;
    as->windowHeight = DEFAULT_WINDOW_HEIGHT;

    if (!SDL_CreateWindowAndRenderer("Hello World", as->windowWidth, as->windowHeight, SDL_WINDOW_RESIZABLE, &as->window, &as->renderer))
    {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // is this needed?
    // TTF_TextEngine *textEngine = TTF_CreateRendererTextEngine(as->renderer);
    // if (!textEngine)
    // {
    //     SDL_Log("Couldn't create renderer text engine: %s\n", SDL_GetError());
    //     return SDL_APP_FAILURE;
    // }

    /* Font loading */
    char fontPath[512]; // fontPath is an array of 512 characters;
    const char *basePath = SDL_GetBasePath();
    SDL_strlcpy(fontPath, basePath, 512);
    SDL_strlcat(fontPath, kFontName, 512);
    as->font = TTF_OpenFont(fontPath, 32);
    if (!as->font)
    {
        SDL_Log("TTF_OpenFont issue: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    struct SDL_Color textColor = {255, 255, 255, 255};
    loadFromRenderedText(&gTextTexture, *appstate, "???", textColor);
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState *as = ((AppState *)appstate);
    if ((event->window).type == SDL_EVENT_WINDOW_RESIZED)
    {
        // SDL_Log("resized to %dx%d\n", (event->window).data1, (event->window).data2);
        as->windowWidth = (event->window).data1;
        as->windowHeight = (event->window).data2;
    }
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *as = ((AppState *)appstate);
    // const float scale = 4.0f;
    // SDL_SetRenderScale(as->renderer, scale, scale);

    /* Draw the message */
    SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, 255);
    SDL_RenderClear(as->renderer);
    render(&gTextTexture, appstate, ((as->windowWidth - getWidth(&gTextTexture)) / 2), ((as->windowHeight - getHeight(&gTextTexture)) / 2));

    SDL_RenderPresent(as->renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppState *as = ((AppState *)appstate);
    destroy(&gTextTexture);
    // Clean up fonts
    TTF_CloseFont(as->font);
    kFontName = NULL;
    TTF_Quit();
    // Clean up window/renderer
    SDL_DestroyRenderer(as->renderer);
    SDL_DestroyWindow(as->window);
    // Clean up appstate
    SDL_free(as);
    // Quit subsystems
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}
