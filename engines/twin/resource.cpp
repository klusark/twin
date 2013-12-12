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
#include "engines/twin/sprite.h"

namespace Twin {

Entity::~Entity() {
	delete _anim;
	delete _model;
}
void Entity::update(uint32 time) {
	_anim->update(time);
}

Resource *g_resource = nullptr;

Resource::Resource() : _firstGrid(0) {
	_scene = new Hqr();
	_scene->open("SCENE.HQR");

	_body = new Hqr();
	_body->open("BODY.HQR");

	_anim = new Hqr();
	_anim->open("ANIM.HQR");

	_ress = new Hqr();
	_ress->open("RESS.HQR");

	_sprites = new Hqr();
	_sprites->open("SPRITES.HQR");

	if (g_twin->getGameType() == GType_LBA2) {
		_bkg = new Hqr();
		_bkg->open("LBA_BKG.HQR");
		loadGridDefaults();

		_ei = new EntityInformation(_ress->createReadStreamForIndex(44));

		loadSpriteInfo(_ress->createReadStreamForIndex(5));
	} else if (g_twin->getGameType() == GType_LBA) {
		_gri = new Hqr();
		_gri->open("LBA_GRI.HQR");

		_brk = new Hqr();
		_brk->open("LBA_BRK.HQR");

		_bll = new Hqr();
		_bll->open("LBA_BLL.HQR");

		_f3d = new Hqr();
		_f3d->open("FILE3D.HQR");

		_ei = new EntityInformation(_f3d);

		loadSpriteInfo(_ress->createReadStreamForIndex(3));
	}
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
	int streamid = id;
	if (g_twin->getGameType() == GType_LBA2) {
		++streamid;
	}
	Scene *s = new Scene(_scene->createReadStreamForIndex(streamid));
	if (g_twin->getGameType() == GType_LBA) {
		s->setGrid(getGrid(id));
	} else if (!_scenes[id]._isIsland) {
		s->setGrid(getGrid(_scenes[id]._id));
	}
	s->setId(id);
	return s;
}

Sprite *Resource::getSprite(uint16 id) {
	Sprite *s = new Sprite(_sprites->createReadStreamForIndex(id ));
	s->setSpriteInfo(&_spriteInfo[id]);
	return s;
}


Grid *Resource::getGrid(int id) {
	Common::SeekableReadStream *stream;
	if (g_twin->getGameType() == GType_LBA2) {
		stream = _bkg->createReadStreamForIndex(_firstGrid + id);
	} else {
		stream = _gri->createReadStreamForIndex(id);
	}
	Grid *g = new Grid(stream);
	return g;
}

//TODO: Make these functions not so duplicated.
// Maybe try using some kind of fancy template
Block *Resource::getBlock(int block) {
	if (_blocks.contains(block)) {
		return _blocks[block];
	}
	Common::SeekableReadStream *stream;
	if (g_twin->getGameType() == GType_LBA2) {
		stream = _bkg->createReadStreamForIndex(_firstBlock + block);
	} else {
		stream = _brk->createReadStreamForIndex(block);
	}
	Block *b = new Block(stream);
	_blocks[block] = b;
	return b;
}

BlockLibrary *Resource::getBlockLibrary(int block) {
	if (_blockLibraries.contains(block)) {
		return _blockLibraries[block];
	}
	Common::SeekableReadStream *stream;
	if (g_twin->getGameType() == GType_LBA2) {
		stream = _bkg->createReadStreamForIndex(_firstLibrary + block);
	} else {
		stream = _bll->createReadStreamForIndex(block);
	}
	BlockLibrary *b = new BlockLibrary(stream);
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

Animation *Resource::getAnimation(uint16 entity, uint16 id, Model *m) {
	EntityEntry *e = &_ei->_entities[entity];
	int index = 0;
	for (uint32 i = 0; i < e->_anims.size(); ++i) {
		EntityAnim *ea = &e->_anims[i];
		if (ea->_index == id) {
			index = ea->_animIndex;
			break;
		}
	}
	return new Animation(_anim->createReadStreamForIndex(index), m, id);
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
	EntityBody *b = &e->_bodies[body];
	Model *m = new Model(_body->createReadStreamForIndex(b->_bodyIndex));
	Animation *a = getAnimation(entity, anim, m);
	Entity *ent = new Entity(m, a);
	ent->_hasBox = b->_hasBox;
	ent->_x1 = b->_x1;
	ent->_x2 = b->_x2;
	ent->_y1 = b->_y1;
	ent->_y2 = b->_y2;
	ent->_z1 = b->_z1;
	ent->_z2 = b->_z2;
	return ent;
}

void Resource::loadSpriteInfo(Common::SeekableReadStream *stream) {
	int numSprites = _sprites->getNumIndices() - 1;
	_spriteInfo = new SpriteInfo[numSprites];

	for (int i = 0; i < numSprites; ++i) {
		SpriteInfo *info = &_spriteInfo[i];
		info->_x = stream->readSint16LE();
		info->_y = stream->readSint16LE();
		info->_x1 = stream->readSint16LE();
		info->_x2 = stream->readSint16LE();
		info->_y1 = stream->readSint16LE();
		info->_y2 = stream->readSint16LE();
		info->_z1 = stream->readSint16LE();
		info->_z2 = stream->readSint16LE();
	}
}

} // end of namespace Twin
