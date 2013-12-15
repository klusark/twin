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
#include "engines/twin/grid.h"
#include "engines/twin/twin.h"
#include "engines/twin/resource.h"
#include "engines/twin/actor.h"

namespace Twin {

Grid::Grid(Common::SeekableReadStream *stream, uint16 id): _id(id) {
	memset(_grid, 0, 64*64*25*sizeof(Square));
	loadLBA2(stream);
}

// LBArchatect(GPL) by zink was used as the base for this function
void Grid::loadLBA2(Common::SeekableReadStream *stream) {
	int off = 0;
	if (g_twin->getGameType() == GType_LBA2) {
		_layoutLib = stream->readByte();
		_gridFragment = stream->readByte();
		off = 34;
	} else {
		_layoutLib = _id;
	}
	_blockLibrary = g_resource->getBlockLibrary(_layoutLib);

	int n = 0;
	int y = 0;
	

	for (int z = 0; z < 64; ++z) {
		for (int x = 0; x < 64; ++x) {
			stream->seek(n * 2 + off);
			uint16 offset = stream->readUint16LE();
			++n;
			y = 0;
			stream->seek(offset + off);
			byte groups = stream->readByte();
			for (byte c = 0; c < groups; ++c) {
				byte flags = stream->readByte();
				byte blocks = (flags & 0x1F) + 1;
				flags &= 0xE0;
				switch (flags) {
				case 0x00:
					for (byte d = 0; d < blocks; ++d) {
						Square s;
						s._layout = 0;
						s._brick = 0;
						setGridAt(x, y, z, s);
						++y;
					}
					break;
				case 0x40:
					for (byte d = 0; d < blocks; ++d) {
						Square s;
						s._layout = stream->readByte();
						s._brick = stream->readByte();
						setGridAt(x, y, z, s);
						++y;
					}
					break;
				case 0x80:
					{
						Square s;
						s._layout = stream->readByte();
						s._brick = stream->readByte();
						for (byte d = 0; d < blocks; ++d) {
							setGridAt(x, y, z, s);
							++y;
						}
					}
					break;
				}
			}
		}
	}
}

void Grid::setGridAt(byte x, byte y, byte z, Square s) {
	_grid[x * 64 * 25 + z * 25 + y] = s;
}

Square *Grid::getGridAt(byte x, byte y, byte z) {
	return &_grid[x * 64 * 25 + z * 25 + y];
}



ShapeType Grid::getBrickShape(const Point &p, Point *out) {
	return getBrickShape(p._x, p._y, p._z, out);
}

ShapeType Grid::getBrickShape(int x, int y, int z, Point *out) {
	x = (x + 0x100) >> 9;
	y = (y >> 8);
	z = (z + 0x100) >> 9;

	if (x < 0 || y < 0 || z < 0 || x >= 64 || y > 25 || z >= 64) {
		return kNone;
	}
	if (out) {
		out->_x = x;
		out->_y = y;
		out->_z = z;
	}
	Square *s = getGridAt(x, y, z);
	if (s->_layout == 0) {
		return kNone;
	}
	BlockInfo *b =_blockLibrary->getBlock(s->_layout - 1);
	SubBlock *sb = &b->_blocks[s->_brick];



	return sb->_shape;
}


} // end of namespace Twin
