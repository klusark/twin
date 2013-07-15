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

#include "engines/twin/scene.h"

namespace Twin {

Scene::Scene(Common::SeekableReadStream *stream) {
	_textBank = stream->readByte();
	_gameOverScene = stream->readByte();
	stream->readUint32LE();

	stream->readUint16LE();
	stream->readUint16LE();

	_ambience[0] = stream->readUint16LE();
	_repeat[0] = stream->readUint16LE();
	_round[0] = stream->readUint16LE();

	_ambience[1] = stream->readUint16LE();
	_repeat[1] = stream->readUint16LE();
	_round[1] = stream->readUint16LE();

	_ambience[2] = stream->readUint16LE();
	_repeat[2] = stream->readUint16LE();
	_round[2] = stream->readUint16LE();

	_ambience[3] = stream->readUint16LE();
	_repeat[3] = stream->readUint16LE();
	_round[3] = stream->readUint16LE();


	stream->readUint16LE();
	stream->readUint16LE();

	stream->readByte();

	_heroX = stream->readUint16LE();
	_heroY = stream->readUint16LE();
	_heroZ = stream->readUint16LE();

	_moveScriptSize = stream->readUint16LE();
	_moveScript = new uint8[_moveScriptSize];
	stream->read(_moveScript, _moveScriptSize);

	_lifeScriptSize = stream->readUint16LE();
	_lifeScript = new uint8[_lifeScriptSize];
	stream->read(_lifeScript, _lifeScriptSize);

	_numActors = stream->readUint16LE();
	_actors = new SceneActor[_numActors];

	for (int i = 0; i < _numActors; ++i) {
		uint16 flags = stream->readUint16LE();
		SceneActor *a = &_actors[i];
		a->_entity = stream->readUint16LE();
		a->_body = stream->readByte();
		a->_anim = stream->readByte();
		a->_sprite = stream->readUint16LE();
		a->_x = stream->readUint16LE();
		a->_y = stream->readUint16LE();
		a->_z = stream->readUint16LE();
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
		stream->readUint16LE();

	}
	_numZones = stream->readUint16LE();
}


} // end of namespace Twin
