#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static int kScreenWidth = 800;
static int kScreenHeight = 600;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
} AppState;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // init TTF
    if (!TTF_Init())
    {
        return SDL_APP_FAILURE;
    }

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", kScreenWidth, kScreenHeight, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Font loading */
    const char *basePath = SDL_GetBasePath();
    // TODO: strcat is unsafe? buffer overflow?
    const char *fontPath = strcat((char *)basePath, "Inter-VariableFont.ttf"); // explicit cast for const
    // SDL_Log("%s\n", fontPath);
    TTF_Font *font = TTF_OpenFont(fontPath, 36);
    if (!font)
    {
        SDL_Log("TTF_OpenFont issue: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // render the font to a surface
    const char *text = "ttf fonts yay";
    struct SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *messageSurface = TTF_RenderText_Solid(app->font, text, sizeof(text), textColor);

    // make a texture from the surface
    SDL_Texture *messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);

    AppState *as = SDL_calloc(1, sizeof(AppState));
    if (!as)
    {
        return SDL_APP_FAILURE;
    }
    as->renderer = renderer;
    as->window = window;
    *appstate = as;

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

    const char *message = "Hello World!";
    int w = 0, h = 0;
    float x, y;
    const float scale = 4.0f;
    /* Center the message and scale it up */
    // SDL_GetRenderOutputSize(app->renderer, &w, &h);
    SDL_SetRenderScale(app->renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    // const SDL_FRect *srcrect;
    // const SDL_FRect *dstrect;

    // get the on-screen dimensions of the text. this is necessary for rendering it
    SDL_PropertiesID texprops = SDL_GetTextureProperties(messageTexture);
    const SDL_FRect destTextRect = {
        // .x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2,
        // .y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2,
        .x = kScreenWidth / scale / 2,
        .y = kScreenHeight / scale / 2,
        // .x = 0,
        // .y = 0,
        .w = (SDL_GetNumberProperty(texprops, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
        .h = (SDL_GetNumberProperty(texprops, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))};
    /* Draw the message */
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    // SDL_RenderDebugText(app->renderer, x, y, message);
    SDL_RenderTexture(app->renderer, messageTexture, NULL, &destTextRect);

    SDL_RenderPresent(app->renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppState *app = ((AppState *)appstate);
    TTF_CloseFont(app->font);
    // we no longer need the font or the surface, so we can destroy those now.
    // SDL_DestroySurface(messageSurface);
}
