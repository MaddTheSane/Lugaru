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

#if USE_OPENAL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

#include "Quaternions.h"
#include "openal_wrapper.h"
#include "Sounds.h"

// NOTE:
// FMOD uses a Left Handed Coordinate system, OpenAL uses a Right Handed
//  one...so we just need to flip the sign on the Z axis when appropriate.

#define DYNAMIC_LOAD_OPENAL 0

#if DYNAMIC_LOAD_OPENAL

#include <dlfcn.h>

#define AL_FUNC(t,ret,fn,params,call,rt) \
    extern "C" { \
        static ret ALAPIENTRY (*p##fn) params = NULL; \
        ret ALAPIENTRY fn params { rt p##fn call; } \
    }
#include "alstubs.h"
#undef AL_FUNC

static void *aldlhandle = NULL;

static bool lookup_alsym(const char *funcname, void **func, const char *libname)
{
    if (!aldlhandle)
        return false;

    *func = dlsym(aldlhandle, funcname);
    if (*func == NULL) {
        fprintf(stderr, "Failed to find OpenAL symbol \"%s\" in \"%s\"\n",
                funcname, libname);
        return false;
    }
    return true;
}

static void unload_alsyms(void)
{
#define AL_FUNC(t,ret,fn,params,call,rt) p##fn = NULL;
#include "alstubs.h"
#undef AL_FUNC
    if (aldlhandle) {
        dlclose(aldlhandle);
        aldlhandle = NULL;
    }
}

static bool lookup_all_alsyms(const char *libname)
{
    if (!aldlhandle) {
        if ( (aldlhandle = dlopen(libname, RTLD_GLOBAL | RTLD_NOW)) == NULL )
            return false;
    }

    bool retval = true;
#define AL_FUNC(t,ret,fn,params,call,rt) \
        if (!lookup_alsym(#fn, (void **) &p##fn, libname)) retval = false;
#include "alstubs.h"
#undef AL_FUNC

    if (!retval)
        unload_alsyms();

    return retval;
}
#else
#define lookup_all_alsyms(x) (true)
#define unload_alsyms()
#endif

using namespace std;

typedef struct {
    ALuint sid;
    OPENAL_SAMPLE *sample;
    bool startpaused;
    float position[3];
} OPENAL_Channels;

typedef struct OPENAL_SAMPLE {
    char *name;
    ALuint bid;  // buffer id.
    int mode;
    int is2d;
} OPENAL_SAMPLE;

static size_t num_channels = 0;
static OPENAL_Channels *impl_channels = NULL;
static bool initialized = false;
static float listener_position[3];

static void set_channel_position(const int channel, const float x,
                                 const float y, const float z)
{
    OPENAL_Channels *chan = &impl_channels[channel];

    chan->position[0] = x;
    chan->position[1] = y;
    chan->position[2] = z;

    OPENAL_SAMPLE *sptr = chan->sample;
    if (sptr == NULL)
        return;

    const ALuint sid = chan->sid;
    const bool no_attenuate = sptr->is2d;

    if (no_attenuate) {
        alSourcei(sid, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(sid, AL_POSITION, 0.0f, 0.0f, 0.0f);
    } else {
        alSourcei(sid, AL_SOURCE_RELATIVE, AL_FALSE);
        alSource3f(sid, AL_POSITION, x, y, z);
    }
}


AL_API void OPENAL_3D_Listener_SetAttributes(const float *pos, const float *vel, float fx, float fy, float fz, float tx, float ty, float tz)
{
    if (!initialized)
        return;
    if (pos != NULL) {
        alListener3f(AL_POSITION, pos[0], pos[1], -pos[2]);
        listener_position[0] = pos[0];
        listener_position[1] = pos[1];
        listener_position[2] = -pos[2];
    }

    ALfloat vec[6] = { fx, fy, -fz, tz, ty, -tz };
    alListenerfv(AL_ORIENTATION, vec);

    // we ignore velocity, since doppler's broken in the Linux AL at the moment...

    // adjust existing positions...
    for (int i = 0; i < num_channels; i++) {
        const float *p = impl_channels[i].position;
        set_channel_position(i, p[0], p[1], p[2]);
    }
}

AL_API bool OPENAL_3D_SetAttributes(int channel, const float *pos, const float *vel)
{
    if (!initialized)
        return false;
    if ((channel < 0) || (channel >= num_channels))
        return false;

    if (pos != NULL)
        set_channel_position(channel, pos[0], pos[1], -pos[2]);

    // we ignore velocity, since doppler's broken in the Linux AL at the moment...

    return true;
}

AL_API bool OPENAL_3D_SetAttributes_(int channel, const XYZ &pos, const float *vel)
{
    if (!initialized)
        return false;
    if ((channel < 0) || (channel >= num_channels))
        return false;

    set_channel_position(channel, pos.x, pos.y, -pos.z);

    return true;
}

AL_API bool OPENAL_Init(int mixrate, int maxsoftwarechannels, unsigned int flags)
{
    if (initialized)
        return false;
    if (maxsoftwarechannels == 0)
        return false;

    if (flags != 0)  // unsupported.
        return false;

    if (!lookup_all_alsyms("./openal.so")) { // !!! FIXME: linux specific lib name
        if (!lookup_all_alsyms("openal.so.1")) { // !!! FIXME: linux specific lib name
            if (!lookup_all_alsyms("openal.so"))  // !!! FIXME: linux specific lib name
                return false;
        }
    }

    ALCdevice *dev = alcOpenDevice(NULL);
    if (!dev)
        return false;

    ALint caps[] = { ALC_FREQUENCY, mixrate, 0 };
    ALCcontext *ctx = alcCreateContext(dev, caps);
    if (!ctx) {
        alcCloseDevice(dev);
        return false;
    }

    alcMakeContextCurrent(ctx);
    alcProcessContext(ctx);

    bool cmdline(const char * cmd);
    if (cmdline("openalinfo")) {
        printf("AL_VENDOR: %s\n", (char *) alGetString(AL_VENDOR));
        printf("AL_RENDERER: %s\n", (char *) alGetString(AL_RENDERER));
        printf("AL_VERSION: %s\n", (char *) alGetString(AL_VERSION));
        printf("AL_EXTENSIONS: %s\n", (char *) alGetString(AL_EXTENSIONS));
    }

    num_channels = maxsoftwarechannels;
    impl_channels = new OPENAL_Channels[maxsoftwarechannels];
    memset(impl_channels, '\0', sizeof (OPENAL_Channels) * num_channels);
    for (int i = 0; i < num_channels; i++)
        alGenSources(1, &impl_channels[i].sid);  // !!! FIXME: verify this didn't fail!

    initialized = true;
    return true;
}

AL_API void OPENAL_Close()
{
    if (!initialized)
        return;

    ALCcontext *ctx = alcGetCurrentContext();
    if (ctx) {
        for (int i = 0; i < num_channels; i++) {
            alSourceStop(impl_channels[i].sid);
            alSourcei(impl_channels[i].sid, AL_BUFFER, 0);
            alDeleteSources(1, &impl_channels[i].sid);
        }
        ALCdevice *dev = alcGetContextsDevice(ctx);
        alcMakeContextCurrent(NULL);
        alcSuspendContext(ctx);
        alcDestroyContext(ctx);
        alcCloseDevice(dev);
    }

    num_channels = 0;
    delete[] impl_channels;
    impl_channels = NULL;

    unload_alsyms();
    initialized = false;
}

static OPENAL_SAMPLE *OPENAL_GetCurrentSample(int channel)
{
    if (!initialized)
        return NULL;
    if ((channel < 0) || (channel >= num_channels))
        return NULL;
    return impl_channels[channel].sample;
}

static bool OPENAL_GetPaused(int channel)
{
    if (!initialized)
        return false;
    if ((channel < 0) || (channel >= num_channels))
        return false;
    if (impl_channels[channel].startpaused)
        return(true);

    ALint state = 0;
    alGetSourceiv(impl_channels[channel].sid, AL_SOURCE_STATE, &state);
    return((state == AL_PAUSED) ? true : false);
}

static unsigned int OPENAL_GetLoopMode(int channel)
{
    if (!initialized)
        return 0;
    if ((channel < 0) || (channel >= num_channels))
        return 0;
    ALint loop = 0;
    alGetSourceiv(impl_channels[channel].sid, AL_LOOPING, &loop);
    if (loop)
        return(OPENAL_LOOP_NORMAL);
    return OPENAL_LOOP_OFF;
}

static bool OPENAL_IsPlaying(int channel)
{
    if (!initialized)
        return false;
    if ((channel < 0) || (channel >= num_channels))
        return false;
    ALint state = 0;
    alGetSourceiv(impl_channels[channel].sid, AL_SOURCE_STATE, &state);
    return((state == AL_PLAYING) ? true : false);
}

static int OPENAL_PlaySoundEx(int channel, OPENAL_SAMPLE *sptr, OPENAL_DSPUNIT *dsp, bool startpaused)
{
    if (!initialized)
        return -1;
    if (sptr == NULL)
        return -1;
    if (dsp != NULL)
        return -1;
    if (channel == OPENAL_FREE) {
        for (int i = 0; i < num_channels; i++) {
            ALint state = 0;
            alGetSourceiv(impl_channels[i].sid, AL_SOURCE_STATE, &state);
            if ((state != AL_PLAYING) && (state != AL_PAUSED)) {
                channel = i;
                break;
            }
        }
    }

    if ((channel < 0) || (channel >= num_channels))
        return -1;
    alSourceStop(impl_channels[channel].sid);
    impl_channels[channel].sample = sptr;
    alSourcei(impl_channels[channel].sid, AL_BUFFER, sptr->bid);
    alSourcei(impl_channels[channel].sid, AL_LOOPING, (sptr->mode == OPENAL_LOOP_OFF) ? AL_FALSE : AL_TRUE);
    set_channel_position(channel, 0.0f, 0.0f, 0.0f);

    impl_channels[channel].startpaused = ((startpaused) ? true : false);
    if (!startpaused)
        alSourcePlay(impl_channels[channel].sid);
    return channel;
}


static void *decode_to_pcm(const char *_fname, ALenum &format, ALsizei &size, ALuint &freq)
{
#ifdef __POWERPC__
    const int bigendian = 1;
#else
    const int bigendian = 0;
#endif

    // !!! FIXME: if it's not Ogg, we don't have a decoder. I'm lazy.  :/
    char *fname = (char *) alloca(strlen(_fname) + 16);
    strcpy(fname, _fname);
    char *ptr = strchr(fname, '.');
    if (ptr)
        *ptr = '\0';
    strcat(fname, ".ogg");

    // just in case...
#undef fopen
    FILE *io = fopen(fname, "rb");
    if (io == NULL)
        return NULL;

    ALubyte *retval = NULL;

#if 0  // untested, so disable this!
    // Can we just feed it to the AL compressed?
    if (alIsExtensionPresent((const ALubyte *) "AL_EXT_vorbis")) {
        format = alGetEnumValue((const ALubyte *) "AL_FORMAT_VORBIS_EXT");
        freq = 44100;
        fseek(io, 0, SEEK_END);
        size = ftell(io);
        fseek(io, 0, SEEK_SET);
        retval = (ALubyte *) malloc(size);
        size_t rc = fread(retval, size, 1, io);
        fclose(io);
        if (rc != 1) {
            free(retval);
            return NULL;
        }
        return retval;
    }
#endif

    // Uncompress and feed to the AL.
    OggVorbis_File vf;
    memset(&vf, '\0', sizeof (vf));
    if (ov_open(io, &vf, NULL, 0) == 0) {
        int bitstream = 0;
        vorbis_info *info = ov_info(&vf, -1);
        size = 0;
        format = (info->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        freq = (ALuint)info->rate;

        if ((info->channels != 1) && (info->channels != 2)) {
            ov_clear(&vf);
            return NULL;
        }

        char buf[1024 * 16];
        long rc = 0;
        size_t allocated = 64 * 1024;
        retval = (ALubyte *) malloc(allocated);
        while ( (rc = ov_read(&vf, buf, sizeof (buf), bigendian, 2, 1, &bitstream)) != 0 ) {
            if (rc > 0) {
                size += rc;
                if (size >= allocated) {
                    allocated *= 2;
                    ALubyte *tmp = (ALubyte *) realloc(retval, allocated);
                    if (tmp == NULL) {
                        free(retval);
                        retval = NULL;
                        break;
                    }
                    retval = tmp;
                }
                memcpy(retval + (size - rc), buf, rc);
            }
        }
        ov_clear(&vf);
        return retval;
    }

    fclose(io);
    return NULL;
}


AL_API OPENAL_SAMPLE *OPENAL_Sample_Load(int index, const char *name_or_data, unsigned int mode, int offset, int length)
{
    if (!initialized)
        return NULL;
    if (index != OPENAL_FREE)
        return NULL;  // this is all the game does...
    if (offset != 0)
        return NULL;  // this is all the game does...
    if (length != 0)
        return NULL;  // this is all the game does...
    if ((mode != OPENAL_HW3D) && (mode != OPENAL_2D))
        return NULL;  // this is all the game does...

    OPENAL_SAMPLE *retval = NULL;
    ALenum format = AL_NONE;
    ALsizei size = 0;
    ALuint frequency = 0;
    void *data = decode_to_pcm(name_or_data, format, size, frequency);
    if (data == NULL)
        return NULL;

    ALuint bid = 0;
    alGetError();
    alGenBuffers(1, &bid);
    if (alGetError() == AL_NO_ERROR) {
        alBufferData(bid, format, data, size, frequency);
        retval = new OPENAL_SAMPLE;
        retval->bid = bid;
        retval->mode = OPENAL_LOOP_OFF;
        retval->is2d = (mode == OPENAL_2D);
        retval->name = new char[strlen(name_or_data) + 1];
        if (retval->name)
            strcpy(retval->name, name_or_data);
    }

    free(data);
    return(retval);
}

AL_API void OPENAL_Sample_Free(OPENAL_SAMPLE *sptr)
{
    if (!initialized)
        return;
    if (sptr) {
        for (int i = 0; i < num_channels; i++) {
            if (impl_channels[i].sample == sptr) {
                alSourceStop(impl_channels[i].sid);
                alSourcei(impl_channels[i].sid, AL_BUFFER, 0);
                impl_channels[i].sample = NULL;
            }
        }
        alDeleteBuffers(1, &sptr->bid);
        delete[] sptr->name;
        delete sptr;
    }
}

static bool OPENAL_Sample_SetMode(OPENAL_SAMPLE *sptr, unsigned int mode)
{
    if (!initialized)
        return false;
    if ((mode != OPENAL_LOOP_NORMAL) && (mode != OPENAL_LOOP_OFF))
        return false;
    if (!sptr)
        return false;
    sptr->mode = mode;
    return true;
}

AL_API bool OPENAL_SetFrequency(int channel, int freq)
{
    if (!initialized)
        return false;
    if (channel == OPENAL_ALL) {
        for (int i = 0; i < num_channels; i++)
            OPENAL_SetFrequency(i, freq);
        return true;
    }

    if ((channel < 0) || (channel >= num_channels))
        return false;
    if (freq == 8012)
        // hack
        alSourcef(impl_channels[channel].sid, AL_PITCH, 8012.0f / 44100.0f);
    else
        alSourcef(impl_channels[channel].sid, AL_PITCH, 1.0f);
    return true;
}

AL_API bool OPENAL_SetVolume(int channel, int vol)
{
    if (!initialized)
        return false;

    if (channel == OPENAL_ALL) {
        for (int i = 0; i < num_channels; i++)
            OPENAL_SetVolume(i, vol);
        return true;
    }

    if ((channel < 0) || (channel >= num_channels))
        return false;

    if (vol < 0)
        vol = 0;
    else if (vol > 255)
        vol = 255;
    ALfloat gain = ((ALfloat) vol) / 255.0f;
    alSourcef(impl_channels[channel].sid, AL_GAIN, gain);
    return true;
}

AL_API bool OPENAL_SetPaused(int channel, bool paused)
{
    if (!initialized)
        return false;

    if (channel == OPENAL_ALL) {
        for (int i = 0; i < num_channels; i++)
            OPENAL_SetPaused(i, paused);
        return true;
    }

    if ((channel < 0) || (channel >= num_channels))
        return false;

    ALint state = 0;
    if (impl_channels[channel].startpaused)
        state = AL_PAUSED;
    else
        alGetSourceiv(impl_channels[channel].sid, AL_SOURCE_STATE, &state);

    if ((paused) && (state == AL_PLAYING))
        alSourcePause(impl_channels[channel].sid);
    else if ((!paused) && (state == AL_PAUSED)) {
        alSourcePlay(impl_channels[channel].sid);
        impl_channels[channel].startpaused = false;
    }
    return true;
}

AL_API void OPENAL_SetSFXMasterVolume(int volume)
{
    if (!initialized)
        return;
    ALfloat gain = ((ALfloat) volume) / 255.0f;
    alListenerf(AL_GAIN, gain);
}

AL_API bool OPENAL_StopSound(int channel)
{
    if (!initialized)
        return false;

    if (channel == OPENAL_ALL) {
        for (int i = 0; i < num_channels; i++)
            OPENAL_StopSound(i);
        return true;
    }

    if ((channel < 0) || (channel >= num_channels))
        return false;
    alSourceStop(impl_channels[channel].sid);
    impl_channels[channel].startpaused = false;
    return true;
}

AL_API void OPENAL_Stream_Close(OPENAL_STREAM *stream)
{
    OPENAL_Sample_Free((OPENAL_SAMPLE *) stream);
}

static OPENAL_SAMPLE *OPENAL_Stream_GetSample(OPENAL_STREAM *stream)
{
    if (!initialized)
        return NULL;
    return (OPENAL_SAMPLE *) stream;
}

static int OPENAL_Stream_PlayEx(int channel, OPENAL_STREAM *stream, OPENAL_DSPUNIT *dsp, bool startpaused)
{
    return OPENAL_PlaySoundEx(channel, (OPENAL_SAMPLE *) stream, dsp, startpaused);
}

static bool OPENAL_Stream_Stop(OPENAL_STREAM *stream)
{
    if (!initialized)
        return false;
    for (int i = 0; i < num_channels; i++) {
        if (impl_channels[i].sample == (OPENAL_SAMPLE *) stream) {
            alSourceStop(impl_channels[i].sid);
            impl_channels[i].startpaused = false;
        }
    }
    return true;
}

AL_API bool OPENAL_Stream_SetMode(OPENAL_STREAM *stream, unsigned int mode)
{
    return OPENAL_Sample_SetMode((OPENAL_SAMPLE *) stream, mode);
}

AL_API void OPENAL_Update()
{
    if (!initialized)
        return;
    alcProcessContext(alcGetCurrentContext());
}

AL_API bool OPENAL_SetOutput(int outputtype)
{
    return true;
}

extern int channels[];

extern "C" void PlaySoundEx(int chan, OPENAL_SAMPLE *sptr, OPENAL_DSPUNIT *dsp, bool startpaused)
{
    const OPENAL_SAMPLE * currSample = OPENAL_GetCurrentSample(channels[chan]);
    if (currSample && currSample == samp[chan]) {
        if (OPENAL_GetPaused(channels[chan])) {
            OPENAL_StopSound(channels[chan]);
            channels[chan] = OPENAL_FREE;
        } else if (OPENAL_IsPlaying(channels[chan])) {
            int loop_mode = OPENAL_GetLoopMode(channels[chan]);
            if (loop_mode & OPENAL_LOOP_OFF) {
                channels[chan] = OPENAL_FREE;
            }
        }
    } else {
        channels[chan] = OPENAL_FREE;
    }

    channels[chan] = OPENAL_PlaySoundEx(channels[chan], sptr, dsp, startpaused);
    if (channels[chan] < 0) {
        channels[chan] = OPENAL_PlaySoundEx(OPENAL_FREE, sptr, dsp, startpaused);
    }
}

extern "C" void PlayStreamEx(int chan, OPENAL_STREAM *sptr, OPENAL_DSPUNIT *dsp, bool startpaused)
{
    const OPENAL_SAMPLE * currSample = OPENAL_GetCurrentSample(channels[chan]);
    if (currSample && currSample == OPENAL_Stream_GetSample(sptr)) {
        OPENAL_StopSound(channels[chan]);
        OPENAL_Stream_Stop(sptr);
    } else {
        OPENAL_Stream_Stop(sptr);
        channels[chan] = OPENAL_FREE;
    }

    channels[chan] = OPENAL_Stream_PlayEx(channels[chan], sptr, dsp, startpaused);
    if (channels[chan] < 0) {
        channels[chan] = OPENAL_Stream_PlayEx(OPENAL_FREE, sptr, dsp, startpaused);
    }
}

#endif

