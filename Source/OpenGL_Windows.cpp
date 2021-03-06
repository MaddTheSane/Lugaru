/*
Copyright (C) 2003, 2010 - Wolfire Games

This file is part of Lugaru.

Lugaru is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#ifdef WIN32
#define UINT8 WIN32API_UINT8
#define UINT16 WIN32API_UINT16
#define boolean WIN32API_boolean
#include <windows.h>
#undef UINT8
#undef UINT16
#undef boolean
#endif



#include "Game.h"
#include "SDL_OpenGL.h"
extern "C" {
#include "zlib.h"
#include "png.h"
#ifdef WIN32
#define INT32 INT32_jpeg
#include "jpeglib.h"
#undef INT32
#else
#include "jpeglib.h"
#endif
}

using namespace Game;

static bool load_image(const char * fname, TGAImageRec & tex);
static bool load_png(const char * fname, TGAImageRec & tex);
static bool load_jpg(const char * fname, TGAImageRec & tex);
bool save_image(const char * fname);
static bool save_png(const char * fname);


#include "openal_wrapper.h"
#include "Globals.h"
#include "Input.h"

// ADDED GWC
#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#endif

extern float multiplier;
extern float sps;
extern float realmultiplier;
extern int slomo;
extern bool cellophane;
extern float texdetail;

extern bool osx;
extern bool freeze;
extern bool stillloading;
extern int mainmenu;
/*extern*/
bool gameFocused;

extern float slomospeed;
extern float slomofreq;



#include <math.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "gamegl.h"
#include "MacCompatibility.h"
#include "Settings.h"

#ifdef WIN32
#include <shellapi.h>
#include "win-res/resource.h"
#else
#include <unistd.h>
#endif

using namespace std;

SDL_Rect **resolutions = NULL;

Boolean SetUp ();
void DoUpdate ();

void CleanUp (void);

// statics/globals (internal only) ------------------------------------------


#ifndef LUGARU_USE_NATIVE_OPENGL

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4273)
#endif

#define GL_FUNC(ret,fn,params,call,rt) \
    extern "C" { \
        static ret (GLAPIENTRY *p##fn) params = NULL; \
        ret GLAPIENTRY fn params { rt p##fn call; } \
    }
#include "glstubs.h"
#undef GL_FUNC

#ifdef _MSC_VER
#pragma warning(pop)
#endif

static bool lookup_glsym(const char *funcname, void **func)
{
    *func = SDL_GL_GetProcAddress(funcname);
    if (*func == NULL) {
        fprintf(stderr, "Failed to find OpenGL symbol \"%s\"\n", funcname);
        return false;
    }
    return true;
}

static bool lookup_all_glsyms(void)
{
    bool retval = true;
#define GL_FUNC(ret,fn,params,call,rt) \
        if (!lookup_glsym(#fn, (void **) &p##fn)) retval = false;
#include "glstubs.h"
#undef GL_FUNC
    return retval;
}

static void GLAPIENTRY glDeleteTextures_doNothing(GLsizei n, const GLuint *textures)
{
    // no-op.
}

#endif

// Menu defs

int kContextWidth;
int kContextHeight;

Boolean gDone = false;

#ifndef __MINGW32__
static int _argc = 0;
static char **_argv = NULL;
#endif

bool cmdline(const char *cmd)
{
    for (int i = 1; i < _argc; i++) {
        char *arg = _argv[i];
        while (*arg == '-')
            arg++;
        if (strcasecmp(arg, cmd) == 0)
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------------------------------

// OpenGL Drawing

void initGL()
{
    glClear( GL_COLOR_BUFFER_BIT );
    swap_gl_buffers();

    // clear all states
    glDisable( GL_ALPHA_TEST);
    glDisable( GL_BLEND);
    glDisable( GL_DEPTH_TEST);
    //glDisable( GL_DITHER);
    glDisable( GL_FOG);
    glDisable( GL_LIGHTING);
    glDisable( GL_LOGIC_OP);
    glDisable( GL_TEXTURE_1D);
    glDisable( GL_TEXTURE_2D);
    glPixelTransferi( GL_MAP_COLOR, GL_FALSE);
    glPixelTransferi( GL_RED_SCALE, 1);
    glPixelTransferi( GL_RED_BIAS, 0);
    glPixelTransferi( GL_GREEN_SCALE, 1);
    glPixelTransferi( GL_GREEN_BIAS, 0);
    glPixelTransferi( GL_BLUE_SCALE, 1);
    glPixelTransferi( GL_BLUE_BIAS, 0);
    glPixelTransferi( GL_ALPHA_SCALE, 1);
    glPixelTransferi( GL_ALPHA_BIAS, 0);

    // set initial rendering states
    glShadeModel( GL_SMOOTH);
    glClearDepth( 1.0f);
    glDepthFunc( GL_LEQUAL);
    glDepthMask( GL_TRUE);
    //glDepthRange( FRONT_CLIP, BACK_CLIP);
    glEnable( GL_DEPTH_TEST);
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glCullFace( GL_FRONT);
    glEnable( GL_CULL_FACE);
    glEnable( GL_LIGHTING);
    //glEnable( GL_LIGHT_MODEL_AMBIENT);
    glEnable( GL_DITHER);
    glEnable( GL_COLOR_MATERIAL);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glAlphaFunc( GL_GREATER, 0.5f);

    if ( CanInitStereo(stereomode) ) {
        InitStereo(stereomode);
    } else {
        fprintf(stderr, "Failed to initialize stereo, disabling.\n");
        stereomode = stereoNone;
    }
}

static void toggleFullscreen()
{
    Uint32 flags = SDL_GetWindowFlags(sdlwindow);
    if (flags & SDL_WINDOW_FULLSCREEN)
        flags &= ~SDL_WINDOW_FULLSCREEN;
    else
        flags |= SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(sdlwindow, flags);
}

static SDL_bool sdlEventProc(const SDL_Event &e)
{
    bool skipkey = false;
    SDL_Keycode sym;
    Uint16 mod;

    switch (e.type) {
	    case SDL_QUIT:
            return SDL_FALSE;

	    case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                return SDL_FALSE;

        case SDL_MOUSEMOTION:
            deltah += e.motion.xrel;
            deltav += e.motion.yrel;
            return SDL_TRUE;

		case SDL_MOUSEBUTTONDOWN:
			return SDL_TRUE;

		case SDL_MOUSEBUTTONUP:
            return SDL_TRUE;

        case SDL_KEYDOWN:
            sym = e.key.keysym.sym;
            mod = e.key.keysym.mod;

            if (sym == SDLK_g) {
                if (mod & KMOD_CTRL) {
                    skipkey = true;

                    SDL_bool mode = SDL_TRUE;
                    if ((SDL_GetWindowFlags(sdlwindow) & SDL_WINDOW_FULLSCREEN) == 0)
                        mode = SDL_GetWindowGrab(sdlwindow) ? SDL_FALSE : SDL_TRUE;
                    SDL_SetWindowGrab(sdlwindow, mode);
                    SDL_SetRelativeMouseMode(mode);
                }
            }

            #if PLATFORM_MACOSX
            else if (sym == SDLK_q) {
                if (mod & KMOD_GUI)
                    return SDL_FALSE;  // quit game
            }
            #endif

            else if (sym == SDLK_RETURN) {
                if (mod & KMOD_ALT) {
                    skipkey = true;
                    toggleFullscreen();
                }
            }

            return SDL_TRUE;

        case SDL_KEYUP:
            return SDL_TRUE;
    }

    return SDL_TRUE;
}



// --------------------------------------------------------------------------

static Point gMidPoint;

Boolean SetUp ()
{
    //char string[10];

    LOGFUNC;

    osx = 0;
    cellophane = 0;
    texdetail = 4;
    slomospeed = 0.25;
    slomofreq = 8012;

    DefaultSettings();

    const int displayIdx = 0;  // !!! FIXME: other monitors?
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        if (SDL_Init(SDL_INIT_VIDEO) == -1) {
            char buf[1024];
            snprintf(buf, sizeof (buf), "SDL_Init() failed: %s\n", SDL_GetError());
            fprintf(stderr, "%s\n", buf);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lugaru init failed!", buf, NULL);
            return false;
        }
    if (!LoadSettings()) {
        fprintf(stderr, "Failed to load config, creating default\n");
        SaveSettings();
    }
    if (kBitsPerPixel != 32 && kBitsPerPixel != 16) {
        kBitsPerPixel = 16;
    }

    if (SDL_GL_LoadLibrary(NULL) == -1) {
        char buf[1024];
        snprintf(buf, sizeof (buf), "SDL_GL_LoadLibrary() failed: %s\n", SDL_GetError());
        fprintf(stderr, "%s\n", buf);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lugaru init failed!", buf, NULL);
        SDL_Quit();
        return false;
    }

    int count = 0;
    const int nummodes = SDL_GetNumDisplayModes(displayIdx);
    for (int i = 0; i < nummodes; i++) {
        SDL_DisplayMode mode;
        if (SDL_GetDisplayMode(displayIdx, i, &mode) == -1)
            continue;
        if ((mode.w < 640) || (mode.h < 480))
            continue;  // sane lower limit.
        count++;
    }

    if (count == 0) {
        char buf[1024];
        snprintf(buf, sizeof (buf), "No suitable video resolutions found.");
        fprintf(stderr, "%s\n", buf);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lugaru init failed!", buf, NULL);
        SDL_Quit();
        return false;
    }

    static SDL_Rect *resolutions_block = NULL;
    resolutions_block = (SDL_Rect*) realloc(resolutions_block, sizeof (SDL_Rect) * count);
    resolutions = (SDL_Rect**) realloc(resolutions, sizeof (SDL_Rect *) * (count + 1));
    if ((resolutions_block == NULL) || (resolutions == NULL)) {
        char buf[1024];
        snprintf(buf, sizeof (buf), "Out of memory!");
        fprintf(stderr, "%s\n", buf);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lugaru init failed!", buf, NULL);
        SDL_Quit();
        fprintf(stderr, "Out of memory!\n");
        return false;
    }

    resolutions[count--] = NULL;
    for (int i = 0; count >= 0; i++, count--) {
        SDL_DisplayMode mode;
        if (SDL_GetDisplayMode(displayIdx, i, &mode) == -1)
            continue;
        if ((mode.w < 640) || (mode.h < 480))
            continue;  // sane lower limit.
        resolutions_block[count].x = resolutions_block[count].y = 0;
        resolutions_block[count].w = mode.w;
        resolutions_block[count].h = mode.h;
        resolutions[count] = &resolutions_block[count];
    }

    if (cmdline("showresolutions")) {
        printf("Resolutions we think are okay:\n");
        for (int i = 0; resolutions[i]; i++)
            printf("  %d x %d\n", (int) resolutions[i]->w, (int) resolutions[i]->h);
    }
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Uint32 sdlflags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (!cmdline("windowed"))
        sdlflags |= SDL_WINDOW_FULLSCREEN;
    if (!cmdline("nomousegrab"))
        sdlflags |= SDL_WINDOW_INPUT_GRABBED;
	if (cmdline("retina") && (sdlflags & SDL_WINDOW_FULLSCREEN) == 0) {
		sdlflags |= SDL_WINDOW_ALLOW_HIGHDPI;
	}

    sdlwindow = SDL_CreateWindow("Lugaru", SDL_WINDOWPOS_CENTERED_DISPLAY(displayIdx), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIdx),
                                 kContextWidth, kContextHeight, sdlflags);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
    if (!sdlwindow) {
        fprintf(stderr, "SDL_CreateWindow() failed: %s\n", SDL_GetError());
        fprintf(stderr, "forcing 640x480...\n");
        kContextWidth = 640;
        kContextHeight = 480;
        sdlwindow = SDL_CreateWindow("Lugaru", SDL_WINDOWPOS_CENTERED_DISPLAY(displayIdx), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIdx),
                                     kContextWidth, kContextHeight, sdlflags);
        if (!sdlwindow) {
            fprintf(stderr, "SDL_CreateWindow() failed: %s\n", SDL_GetError());
            fprintf(stderr, "forcing 640x480 windowed mode...\n");
            sdlflags &= ~SDL_WINDOW_FULLSCREEN;
            sdlwindow = SDL_CreateWindow("Lugaru", SDL_WINDOWPOS_CENTERED_DISPLAY(displayIdx), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIdx),
                                         kContextWidth, kContextHeight, sdlflags);

            if (!sdlwindow) {
                char buf[1024];
                snprintf(buf, sizeof (buf), "SDL_CreateWindow() failed: %s\n", SDL_GetError());
                fprintf(stderr, "%s\n", buf);
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lugaru init failed!", buf, NULL);
                SDL_Quit();
                return false;
            }
        }
    }

	windowWidth = kContextWidth;
	windowHeight = kContextHeight;
	SDL_GLContext glctx = SDL_GL_CreateContext(sdlwindow);
	SDL_GL_GetDrawableSize(sdlwindow, &kContextWidth, &kContextHeight);
    if (!glctx) {
        char buf[1024];
        snprintf(buf, sizeof (buf), "SDL_CreateContext() failed: %s\n", SDL_GetError());
        fprintf(stderr, "%s\n", buf);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lugaru init failed!", buf, NULL);
        SDL_Quit();
        return false;
    }

    SDL_GL_MakeCurrent(sdlwindow, glctx);

#ifndef LUGARU_USE_NATIVE_OPENGL
    if (!lookup_all_glsyms()) {
        char buf[1024];
        snprintf(buf, sizeof (buf), "Missing required OpenGL functions.");
        fprintf(stderr, "%s\n", buf);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lugaru init failed!", buf, NULL);
        SDL_Quit();
        return false;
    }
#endif

    int dblbuf = 0;
    if ((SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &dblbuf) == -1) || (!dblbuf)) {
        char buf[1024];
        snprintf(buf, sizeof (buf), "Failed to get a double-buffered context.");
        fprintf(stderr, "%s\n", buf);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lugaru init failed!", buf, NULL);
        SDL_Quit();
        return false;
    }

    if (SDL_GL_SetSwapInterval(-1) == -1)  // try swap_tear first.
        SDL_GL_SetSwapInterval(1);

    SDL_ShowCursor(0);
    SDL_SetWindowGrab(sdlwindow, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);


    initGL();

    GLint width = kContextWidth;
    GLint height = kContextHeight;
    gMidPoint.h = width / 2;
    gMidPoint.v = height / 2;
    screenwidth = width;
    screenheight = height;

    newdetail = detail;
    newscreenwidth = screenwidth;
    newscreenheight = screenheight;

    InitGame();

    return true;
}


static void DoMouse()
{

    if (mainmenu || ( (abs(deltah) < 10 * realmultiplier * 1000) && (abs(deltav) < 10 * realmultiplier * 1000) )) {
        deltah *= usermousesensitivity;
        deltav *= usermousesensitivity;
        mousecoordh += deltah;
        mousecoordv += deltav;
        if (mousecoordh < 0)
            mousecoordh = 0;
        else if (mousecoordh >= kContextWidth)
            mousecoordh = kContextWidth - 1;
        if (mousecoordv < 0)
            mousecoordv = 0;
        else if (mousecoordv >= kContextHeight)
            mousecoordv = kContextHeight - 1;
    }

}

void DoFrameRate (int update)
{
    static long frames = 0;

    static AbsoluteTime time = {0, 0};
    static AbsoluteTime frametime = {0, 0};
    AbsoluteTime currTime = UpTime ();
    double deltaTime = (float) AbsoluteDeltaToDuration (currTime, frametime);

    if (0 > deltaTime) // if negative microseconds
        deltaTime /= -1000000.0;
    else // else milliseconds
        deltaTime /= 1000.0;

    multiplier = deltaTime;
    if (multiplier < .001)
        multiplier = .001;
    if (multiplier > 10)
        multiplier = 10;
    if (update)
        frametime = currTime; // reset for next time interval

    deltaTime = (float) AbsoluteDeltaToDuration (currTime, time);

    if (0 > deltaTime) // if negative microseconds
        deltaTime /= -1000000.0;
    else // else milliseconds
        deltaTime /= 1000.0;
    frames++;
    if (0.001 <= deltaTime) { // has update interval passed
        if (update) {
            time = currTime; // reset for next time interval
            frames = 0;
        }
    }
}


void DoUpdate ()
{
    static float sps = 200;
    static int count;
    static float oldmult;

    DoFrameRate(1);
    if (multiplier > .6)
        multiplier = .6;

    fps = 1 / multiplier;

    count = multiplier * sps;
    if (count < 2)
        count = 2;

    realmultiplier = multiplier;
    multiplier *= gamespeed;
    if (difficulty == 1)
        multiplier *= .9;
    if (difficulty == 0)
        multiplier *= .8;

    if (loading == 4)
        multiplier *= .00001;
    if (slomo && !mainmenu)
        multiplier *= slomospeed;
    oldmult = multiplier;
    multiplier /= (float)count;

    DoMouse();

    TickOnce();

    for (int i = 0; i < count; i++) {
        Tick();
    }
    multiplier = oldmult;

    TickOnceAfter();
    /* - Debug code to test how many channels were active on average per frame
        static long frames = 0;

        static AbsoluteTime start = {0,0};
        AbsoluteTime currTime = UpTime ();
        static int num_channels = 0;

        num_channels += OPENAL_GetChannelsPlaying();
        double deltaTime = (float) AbsoluteDeltaToDuration (currTime, start);

        if (0 > deltaTime)  // if negative microseconds
            deltaTime /= -1000000.0;
        else                // else milliseconds
            deltaTime /= 1000.0;

        ++frames;

        if (deltaTime >= 1)
        {
            start = currTime;
            float avg_channels = (float)num_channels / (float)frames;

            ofstream opstream("log.txt",ios::app);
            opstream << "Average frame count: ";
            opstream << frames;
            opstream << " frames - ";
            opstream << avg_channels;
            opstream << " per frame.\n";
            opstream.close();

            frames = 0;
            num_channels = 0;
        }
    */
    if ( stereomode == stereoNone ) {
        DrawGLScene(stereoCenter);
    } else {
        DrawGLScene(stereoLeft);
        DrawGLScene(stereoRight);
    }
}

// --------------------------------------------------------------------------


void CleanUp (void)
{
    LOGFUNC;

    SDL_Quit();
#ifndef LUGARU_USE_NATIVE_OPENGL
#define GL_FUNC(ret,fn,params,call,rt) p##fn = NULL;
#include "glstubs.h"
#undef GL_FUNC
    // cheat here...static destructors are calling glDeleteTexture() after
    //  the context is destroyed and libGL unloaded by SDL_Quit().
    pglDeleteTextures = glDeleteTextures_doNothing;
#endif

}

// --------------------------------------------------------------------------

static bool IsFocused()
{
    return (SDL_GetWindowFlags(sdlwindow) & SDL_WINDOW_INPUT_FOCUS) == SDL_WINDOW_INPUT_FOCUS;
}



#ifndef WIN32
// (code lifted from physfs: http://icculus.org/physfs/ ... zlib license.)
static char *findBinaryInPath(const char *bin, char *envr)
{
    size_t alloc_size = 0;
    char *exe = NULL;
    char *start = envr;
    char *ptr;

    do {
        size_t size;
        ptr = strchr(start, ':');  /* find next $PATH separator. */
        if (ptr)
            *ptr = '\0';

        size = strlen(start) + strlen(bin) + 2;
        if (size > alloc_size) {
            char *x = (char *) realloc(exe, size);
            if (x == NULL) {
                if (exe != NULL)
                    free(exe);
                return(NULL);
            } /* if */

            alloc_size = size;
            exe = x;
        } /* if */

        /* build full binary path... */
        strcpy(exe, start);
        if ((exe[0] == '\0') || (exe[strlen(exe) - 1] != '/'))
            strcat(exe, "/");
        strcat(exe, bin);

        if (access(exe, X_OK) == 0) { /* Exists as executable? We're done. */
            strcpy(exe, start);  /* i'm lazy. piss off. */
            return(exe);
        } /* if */

        start = ptr + 1;  /* start points to beginning of next element. */
    } while (ptr != NULL);

    if (exe != NULL)
        free(exe);

    return(NULL);  /* doesn't exist in path. */
} /* findBinaryInPath */


char *calcBaseDir(const char *argv0)
{
    /* If there isn't a path on argv0, then look through the $PATH for it. */
    char *retval;
    char *envr;

    //const char *ptr = strrchr((char *)argv0, '/');
    if (strchr(argv0, '/')) {
        retval = strdup(argv0);
        if (retval)
            *((char *) strrchr(retval, '/')) = '\0';
        return(retval);
    }

    envr = getenv("PATH");
    if (!envr)
        return NULL;
    envr = strdup(envr);
    if (!envr)
        return NULL;
    retval = findBinaryInPath(argv0, envr);
    free(envr);
    return(retval);
}

static inline void chdirToAppPath(const char *argv0)
{
    char *dir = calcBaseDir(argv0);
    if (dir) {
#if (defined(__APPLE__) && defined(__MACH__))
        // Chop off /Contents/MacOS if it's at the end of the string, so we
        //  land in the base of the app bundle.
        const size_t len = strlen(dir);
        const char *bundledirs = "/Contents/MacOS";
        const size_t bundledirslen = strlen(bundledirs);
        if (len > bundledirslen) {
            char *ptr = (dir + len) - bundledirslen;
            if (strcasecmp(ptr, bundledirs) == 0)
                *ptr = '\0';
        }
#endif
        chdir(dir);
        free(dir);
    }
}
#endif


int main(int argc, char **argv)
{
#ifndef __MINGW32__
    _argc = argc;
    _argv = argv;
#endif

    // !!! FIXME: we could use a Win32 API for this.  --ryan.
#ifndef WIN32
    chdirToAppPath(argv[0]);
#endif

    LOGFUNC;

    try {
        {
            newGame();

            //ofstream os("error.txt");
            //os.close();
            //ofstream os("log.txt");
            //os.close();

            if (!SetUp ())
                return 42;

            while (!gDone && !tryquit) {
                if (IsFocused()) {
                    gameFocused = true;

                    // check windows messages

                    deltah = 0;
                    deltav = 0;
                    SDL_Event e;
                    if (!waiting) {
                        // message pump
                        while ( SDL_PollEvent( &e ) ) {
                            if ( e.type == SDL_QUIT ) {
                                gDone = true;
                                break;
                            }
                            sdlEventProc(e);
                        }
                    }

                    // game
                    DoUpdate();
                } else {
                    if (gameFocused) {
                        // allow game chance to pause
                        gameFocused = false;
                        DoUpdate();
                    }

                    // game is not in focus, give CPU time to other apps by waiting for messages instead of 'peeking'
                    SDL_Event evt;
                    SDL_WaitEvent(&evt);
                    if (evt.type == SDL_APP_DIDENTERFOREGROUND)
                        gameFocused = true;
                    else if (evt.type == SDL_QUIT)
                        gDone = true;
                }
            }

            deleteGame();
        }

        CleanUp ();

        return 0;
    } catch (const std::exception& error) {
        CleanUp();

        std::string e = "Caught exception: ";
        e += error.what();

        LOG(e);

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Uncaught Exception!", e.c_str(), NULL);
    }

    CleanUp();

    return -1;
}



// --------------------------------------------------------------------------


bool LoadImage(const char * fname, TGAImageRec & tex)
{
    if ( tex.data == NULL )
        return false;
    else
        return load_image(fname, tex);
}

void ScreenShot(const char * fname)
{
    save_image(fname);
}



static bool load_image(const char *file_name, TGAImageRec &tex)
{
    const char *ptr = strrchr((char *)file_name, '.');
    if (ptr) {
        if (strcasecmp(ptr+1, "png") == 0) {
            return load_png(file_name, tex);
        } else if (strcasecmp(ptr+1, "jpg") == 0) {
            return load_jpg(file_name, tex);
        } else if (strcasecmp(ptr+1, "tga") == 0) {
            TGAImageRec *tmpTex = LoadTGA(file_name);
            if (tmpTex) {
                tex = *tmpTex;
                return true;
            } else {
                return false;
            }
        }
    }

    STUBBED("Unsupported image type");
    return false;
}


struct my_error_mgr {
    struct jpeg_error_mgr pub; /* "public" fields */
    jmp_buf setjmp_buffer; /* for return to caller */
};
typedef struct my_error_mgr * my_error_ptr;


static void my_error_exit(j_common_ptr cinfo)
{
    struct my_error_mgr *err = (struct my_error_mgr *)cinfo->err;
    longjmp(err->setjmp_buffer, 1);
}

/* stolen from public domain example.c code in libjpg distribution. */
static bool load_jpg(const char *file_name, TGAImageRec &tex)
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPROW buffer[1]; /* Output row buffer */
    int row_stride; /* physical row width in output buffer */
    FILE *infile = fopen(file_name, "rb");

    if (infile == NULL)
        return false;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return false;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, TRUE);

    cinfo.out_color_space = JCS_RGB;
    cinfo.quantize_colors = 0;
    (void) jpeg_calc_output_dimensions(&cinfo);
    (void) jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;
    tex.sizeX = cinfo.output_width;
    tex.sizeY = cinfo.output_height;
    tex.bpp = 24;

    while (cinfo.output_scanline < cinfo.output_height) {
        buffer[0] = (JSAMPROW)(char *)tex.data +
                    ((cinfo.output_height - 1) - cinfo.output_scanline) * row_stride;
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    }

    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return true;
}


/* stolen from public domain example.c code in libpng distribution. */
static bool load_png(const char *file_name, TGAImageRec &tex)
{
    bool hasalpha = false;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    //png_byte **rows = NULL;
    bool retval = false;
    png_byte **row_pointers = NULL;
    FILE *fp = fopen(file_name, "rb");

    if (fp == NULL) {
        cerr << file_name << " not found" << endl;
        return(NULL);
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        goto png_done;

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
        goto png_done;

    if (setjmp(png_jmpbuf(png_ptr)))
        goto png_done;

    png_init_io(png_ptr, fp);
    png_read_png(png_ptr, info_ptr,
                 PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING,
                 NULL);
    png_get_IHDR(png_ptr, info_ptr, &width, &height,
                 &bit_depth, &color_type, &interlace_type, NULL, NULL);

    if (bit_depth != 8)  // transform SHOULD handle this...
        goto png_done;

    if (color_type & PNG_COLOR_MASK_PALETTE)  // !!! FIXME?
        goto png_done;

    if ((color_type & PNG_COLOR_MASK_COLOR) == 0)  // !!! FIXME?
        goto png_done;

    hasalpha = ((color_type & PNG_COLOR_MASK_ALPHA) != 0);
    row_pointers = png_get_rows(png_ptr, info_ptr);
    if (!row_pointers)
        goto png_done;

    if (!hasalpha) {
        png_byte *dst = tex.data;
        for (int i = height - 1; i >= 0; i--) {
            png_byte *src = row_pointers[i];
            for (int j = 0; j < width; j++) {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = 0xFF;
                src += 3;
                dst += 4;
            }
        }
    }

    else {
        png_byte *dst = tex.data;
        int pitch = width * 4;
        for (int i = height - 1; i >= 0; i--, dst += pitch)
            memcpy(dst, row_pointers[i], pitch);
    }

    tex.sizeX = width;
    tex.sizeY = height;
    tex.bpp = 32;
    retval = true;

png_done:
    if (!retval) {
        cerr << "There was a problem loading " << file_name << endl;
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    if (fp)
        fclose(fp);
    return (retval);
}


bool save_image(const char *file_name)
{
    const char *ptr = strrchr((char *)file_name, '.');
    if (ptr) {
        if (strcasecmp(ptr + 1, "png") == 0)
            return save_png(file_name);
    }

    STUBBED("Unsupported image type");
    return false;
}


static bool save_png(const char *file_name)
{
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    bool retval = false;

    fp = fopen(file_name, "wb");
    if (fp == NULL)
        return false;

    png_bytep *row_pointers = new png_bytep[kContextHeight];
    png_bytep screenshot = new png_byte[kContextWidth * kContextHeight * 3];
    if ((!screenshot) || (!row_pointers))
        goto save_png_done;

    glGetError();
    glReadPixels(0, 0, kContextWidth, kContextHeight,
                 GL_RGB, GL_UNSIGNED_BYTE, screenshot);
    if (glGetError() != GL_NO_ERROR)
        goto save_png_done;

    for (int i = 0; i < kContextHeight; i++)
        row_pointers[i] = screenshot + ((kContextWidth * ((kContextHeight - 1) - i)) * 3);

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        goto save_png_done;

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
        goto save_png_done;

    if (setjmp(png_jmpbuf(png_ptr)))
        goto save_png_done;

    png_init_io(png_ptr, fp);

    if (setjmp(png_jmpbuf(png_ptr)))
        goto save_png_done;

    png_set_IHDR(png_ptr, info_ptr, kContextWidth, kContextHeight,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr)))
        goto save_png_done;

    png_write_image(png_ptr, row_pointers);

    if (setjmp(png_jmpbuf(png_ptr)))
        goto save_png_done;

    png_write_end(png_ptr, NULL);
    retval = true;

save_png_done:
    png_destroy_write_struct(&png_ptr, &info_ptr);
    delete[] screenshot;
    delete[] row_pointers;
    if (fp)
        fclose(fp);
    if (!retval)
        unlink(ConvertFileName(file_name));
    return retval;
}
