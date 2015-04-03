/*
    C-Dogs SDL
    A port of the legendary (and fun) action/arcade cdogs.

    Copyright (c) 2014, Cong Xu
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
#include "game_loop.h"

#include <SDL_timer.h>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#include "config.h"
#include "events.h"
#include "music.h"
#include "net_client.h"
#include "net_server.h"
#include "sounds.h"


GameLoopData GameLoopDataNew(
	void *updateData, GameLoopResult (*updateFunc)(void *),
	void *drawData, void (*drawFunc)(void *))
{
	GameLoopData g;
	memset(&g, 0, sizeof g);
	g.UpdateData = updateData;
	g.UpdateFunc = updateFunc;
	g.DrawData = drawData;
	g.DrawFunc = drawFunc;
	g.FPS = 30;
	return g;
}

void EmscriptenMainLoop(GameLoopData *data)
{
	Uint32 ticksNow = SDL_GetTicks();
	GameLoopResult result = UPDATE_RESULT_OK;

	// Input
	if ((data->Frames & 1) || !data->InputEverySecondFrame)
	{
		EventPoll(&gEventHandlers, ticksNow);
		if (data->InputFunc)
		{
			data->InputFunc(data->InputData);
		}
	}

	NetClientPoll(&gNetClient);
	NetServerPoll(&gNetServer);

	// Update
	data->Frames++;
	result = data->UpdateFunc(data->UpdateData);
	if (data->DrawFunc)
	{
		data->DrawFunc(data->DrawData);
	}
	BlitFlip(&gGraphicsDevice);
	data->HasDrawnFirst = true;

	if (result == UPDATE_RESULT_EXIT)
	{
		emscripten_cancel_main_loop();
		printf("left main loop");
	}
}

void GameLoop(GameLoopData *data)
{
	EventReset(&gEventHandlers, gEventHandlers.mouse.cursor);

#ifndef __EMSCRIPTEN__
	GameLoopResult result = UPDATE_RESULT_OK;
	Uint32 ticksNow = SDL_GetTicks();
	Uint32 ticksElapsed = 0;
	int framesSkipped = 0;
	const int maxFrameskip = data->FPS / 5;
	for (; result != UPDATE_RESULT_EXIT; )
	{
		// Frame rate control
		const Uint32 ticksThen = ticksNow;
		ticksNow = SDL_GetTicks();
		ticksElapsed += ticksNow - ticksThen;
		if ((int)ticksElapsed < 1000 / data->FPS)
		{
			SDL_Delay(1);
			//debug(D_MAX, "Delaying 1 ticksNow %u elapsed %u\n", ticksNow, ticksElapsed);
			continue;
		}

#ifndef RUN_WITHOUT_APP_FOCUS
		MusicSetPlaying(&gSoundDevice, SDL_GetAppState() & SDL_APPINPUTFOCUS);
#endif

		// Input
		if ((data->Frames & 1) || !data->InputEverySecondFrame)
		{
			EventPoll(&gEventHandlers, ticksNow);
			if (data->InputFunc)
			{
				data->InputFunc(data->InputData);
			}
		}

		NetClientPoll(&gNetClient);
		NetServerPoll(&gNetServer);

		// Update
		result = data->UpdateFunc(data->UpdateData);
		bool draw = !data->HasDrawnFirst;
		switch (result)
		{
		case UPDATE_RESULT_OK:
			// Do nothing
			break;
		case UPDATE_RESULT_DRAW:
			draw = true;
			break;
		case UPDATE_RESULT_EXIT:
			// Will exit
			break;
		default:
			CASSERT(false, "Unknown loop result");
			break;
		}
		ticksElapsed -= 1000 / data->FPS;
		data->Frames++;
		// frame skip
		if ((int)ticksElapsed > 1000 / data->FPS && framesSkipped < maxFrameskip)
		{
			framesSkipped++;
			continue;
		}
		framesSkipped = 0;

		// Draw
		if (draw)
		{
			if (data->DrawFunc)
			{
				data->DrawFunc(data->DrawData);
			}
			BlitFlip(&gGraphicsDevice);
			data->HasDrawnFirst = true;
		}
	}
#else
    emscripten_set_main_loop_arg((em_arg_callback_func)EmscriptenMainLoop, data, 0, 1);
#endif
}
