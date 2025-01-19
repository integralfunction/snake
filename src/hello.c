#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

/* Constants */
const int kScreenWidth = 800;
const int kScreenHeight = 600;
const char *kFontName = "Inter-VariableFont.ttf";

struct LTexture gTextTexture;
// why typedef here?
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Texture *messageTex;
    SDL_FRect messageDest;
} AppState;

struct LTexture
{
    // Contains texture data
    SDL_Texture *mTexture;

    // Texture dimensions
    int mWidth;
    int mHeight;
};

void init(struct LTexture *a)
{
    // a->mTexture = NULL;
    a->mWidth = 0;
    a->mHeight = 0;
}

void destroy(struct LTexture *a)
{
    SDL_DestroyTexture(a->mTexture);
    a->mWidth = 0;
    a->mHeight = 0;
}

int getWidth(struct LTexture *a)
{
    return a->mWidth;
}
int getHeight(struct LTexture *a)
{
    return a->mHeight;
}

SDL_AppResult loadFromRenderedText(struct LTexture *a, void *appstate, char *textMessage, SDL_Color textColor)
{
    AppState *as = ((AppState *)appstate);

    // Clean up texture if it already exists
    destroy(a);

    SDL_Surface *textSurface = TTF_RenderText_Blended(as->font, textMessage, 0, textColor);
    if (!textSurface)
    {
        SDL_Log("SDL could not load textSurface! SDL error: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    a->mTexture = SDL_CreateTextureFromSurface(as->renderer, textSurface);
    if (!a->mTexture)
    {
        SDL_Log("SDL could not load mTexture! SDL error: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    a->mWidth = textSurface->w;
    a->mHeight = textSurface->h;

    SDL_DestroySurface(textSurface);

    return SDL_APP_CONTINUE;
}

void render(struct LTexture *a, void *appstate, float x, float y)
{
    AppState *as = ((AppState *)appstate);
    // get the on-screen dimensions of the text. this is necessary for rendering it
    // SDL_PropertiesID texprops = SDL_GetTextureProperties(messageTexture);
    // TTF_GetTextSize();
    const SDL_FRect destTextRect = {
        .x = x,
        .y = y,
        .w = a->mWidth,
        .h = a->mHeight,
    };

    SDL_RenderTexture(as->renderer, a->mTexture, NULL, &destTextRect);
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

    // TTF_TextEngine *textEngine = TTF_CreateRendererTextEngine(as->renderer);
    // if (!textEngine)
    // {
    //     SDL_Log("Couldn't create renderer text engine: %s\n", SDL_GetError());
    //     return SDL_APP_FAILURE;
    // }
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
    as->font = TTF_OpenFont(fontPath, 64);
    if (!as->font)
    {
        SDL_Log("TTF_OpenFont issue: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // render the font to a surface
    // const char *text = "ttf fonts yay";
    struct SDL_Color textColor = {255, 255, 255, 255};
    // SDL_Surface *messageSurface = TTF_RenderText_Solid(as->font, text, 0, textColor);

    // make a texture from the surface
    // SDL_Texture *messageTexture = SDL_CreateTextureFromSurface(as->renderer, messageSurface);

    // we no longer need the font or the surface, so we can destroy those now.
    // TTF_CloseFont(as->font);
    // SDL_DestroySurface(messageSurface);

    // get the on-screen dimensions of the text. this is necessary for rendering it
    // SDL_PropertiesID texprops = SDL_GetTextureProperties(messageTexture);
    // TTF_GetTextSize();
    // const SDL_FRect destTextRect = {
    //     .x = kScreenWidth / 4.0 / 2.0,
    //     .y = kScreenHeight / 4.0 / 2.0,
    //     .w = (SDL_GetNumberProperty(texprops, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
    //     .h = (SDL_GetNumberProperty(texprops, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))};

    // struct SDL_Color textColor = {255, 255, 255, 255};
    loadFromRenderedText(&gTextTexture, *appstate, "???", textColor);
    // as->messageTex = messageTexture;
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
    AppState *as = ((AppState *)appstate);
    // const float scale = 4.0f;
    /* Center the message and scale it up */
    // SDL_GetRenderOutputSize(as->renderer, &w, &h); // why is this needed?
    // SDL_SetRenderScale(as->renderer, scale, scale);

    /* Draw the message */
    SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, 255);
    SDL_RenderClear(as->renderer);
    SDL_SetRenderDrawColor(as->renderer, 255, 255, 255, 255);
    SDL_RenderTexture(as->renderer, as->messageTex, NULL, &as->messageDest);

    render(&gTextTexture, appstate, 0, 0);
    render(&gTextTexture, appstate, ((kScreenWidth - getWidth(&gTextTexture)) / 2), ((kScreenHeight - getHeight(&gTextTexture)) / 2));
    SDL_RenderPresent(as->renderer);

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
