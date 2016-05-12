/*
 Copyright (C) 2016 - Wolfire Games
 
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

#ifndef Globals_h
#define Globals_h

#include "SDL.h"

#include "TGALoader.h"
#include "openal_wrapper.h"
#include "Quaternions.h"
#include "Game.h"

class Terrain;
class FRUSTUM;
class Light;
class Weapons;
class Light;
class Sprites;
class Animation;
class Person;
class Skeleton;
class Objects;

extern bool visibleloading;
extern OPENAL_SAMPLE *samp[71];
extern OPENAL_STREAM *strm[20];
extern int channels[100];

extern XYZ viewer;
extern int environment;
extern float texscale;
extern Light light;
extern Terrain terrain;
extern Sprites sprites;
extern float multiplier;
extern float sps;
extern float viewdistance;
extern float fadestart;
extern float screenwidth,screenheight;
extern float windowWidth,windowHeight;
extern int kTextureSize;
extern FRUSTUM frustum;
extern Objects objects;
extern int detail;
extern float usermousesensitivity;
extern bool osx;
extern float camerashake;
extern Weapons weapons;
extern Person player[maxplayers];
extern int slomo;
extern float slomodelay;
extern bool ismotionblur;
extern float woozy;
extern float blackout;
extern bool damageeffects;
extern float volume;
extern int numplayers;
extern bool texttoggle;
extern float blurness;
extern float targetblurness;
extern float playerdist;
extern bool cellophane;
extern bool freeze;
extern float flashamount,flashr,flashg,flashb;
extern int flashdelay;
extern int netstate;
extern float motionbluramount;
extern bool isclient;
extern bool alwaysblur;
extern int test;
extern bool tilt2weird;
extern bool tiltweird;
extern bool midweird;
extern bool proportionweird;
extern bool vertexweird[6];
extern bool velocityblur;
extern bool buttons[3];
extern bool debugmode;
extern int mainmenu;
extern int oldmainmenu;
extern int bloodtoggle;
extern int difficulty;
extern bool decals;
// MODIFIED GWC
//extern int texdetail;
extern float texdetail;
extern bool musictoggle;
extern int bonus;
extern float bonusvalue;
extern float bonustotal;
extern float bonustime;
extern int oldbonus;
extern float startbonustotal;
extern float bonusnum[100];
extern int tutoriallevel;
extern float smoketex;
extern float tutorialstagetime;
extern float tutorialmaxtime;
extern int tutorialstage;
extern bool againbonus;
extern float damagedealt;
extern float damagetaken;
extern bool invertmouse;

extern int numhotspots;
extern bool winhotspot;
extern int killhotspot;
extern XYZ hotspot[40];
extern int hotspottype[40];
extern float hotspotsize[40];
extern char hotspottext[40][256];
extern int currenthotspot;


extern int numfalls;
extern int numflipfail;
extern int numseen;
extern int numstaffattack;
extern int numswordattack;
extern int numknifeattack;
extern int numunarmedattack;
extern int numescaped;
extern int numflipped;
extern int numwallflipped;
extern int numthrowkill;
extern int numafterkill;
extern int numreversals;
extern int numattacks;
extern int maxalarmed;
extern int numresponded;

extern bool campaign;
extern bool winfreeze;

extern float menupulse;

extern bool gamestart;

extern XYZ participantlocation[max_dialogues][10];
extern int participantfocus[max_dialogues][max_dialoguelength];
extern int participantaction[max_dialogues][max_dialoguelength];
extern float participantrotation[max_dialogues][10];
extern XYZ participantfacing[max_dialogues][max_dialoguelength][10];
extern float dialoguecamerarotation[max_dialogues][max_dialoguelength];
extern float dialoguecamerarotation2[max_dialogues][max_dialoguelength];
extern int indialogue;
extern int whichdialogue;
extern int directing;
extern float dialoguetime;
extern int dialoguegonethrough[20];


extern bool gamestarted;

#pragma mark screen

extern XYZ lightlocation;
extern float lightambient[3],lightbrightness[3];
extern float gravity;
extern Animation animation[animation_count];
extern Skeleton testskeleton;
extern int numsounds;
extern float realtexdetail;
extern float terraindetail;
extern GLubyte bloodText[512*512*3];
extern GLubyte wolfbloodText[512*512*3];
extern bool trilinear;
extern bool ambientsound;
extern int netdatanew;
extern float mapinfo;
extern bool stillloading;
extern TGAImageRec texture;
extern short vRefNum;
extern long dirID;
extern int loadscreencolor;
extern int whichjointstartarray[26];
extern int whichjointendarray[26];
extern float tintr,tintg,tintb;
extern float slomospeed;
extern char mapname[256];

#pragma mark dialog
extern int numdialogues;
extern int numdialogueboxes[20];
extern int dialoguetype[20];
extern int dialogueboxlocation[20][20];
extern float dialogueboxcolor[20][20][3];
extern int dialogueboxsound[20][20];
extern char dialoguetext[20][20][128];
extern char dialoguename[20][20][64];
extern XYZ dialoguecamera[20][20];
extern float dialoguecamerarotation[20][20];
extern float dialoguecamerarotation2[20][20];
extern int indialogue;
extern int whichdialogue;
extern float dialoguetime;

#pragma mark -

//class Game;

//extern Game * pgame;
extern bool skyboxtexture;

extern bool LoadImage(const char * fname, TGAImageRec & tex);


#pragma mark -

extern bool floatjump;
extern float windvar;
extern float precipdelay;
extern XYZ viewerfacing;
extern bool mousejump;
extern bool autoslomo;
extern bool keyboardfrozen;
extern bool loadingstuff;
extern XYZ windvector;
extern int music1;
extern XYZ envsound[30];
extern float envsoundvol[30];
extern int numenvsounds;
extern float envsoundlife[30];
extern bool foliage;
extern bool showpoints;
extern float gamespeed;
extern bool vblsync;
extern bool immediate;
extern float skyboxr;
extern float skyboxg;
extern float skyboxb;
extern float skyboxlightr;
extern float skyboxlightg;
extern float skyboxlightb;
extern float slomofreq;
extern float tutorialsuccess;
extern bool reversaltrain;
extern bool canattack;
extern bool cananger;
extern int maptype;
extern editortypes editoractive;
extern int editorpathtype;
extern bool oldbuttons[3];

extern float hostiletime;

extern bool windialogue;

extern int kBitsPerPixel;
extern int hostile;


extern float oldgamespeed;

extern bool showdamagebar;


#pragma mark -

extern float realmultiplier;
extern GLubyte texturearray[512*512*3];
typedef struct SDL_Window SDL_Window;
extern SDL_Window *sdlwindow;


#endif /* Globals_h */
