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

#include "common/stream.h"

#include "engines/twin/resource.h"
#include "engines/twin/twin.h"
#include "engines/twin/hqr.h"
#include "engines/twin/block.h"
#include "engines/twin/block_library.h"
#include "engines/twin/grid.h"
#include "engines/twin/entity_information.h"
#include "engines/twin/animation.h"
#include "engines/twin/model.h"
#include "engines/twin/scene.h"

namespace Twin {

Entity::~Entity() {
	delete _anim;
	delete _model;
}
void Entity::update(uint32 time) {
	_anim->update(time);
}

Resource *g_resource = nullptr;

Resource::Resource() {
	_bkg = new Hqr();
	_bkg->open("LBA_BKG.HQR");
	loadGridDefaults();

	_ress = new Hqr();
	_ress->open("RESS.HQR");
	_ei = new EntityInformation(_ress->createReadStreamForIndex(44));

	_body = new Hqr();
	_body->open("BODY.HQR");

	_anim = new Hqr();
	_anim->open("ANIM.HQR");

	_scene = new Hqr();
	_scene->open("SCENE.HQR");
}

Resource::~Resource() {
	delete _bkg;
	delete _ress;
	delete _ei;
	delete _body;
	delete _anim;
	delete _scene;
}

Scene *Resource::getScene(uint16 id) {
	Scene *s = new Scene(_scene->createReadStreamForIndex(id + 1));
	if (!_scenes[id]._isIsland) {
		s->setGrid(getGrid(_scenes[id]._id));
	}
	s->setId(id);
	return s;
}

Grid *Resource::getGrid(int id) {
	Grid *g = new Grid(_bkg->createReadStreamForIndex(_firstGrid + id));
	return g;
}

//TODO: Make these functions not so duplicated.
// Maybe try using some kind of fancy template
Block *Resource::getBlock(int block) {
	if (_blocks.contains(block)) {
		return _blocks[block];
	}
	Block *b = new Block(_bkg->createReadStreamForIndex(_firstBlock + block));
	_blocks[block] = b;
	return b;
}

BlockLibrary *Resource::getBlockLibrary(int block) {
	if (_blockLibraries.contains(block)) {
		return _blockLibraries[block];
	}
	BlockLibrary *b = new BlockLibrary(_bkg->createReadStreamForIndex(_firstLibrary + block));
	_blockLibraries[block] = b;
	return b;
}

void Resource::loadGridDefaults() {
	Common::SeekableReadStream *stream = _bkg->createReadStreamForIndex(0);
	_firstGrid = stream->readUint16LE();
	_firstGridFragment = stream->readUint16LE();
	_firstLibrary = stream->readUint16LE();
	_firstBlock = stream->readUint16LE();
	_numBlocks = stream->readUint16LE();
	_numScenes = 0;
	delete stream;
	stream = _bkg->createReadStreamForIndex(_firstBlock + _numBlocks);
	for (;;) {
		byte opcode = stream->readByte();
		byte id = stream->readByte();
		if (opcode == 0) {
			break;
		} else if (opcode == 1) {
			_scenes[_numScenes]._isIsland = false;
		} else if (opcode == 2) {
			_scenes[_numScenes]._isIsland = true;
		}
		_scenes[_numScenes]._id = id;
		++_numScenes;
	}
	delete stream;
}

Animation *Resource::getAnimation(uint16 id, Model *m) {
	return new Animation(_anim->createReadStreamForIndex(id), m);
}

Entity *Resource::getEntity(uint16 entity, uint16 body, uint16 anim) {
	if (entity >= _ei->_numEntities) {
		entity = _ei->_numEntities - 1;
	}
	EntityEntry *e = &_ei->_entities[entity];
	if (body >= e->_bodies.size()) {
		body = e->_bodies.size() - 1;
	}
	if (anim >= e->_anims.size()) {
		anim = e->_anims.size() - 1;
	}

	Model *m = new Model(_body->createReadStreamForIndex(e->_bodies[body]._bodyIndex));
	Animation *a = getAnimation(e->_anims[anim]._animIndex, m);
	Entity *ent = new Entity(m, a);
	return ent;
}

} // end of namespace Twin
