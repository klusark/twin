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

namespace Twin {

Actor::Actor(Common::SeekableReadStream *stream) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	}
}


void Actor::loadLBA2(Common::SeekableReadStream *stream) {
	//These sizes are probably mostly wrong, but the overall actor size is right
	uint16 flags = stream->readUint16LE();
	_entity = stream->readUint16LE();
	_body = stream->readByte();
	_anim = stream->readByte();
	_sprite = stream->readUint16LE();
	_x = stream->readUint16LE();
	_y = stream->readUint16LE();
	_z = stream->readUint16LE();
	stream->readByte();
	stream->readUint16LE();
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
	stream->readByte();
	stream->readUint16LE();
	stream->readByte();

	_moveScriptSize = stream->readUint16LE();
	_moveScript = new uint8[_moveScriptSize];
	stream->read(_moveScript, _moveScriptSize);

	_lifeScriptSize = stream->readUint16LE();
	_lifeScript = new uint8[_lifeScriptSize];
	stream->read(_lifeScript, _lifeScriptSize);

}


} // end of namespace Twin
