#include <sys/stat.h>
#include <time.h>

#include <SDL2/SDL.h>

#define HELP "\
Arguments:\n\
x, y: x and y position\n\
z: zoom\n\
i: iterations\n\
r: resolution\n\
s: enable screenshot\n\
q: quit on finish\n\
Example:\n\
mandelbrot x.25 y0 z9.9 i300 s q\
"

double PosX, PosY, Zoom;
int Iterations, Resolution;
_Bool BScreenshot, Quit;

SDL_Event Event;
SDL_Renderer * Renderer;
SDL_Window * Window;
SDL_Surface * Surface;

inline double remap(float n, float low1, float high1, float low2, float high2)
{
    return low2 + n * (high2 - low2) / (high1 - low1);
}

void inloop()
{
    while(1) {
        if (SDL_PollEvent(&Event) == 0) {
            if(Event.type == SDL_QUIT)
                return;
            // TODO: input
        }
    }
}
int render()
{
    double Px, Py, x0, y0, x, y, x2, y2;
    int i;
    for(Py = 0; Py < Resolution; Py++) {
        for(Px = 0; Px < Resolution; Px++) {
            // remap screen to mandelbrot coords
            x0 = remap(Px, 0, Resolution, -1/Zoom, 1/Zoom) + PosX;
            y0 = remap(Py, 0, Resolution, -1/Zoom, 1/Zoom) + PosY;
            x = 0; y = 0; x2 = 0; y2 = 0; i = 0;
            // mandelbrot mathz
            while(x2 + y2 <= 4 && i < Iterations) {
                y = 2*x*y + y0;
                x = x2 - y2 + x0;
                x2 = x*x;
                y2 = y*y;
                i++;
            }
            // TODO: colors or something
            // remap i to fit rgb range
            i = remap(i, 0, Iterations, 0, 255);
            if(i == 255)
                i = 0;
            SDL_SetRenderDrawColor(Renderer, i, i, i, 255);
            SDL_RenderDrawPoint(Renderer, Px, Py);
        }
    }
    SDL_RenderPresent(Renderer);
    printf(
        "Rendered (%f, %f) w/ zoom %f & %d iterations\n",
        PosX, PosY, Zoom, Iterations
    );
}
void screenshot()
{
    Surface = SDL_CreateRGBSurface(
        0, Resolution, Resolution, 32, 0x00ff0000,
        0x0000ff00, 0x000000ff, 0xff000000
    );
    SDL_RenderReadPixels(
        Renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
        Surface->pixels, Surface->pitch
    );

    char filename[128];
    time_t timestamp = time(NULL);
    sprintf(
        filename,
        "out/%d (%.3f, %.3f) %.3f.bmp", 
        timestamp, PosX, PosY, Zoom
    );
    mkdir("out", 0774);
    SDL_SaveBMP(Surface, filename);
    printf("Saved screenshot at '%s'\n", filename);
}

int main(int argc, char *argv[])
{
    PosY = 0;
    PosY = 0;
    Zoom = 1;
    Iterations = 150;
    Resolution = 512;
    BScreenshot = 0;
    Quit = 0;

    if (argc < 2) {
        puts(HELP);
        return 0;
    }

    // read args
    for (int i = 1; i < argc; i++) {
        char * arg = argv[i];
        char argpre = arg[0];
        for (int j = 0; j < strlen(arg); j++)
            arg[j] = arg[j + 1];

        switch (argpre) {
            case 'x':
                sscanf(arg, "%lf", &PosX);
                break;
            case 'y':
                sscanf(arg, "%lf", &PosY);
                break;
            case 'z':
                sscanf(arg, "%lf", &Zoom);
                break;
            case 'i': 
                sscanf(arg, "%d", &Iterations);
                break;
            case 'r':
                sscanf(arg, "%d", &Resolution);
                break;
            case 's':
                BScreenshot = 1;
                break;
            case 'q':
                Quit = 1;
                break;
            default:
                printf("'%c': invalid argument\n", argpre);
                puts(HELP);
                return 0;
        }
    }

    // SDL init
    Window = SDL_CreateWindow(
        "Mandelbrot",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        Resolution, Resolution,
        SDL_WINDOW_SHOWN
    );
    Renderer = SDL_CreateRenderer(
        Window, -1, 0
    );

    render();

    if(BScreenshot)
        screenshot();

    if (!Quit)
        inloop();

    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_Quit();
    return 0;
}
