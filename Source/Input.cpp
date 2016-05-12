/*
Copyright (C) 2003, 2010 - Wolfire Games
Copyright (C) 2010 - Côme <MCMic> BERNIGAUD

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

/**> HEADER FILES <**/
#include "Input.h"

extern bool keyboardfrozen;

bool keyDown[SDL_NUM_SCANCODES + 6];
bool keyPressed[SDL_NUM_SCANCODES + 6];

void Input::Tick()
{
    SDL_PumpEvents();
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        keyPressed[i] = !keyDown[i] && keyState[i];
        keyDown[i] = keyState[i];
    }
    Uint8 mb = SDL_GetMouseState(NULL, NULL);
    for (int i = 1; i < 6; i++) {
        keyPressed[SDL_NUM_SCANCODES + i] = !keyDown[SDL_NUM_SCANCODES + i] && (mb & SDL_BUTTON(i));
        keyDown[SDL_NUM_SCANCODES + i] = (mb & SDL_BUTTON(i));
    }
}

bool Input::isKeyDown(SDL_Keycode k)
{
    if (keyboardfrozen) // really useful? check that.
        return false;
    else if (k > SDL_BUTTON_X2)
        return keyDown[SDL_GetScancodeFromKey(k)];

    return keyDown[SDL_NUM_SCANCODES+k];
}

bool Input::isKeyPressed(SDL_Keycode k)
{
    if (keyboardfrozen)
        return false;
    else if (k > SDL_BUTTON_X2)
        return keyPressed[SDL_GetScancodeFromKey(k)];

    return keyPressed[SDL_NUM_SCANCODES+k];
}

const char* Input::keyToChar(SDL_Keycode i)
{
    if (i > SDL_BUTTON_X2) {
        return SDL_GetKeyName(i);
    }
    else if (i == SDL_BUTTON_LEFT)
        return "mouse1";
    else if (i == SDL_BUTTON_RIGHT)
        return "mouse2";
    else if (i == SDL_BUTTON_MIDDLE)
        return "mouse3";
    else
        return "unknown";
}

SDL_Keycode Input::CharToKey(const char* which)
{
    SDL_Keycode ourCode = SDL_GetKeyFromName(which);
    if (ourCode != SDLK_UNKNOWN) {
        return ourCode;
    }
    if (!strcasecmp(which, "mouse1")) {
        return MOUSEBUTTON1;
    }
    if (!strcasecmp(which, "mouse2")) {
        return MOUSEBUTTON2;
    }
    if (!strcasecmp(which, "mouse3")) {
        return MOUSEBUTTON3;
    }
    return SDLK_UNKNOWN;
}

Boolean Input::MouseClicked()
{
    return isKeyPressed(SDL_BUTTON_LEFT);
}
