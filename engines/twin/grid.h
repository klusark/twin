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

#ifndef TWIN_GRID_H
#define TWIN_GRID_H

#include "engines/twin/block_library.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Actor;
class Point;

struct Square {
	byte _layout;
	byte _brick;
};


class Grid {
public:
	Grid(Common::SeekableReadStream *stream, uint16 id);
	void setGridAt(byte x, byte y, byte z, Square s);
	Square *getGridAt(byte x, byte y, byte z);
	byte getLibrary() { return _layoutLib; }
	BlockLibrary *getBlockLibrary() { return _blockLibrary; }
	ShapeType getBrickShape(const Point &p, Point *out = nullptr);
	ShapeType getBrickShape(int x, int y, int z, Point *out = nullptr);
private:
	void loadLBA2(Common::SeekableReadStream *stream);

	byte _layoutLib;
	byte _gridFragment;

	BlockLibrary *_blockLibrary;
	uint16 _id;

	Square _grid[64*64*25];
};

} // end of namespace Twin

#endif
