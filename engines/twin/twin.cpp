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
#include "engines/twin/scene.h"



namespace Twin {


TwinEngine::TwinEngine(OSystem *syst) :
		Engine(syst) {
}

TwinEngine::~TwinEngine() {

}

void TwinEngine::createRenderer() {
	_renderer = new GfxOpenGL();
}

Common::Error TwinEngine::run() {
	createRenderer();
	_renderer->setupScreen(640, 480, false);
	Hqr scene;
	scene.open("SCENE.HQR");
	Common::SeekableReadStream *stream = scene.createReadStreamForIndex(1);
	Scene s(stream);

	return Common::kNoError;
}

} // end of namespace Twin
