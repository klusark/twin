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
#include "common/textconsole.h"

namespace Twin {

Actor::Actor(Common::SeekableReadStream *stream) : _entity(nullptr) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	}
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

	_moveScriptSize = stream->readUint16LE();
	_moveScript = new uint8[_moveScriptSize];
	stream->read(_moveScript, _moveScriptSize);

	_lifeScriptSize = stream->readUint16LE();
	_lifeScript = new uint8[_lifeScriptSize];
	stream->read(_lifeScript, _lifeScriptSize);

}

void Actor::update(uint32 delta) {
	if (_entity) {
		_entity->_anim->update(delta);
	}
}

void Actor::draw() {
	if (_entity) {
		g_renderer->drawActor(this);
	}
}

} // end of namespace Twin
