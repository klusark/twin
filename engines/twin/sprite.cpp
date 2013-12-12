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

#include "engines/twin/sprite.h"
#include "engines/twin/twin.h"

namespace Twin {

Sprite::Sprite(Common::SeekableReadStream *stream) : _info(nullptr), _renderData(nullptr) {

	//if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	//}
	delete stream;
}

Sprite::~Sprite() {
	delete[] _data;
}

// This code should apply to both games
void Sprite::loadLBA2(Common::SeekableReadStream *stream) {
	uint32 start = stream->readUint32LE();
	stream->seek(start, SEEK_SET);
	_width = stream->readByte();
	_height = stream->readByte();
	_offsetX = stream->readByte();
	_offsetY = stream->readByte();
	_data = new byte[_width * _height];

	uint32 pixel = 0;

	for (byte y = 0; y < _height; ++y) {
		byte numRuns = stream->readByte();
		for (byte r = 0; r < numRuns; ++r) {
			byte spec = stream->readByte();
			bool colourFlag = spec & 0x80;
			bool copyFlag = spec & 0x40;
			byte runLength = (spec & 0x3F) + 1;
			if (colourFlag) {
				byte colour = stream->readByte();
				for (byte i = 0; i < runLength; ++i) {
					_data[pixel] = colour;
					++pixel;
				}
			} else if (copyFlag) {
				for (byte i = 0; i < runLength; ++i) {
					byte colour = stream->readByte();
					_data[pixel] = colour;
					++pixel;
				}
			} else {
				for (byte i = 0; i < runLength; ++i) {
					_data[pixel] = 0;
					++pixel;
				}
			}
		}
	}
}



} // end of namespace Twin
