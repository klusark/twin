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

#include "engines/twin/block_library.h"
#include "engines/twin/twin.h"

namespace Twin {

BlockLibrary::BlockLibrary(Common::SeekableReadStream *stream) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	}
}

void BlockLibrary::loadLBA2(Common::SeekableReadStream *stream) {
	uint32 firstOffset = stream->readUint32LE();

	_numOffsets = firstOffset/4;

	_offsets = new uint32[_numOffsets];
	_offsets[0] = firstOffset;
	for (int i = 1; i < _numOffsets; ++i) {
		_offsets[i] = stream->readUint32LE();
	}

	_blocks = new Block[_numOffsets];
	for (int i = 0; i < _numOffsets; ++i) {
		Block *b = &_blocks[i];
		b->_x = stream->readByte();
		b->_y = stream->readByte();
		b->_z = stream->readByte();
		int numBlocks = b->_x * b->_y * b->_z;
		for (int j = 0; j < numBlocks; ++j) {
			b->_shape  = stream->readByte();
			b->_type  = stream->readByte();
			b->_index  = stream->readUint16LE();
		}
	}
}



} // end of namespace Twin
