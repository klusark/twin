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

#include "common/file.h"
#include "common/substream.h"

#include "engines/twin/hqr.h"
#include "engines/twin/lzss.h"

namespace Twin {


bool Hqr::open(const Common::String &filename) {
	_hqrFileName = filename;

	bool result = true;

	Common::File *file = new Common::File();
	if (!file->open(filename)) {
		result = false;
	} else {
		parseFile(file);
	}

	delete file;

	return result;
}

void Hqr::parseFile(Common::File *file) {
	uint32 headerSize;

	headerSize = file->readUint32LE();

	_numIndices = headerSize / 4;
	_indices = new int[_numIndices];
	_indices[0] = headerSize;
	for (int i = 1; i < _numIndices; ++i) {
		_indices[i] = file->readUint32LE();
	}

}

Common::SeekableReadStream *Hqr::createReadStreamForIndex(int index) const {
	if (index >= _numIndices)
		return 0;

	uint32 realSize;
	uint32 compressedSize;
	uint16 mode;

	Common::File *file = new Common::File();
	file->open(_hqrFileName);
	file->seek(_indices[index]);
	realSize = file->readUint32LE();
	compressedSize = file->readUint32LE();
	mode = file->readUint16LE();
	
	uint32 begin = _indices[index] + 10;
	uint32 end = 0;
	if (mode == 0) {
		end = begin + realSize;
	} else {
		end = begin + compressedSize;
	}
	Common::SeekableReadStream *stream = new Common::SeekableSubReadStream(file, begin, end, DisposeAfterUse::YES);

	if (mode != 0) {
		stream = new LzssReadStream(stream, mode, realSize);
	}

	return stream;
}

} // end of namespace Twin
