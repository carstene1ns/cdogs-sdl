/*
	C-Dogs SDL
	A port of the legendary (and fun) action/arcade cdogs.

	Copyright (c) 2013, Cong Xu
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
#include "menu_utils.h"

#include <assert.h>
#include <ctype.h>

#include <cdogs/draw.h>
#include <cdogs/font.h>


// Display a character and the player name above it, with the character
// centered around the target position
void DisplayCharacterAndName(Vec2i pos, const Character *c, const char *name)
{
	// Move the point down a bit since the default character draw point is at
	// its feet
	pos.y += 8;
	Vec2i namePos = Vec2iAdd(pos, Vec2iNew(-FontStrW(name) / 2, -30));
	DrawCharacterSimple(
		c, pos,
		DIRECTION_DOWN, STATE_IDLE, -1, GUNSTATE_READY, &c->table);
	FontStr(name, namePos);
}

void MenuDisplayPlayer(
	const menu_t *menu, GraphicsDevice *g,
	const Vec2i pos, const Vec2i size, const void *data)
{
	UNUSED(g);
	const MenuDisplayPlayerData *d = data;
	Vec2i playerPos;
	char s[22];
	UNUSED(menu);
	Vec2i dPos = pos;
	dPos.x -= size.x;	// move to left half of screen
	playerPos = Vec2iNew(
		dPos.x + size.x * 3 / 4 - 12 / 2, CENTER_Y(dPos, size, 0));

	const PlayerData *pData = CArrayGet(&gPlayerDatas, d->playerIndex);
	if (d->currentMenu && strcmp((*d->currentMenu)->name, "Name") == 0)
	{
		sprintf(s, "%c%s%c", '>', pData->name, '<');
	}
	else
	{
		strcpy(s, pData->name);
	}

	DisplayCharacterAndName(playerPos, &pData->Char, s);
}

void MenuDisplayPlayerControls(
	const menu_t *menu, GraphicsDevice *g,
	const Vec2i pos, const Vec2i size, const void *data)
{
	UNUSED(g);
	char s[256];
	const int *playerIndex = data;
	Vec2i textPos = Vec2iNew(0, pos.y + size.y - FontH());
	int textWidth = 0;

	UNUSED(menu);

	const PlayerData *pData = CArrayGet(&gPlayerDatas, *playerIndex);
	switch (pData->inputDevice)
	{
	case INPUT_DEVICE_KEYBOARD:
		{
			const input_keys_t *keys =
				&gEventHandlers.keyboard.PlayerKeys[*playerIndex];
			sprintf(s, "(%s, %s, %s, %s, %s and %s)",
				SDL_GetKeyName(keys->left),
				SDL_GetKeyName(keys->right),
				SDL_GetKeyName(keys->up),
				SDL_GetKeyName(keys->down),
				SDL_GetKeyName(keys->button1),
				SDL_GetKeyName(keys->button2));
		}
		break;
	case INPUT_DEVICE_MOUSE:
		sprintf(s, "(mouse wheel to scroll, left and right click)");
		break;
	case INPUT_DEVICE_JOYSTICK:
		sprintf(s, "(%s)",
			InputDeviceName(pData->inputDevice, pData->deviceIndex));
		break;
	case INPUT_DEVICE_AI:
		sprintf(s, "(%s)",
			InputDeviceName(pData->inputDevice, pData->deviceIndex));
		break;
	default:
		assert(0 && "unknown device");
		break;
	}

	// If the text is too long, split the text with a newline
	textWidth = FontStrW(s);
	if (textWidth < 125)
	{
		textPos.x = pos.x - textWidth / 2;
		FontStr(s, textPos);
	}
	else
	{
		// find the first whitespace before half of the string, split there,
		// and print two lines
		char *secondLine;
		for (secondLine = &s[strlen(s) / 2]; secondLine > s; secondLine--)
		{
			if (isspace(*secondLine))
			{
				*secondLine = '\0';
				secondLine++;
				break;
			}
		}
		textWidth = FontStrW(s);
		textPos.x = pos.x - textWidth / 2;
		textPos.y -= FontH();
		FontStr(s, textPos);
		textWidth = FontStrW(secondLine);
		textPos.x = pos.x - textWidth / 2;
		textPos.y += FontH();
		FontStr(secondLine, textPos);
	}
}
