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

#ifndef TWIN_H
#define TWIN_H

#include "engines/engine.h"

#include "common/system.h"
#include "common/events.h"

#include "engines/twin/gfx_base.h"

namespace Graphics {
struct Surface;
}

namespace Twin {

class Scene;
class Player;

enum TwinGameType {
	GType_LBA,
	GType_LBA2
};

enum Keys {
	KeyUp = 0,
	KeyDown,
	KeyLeft,
	KeyRight,
	KeyAction,
	NumKeys,
};

class TwinEngine : public Engine {
	GfxBase *_renderer;
	TwinGameType _type;
	Scene *_scene;
	Scene *_nextScene;
	bool _action;
	Player *_player;
protected:
	// Engine APIs
	virtual Common::Error run();
	void createRenderer();
	void playMovie(const Common::String &name);
	void intro();
	void processKeyUp(Common::KeyCode key);
	void processKeyDown(Common::KeyCode key);
	bool _keys[NumKeys];
public:
	TwinEngine(OSystem *syst, TwinGameType type);
	virtual ~TwinEngine();
	Scene *getCurrentScene() { return _scene; }
	TwinGameType getGameType() { return _type; }
	bool isPressingAction() { return _action; }
	void changeScene(uint16 id, uint16 heroX, uint16 heroY, uint16 heroZ);

	Player *getPlayer() { return _player; }

	bool getKey(byte id) { return _keys[id]; }
};

extern TwinEngine *g_twin;

} // end of namespace Twin

#endif
