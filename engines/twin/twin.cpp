/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/error.h"

#include "engines/engine.h"
#include "graphics/pixelbuffer.h"

#include "engines/twin/twin.h"
#include "engines/twin/hqr.h"
#include "engines/twin/image.h"
#include "engines/twin/scene.h"



namespace Twin {

TwinEngine *g_twin = NULL;

TwinEngine::TwinEngine(OSystem *syst, TwinGameType type) :
		Engine(syst), _type(type) {
	g_twin = this;
}

TwinEngine::~TwinEngine() {
	g_twin = NULL;
}

void TwinEngine::intro() {
	// TODO: Should fade Black to White.
	// TODO: Additional palette-work (for fading).
	Image *adelineLogo = NULL;
	if (getGameType() == GType_LBA) {
		adelineLogo = new Image("RESS.HQR", 27, _renderer);
	} else if (getGameType() == GType_LBA2) {
		adelineLogo = new Image("SCREEN.HQR", 0, _renderer);
	}
	_renderer->clearScreen();
	adelineLogo->display();
	_renderer->flipBuffer();

	// Just delay a bit for now, so we can see the results.
	g_system->delayMillis(2000);
	delete adelineLogo;
}

void TwinEngine::createRenderer() {
	_renderer = new GfxOpenGL();
}

Common::Error TwinEngine::run() {
	createRenderer();
	_renderer->setupScreen(640, 480, false);
	intro();

	Hqr scene;
	scene.open("SCENE.HQR");
	Common::SeekableReadStream *stream = scene.createReadStreamForIndex(1);
	Scene s(stream);

	return Common::kNoError;
}

} // end of namespace Twin
