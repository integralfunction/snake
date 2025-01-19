#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

// const SDL_Window *window = NULL;
// const SDL_Renderer *renderer = NULL;
const int kScreenWidth = 800;
const int kScreenHeight = 600;
const char *kFontName = "Inter-VariableFont.ttf";

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *messageTex;
    SDL_FRect messageDest;
} AppState;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    /* Initialize SDL */
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return SDL_APP_FAILURE;
    }

    /* Initialize SDL_ttf */
    if (!TTF_Init())
    {
        SDL_Log("Couldn't initialize TTF: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Set appstate = to our AppState struct before anything else, we then pollute the struct as neccesary.
    AppState *as = SDL_calloc(1, sizeof(AppState));
    if (!as)
    {
        return SDL_APP_FAILURE;
    }
    *appstate = as; // derefrenceing a void **appstate; thus *appstate will be a void *appstate pointer;

    /* Create the window */
    as->window = SDL_CreateWindow("Window Title", kScreenWidth, kScreenHeight, SDL_WINDOW_RESIZABLE);
    if (!as->window)
    {
        SDL_Log("Couldn't create window: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Create the renderer */
    as->renderer = SDL_CreateRenderer(as->window, NULL); // Create renderer, let SDL choose a rendering driver
    if (!as->renderer)
    {
        SDL_Log("Couldn't create renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Log drivers that are available, in the order of priority SDL chooses them.
    // for (int i = 0; i < SDL_GetNumRenderDrivers(); i++)
    // {
    //     SDL_Log("driver: %s\n", SDL_GetRenderDriver(i));
    // }

    // if (!SDL_CreateWindowAndRenderer("Hello World", kScreenWidth, kScreenHeight, SDL_WINDOW_RESIZABLE, &window, &renderer))
    // {
    //     SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
    //     return SDL_APP_FAILURE;
    // }

    /* Font loading */
    char fontPath[512]; // since each char in c has a sizeof(char)=1, buf is an array of 512 characters;
    const char *basePath = SDL_GetBasePath();
    // strcpy(fontPath, basePath); // A buffer overflow can easily happen here if you just move the project to a really long path which exceeds 512 characters.
    // strcat(fontPath, kFontName);
    SDL_strlcpy(fontPath, basePath, 512);
    SDL_strlcat(fontPath, kFontName, 512);
    // SDL_Log("%s\n", buf);
    TTF_Font *font = TTF_OpenFont(fontPath, 16);
    if (!font)
    {
        SDL_Log("TTF_OpenFont issue: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // render the font to a surface
    const char *text = "ttf fonts yay";
    struct SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *messageSurface = TTF_RenderText_Solid(font, text, 0, textColor);

    // make a texture from the surface
    SDL_Texture *messageTexture = SDL_CreateTextureFromSurface(as->renderer, messageSurface);

    // we no longer need the font or the surface, so we can destroy those now.
    TTF_CloseFont(font);
    SDL_DestroySurface(messageSurface);

    // get the on-screen dimensions of the text. this is necessary for rendering it
    SDL_PropertiesID texprops = SDL_GetTextureProperties(messageTexture);
    // TTF_GetTextSize();
    const SDL_FRect destTextRect = {
        .x = kScreenWidth / 4.0 / 2.0,
        .y = kScreenHeight / 4.0 / 2.0,
        .w = (SDL_GetNumberProperty(texprops, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
        .h = (SDL_GetNumberProperty(texprops, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))};

    as->messageTex = messageTexture;
    as->messageDest = destTextRect;
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
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
    AppState *app = ((AppState *)appstate);
    const float scale = 4.0f;
    // const char *message = "Hello World!";
    // int w = 0, h = 0;
    // float x, y;
    /* Center the message and scale it up */
    // SDL_GetRenderOutputSize(app->renderer, &w, &h); // why is this needed?
    SDL_SetRenderScale(app->renderer, scale, scale);
    // x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    // y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    /* Draw the message */
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    // SDL_RenderDebugText(app->renderer, x, y, message);
    SDL_RenderTexture(app->renderer, app->messageTex, NULL, &app->messageDest);

    SDL_RenderPresent(app->renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // AppState *app = ((AppState *)appstate);
    // TTF_CloseFont(app->font);
    // we no longer need the font or the surface, so we can destroy those now.
    // SDL_DestroySurface(messageSurface);
}
