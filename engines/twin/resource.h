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

#ifndef TWIN_RESOURCE_H
#define TWIN_RESOURCE_H

#include "common/hashmap.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Block;
class Hqr;
class BlockLibrary;
class Grid;
class EntityInformation;
class Model;
class Animation;

class Entity {
public:
	Entity(Model *m, Animation *a) : _model(m), _anim(a) { }
	~Entity();
	void update(uint32 time);
	Model *_model;
	Animation *_anim;
};

class Resource {
public:
	Resource();
	~Resource();

	Grid *getGrid(int id);
	Block *getBlock(int id);
	BlockLibrary *getBlockLibrary(int id);
	Entity *getEntity(uint16 entity, uint16 body, uint16 anim);
private:
	void loadGridDefaults();

	uint16 _firstGrid;
	uint16 _firstGridFragment;
	uint16 _firstLibrary;
	uint16 _firstBlock;

	Hqr *_bkg;
	Hqr *_ress;
	Hqr *_body;
	Hqr *_anim;

	EntityInformation *_ei;

	Common::HashMap<int, Block *> _blocks;
	Common::HashMap<int, BlockLibrary *> _blockLibraries;
};

extern Resource *g_resource;

} // end of namespace Twin

#endif
