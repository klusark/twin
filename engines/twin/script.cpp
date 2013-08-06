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
#include "common/textconsole.h"

#include "engines/twin/script.h"
#include "engines/twin/twin.h"

namespace Twin {

uint16 Script::_gameVars[256];

Script::Script(Common::SeekableReadStream *stream) : _actor(nullptr), _isWaiting(false) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	}
	_isExecuting = true;
	_isYielded = false;
}

Script::~Script() {
	delete[] _data;
}

void Script::loadLBA2(Common::SeekableReadStream *stream) {
	_length = stream->readUint16LE();
	_data = new uint8[_length];
	stream->read(_data, _length);

	_ptr = _data;
}

void Script::setOpcode(byte opcode) {
	*_opcodePtr = opcode;
}

void Script::run(uint32 delta) {
	if (_isWaiting) {
		_waitedTime += delta;
		if (_waitedTime > _waitTime) {
			_isWaiting = false;
		}
	}
	//warning("start");
	while (_isExecuting && !_isYielded && !_isWaiting) {
		_opcodePtr = _ptr;
		byte opcode = _ptr[0];
		++_ptr;
		execute(opcode);
	}
	_isYielded = false;
}

void Script::start() {
	_isExecuting = true;
}

void Script::stop() {
	_isExecuting = false;
}
void Script::yield() {
	_isYielded = true;
}

void Script::jump(uint16 size) {
	_ptr += size;
}

void Script::jumpAddress(uint16 size) {
	_ptr = _data + size;;
}

uint16 Script::getGameVar(byte id) {
	return _gameVars[id];
}

void Script::setGameVar(byte id, uint16 val) {
	_gameVars[id] = val;
}

byte Script::getParamByte() {
	byte param = _ptr[0];
	++_ptr;
	return param;
}

uint16 Script::getParamUint16() {
	uint16 param = ((uint16 *)_ptr)[0];
	++_ptr;
	++_ptr;
	return param;
}

int16 Script::getParamInt16() {
	int16 param = ((int16 *)_ptr)[0];
	++_ptr;
	++_ptr;
	return param;
}

} // end of namespace Twin
