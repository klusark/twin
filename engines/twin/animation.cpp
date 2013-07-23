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

#include "engines/twin/animation.h"
#include "engines/twin/twin.h"

namespace Twin {

Animation::Animation(Common::SeekableReadStream *stream) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	}
}

Animation::~Animation() {
	for (int i = 0; i < _numKeys; ++i) {
		delete[] _keyframes[i]._bones;
	}
	delete[] _keyframes;
}

void Animation::loadLBA2(Common::SeekableReadStream *stream) {
	_numKeys = stream->readUint16LE();
	_numBones = stream->readUint16LE();
	uint16 unknown0 = stream->readUint16LE();
	uint16 unknown1 = stream->readUint16LE();

	_keyframes = new Keyframe[_numKeys];
	for (int i = 0; i < _numKeys; ++i) {
		Keyframe *k = &_keyframes[i];
		k->_length = stream->readUint16LE();
		k->_x = stream->readSint16LE();
		k->_y = stream->readSint16LE();
		k->_z = stream->readSint16LE();
		k->_bones = new Boneframe[_numBones];
		for (int j = 0; j < _numBones; ++j) {
			Boneframe *b = &k->_bones[j];
			b->_type = stream->readUint16LE();
			b->_x = stream->readSint16LE();
			b->_y = stream->readSint16LE();
			b->_z = stream->readSint16LE();
		}
	}
}


} // end of namespace Twin
