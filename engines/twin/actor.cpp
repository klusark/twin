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

#include "engines/twin/actor.h"
#include "engines/twin/twin.h"
#include "engines/twin/resource.h"
#include "engines/twin/gfx_base.h"
#include "engines/twin/animation.h"
#include "engines/twin/script_track_v2.h"
#include "engines/twin/script_life_v2.h"
#include "engines/twin/scene.h"
#include "common/textconsole.h"

namespace Twin {

Actor::Actor(Common::SeekableReadStream *stream) : _entity(nullptr), _dest(nullptr), _dead(false) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	}
}

Actor::Actor() : _entity(nullptr), _dest(nullptr), _dead(false) {
	_entity = g_resource->getEntity(0, 0, 0);
	_x = 0;
	_y = 0;
	_z = 0;
}


void Actor::loadLBA2(Common::SeekableReadStream *stream) {
	//These sizes are probably mostly wrong, but the overall actor size is right
	uint16 flags = stream->readUint16LE();
	uint16 flag2 = stream->readUint16LE();

	_entityID = stream->readUint16LE();
	_body = stream->readByte();
	stream->readByte();
	_anim = stream->readByte();
	_sprite = stream->readUint16LE();
	_x = stream->readUint16LE();
	_y = stream->readUint16LE();
	_z = stream->readUint16LE();
	if (_entityID != 0xffff) {
		_entity = g_resource->getEntity(_entityID, 0, 0);
	}

	stream->readByte();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readByte();
	stream->readByte();
	stream->readByte();
	stream->readUint16LE();
	stream->readByte();

	if (flag2 & 0x004) {
		stream->readByte();
		stream->readByte();
		stream->readByte();
		stream->readByte();
		stream->readByte();
		stream->readByte();
	}

	_trackScript = new ScriptTrackV2(stream);
	_trackScript->setActor(this);


	_lifeScript = new ScriptLifeV2(stream, (ScriptTrackV2 *)_trackScript);
	_lifeScript->setActor(this);
}


void Actor::update(uint32 delta) {
	if (_dead) {
		return;
	}
	_lifeScript->run();
	_trackScript->run();
	if (_entity) {
		_entity->_anim->update(delta);
	}

	if (_dest) {
		int xdiff = _dest->_x - _x;
		int ydiff = _dest->_y - _y;
		int zdiff = _dest->_z - _z;

		if (xdiff < 0) {
			_x = MAX(_x - delta, _dest->_x);
		} else if (xdiff > 0) {
			_x = MIN(_x + delta, _dest->_x);
		}
		if (ydiff < 0) {
			_y = MAX(_y - delta, _dest->_y);
		} else if (ydiff > 0) {
			_y = MIN(_y + delta, _dest->_y);
		}
		if (zdiff < 0) {
			_z = MAX(_z - delta, _dest->_z);
		} else if (zdiff > 0) {
			_z = MIN(_z + delta, _dest->_z);
		}

		if (_x ==_dest->_x && _y == _dest->_y && _z == _dest->_z) {
			_dest = nullptr;
		}
	}
}

void Actor::draw() {
	if (_dead) {
		return;
	}
	if (_entity) {
		g_renderer->drawActor(this);
	}
}

void Actor::setAnimation(uint16 anim) {
	if (_entity) {
		delete _entity->_anim;
		_entity->_anim = g_resource->getAnimation(anim, _entity->_model);
	}
}

void Actor::gotoPoint(Point *p) {
	_dest = p;
}

} // end of namespace Twin
