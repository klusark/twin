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

#include "engines/twin/resource.h"
#include "engines/twin/twin.h"
#include "engines/twin/hqr.h"
#include "engines/twin/block.h"
#include "engines/twin/block_library.h"
#include "engines/twin/grid.h"

namespace Twin {

Resource *g_resource = nullptr;

Resource::Resource() {
	f = new Hqr();
	f->open("LBA_BKG.HQR");
	loadGridDefaults();
}

Resource::~Resource() {

}

Grid *Resource::getGrid(int id) {
	Grid *g = new Grid(f->createReadStreamForIndex(_firstGrid + id));
	return g;
}

//TODO: Make these functions not so duplicated.
// Maybe try using some kind of fancy template
Block *Resource::getBlock(int block) {
	if (_blocks.contains(block)) {
		return _blocks[block];
	}
	Block *b = new Block(f->createReadStreamForIndex(_firstBlock + block));
	_blocks[block] = b;
	return b;
}

BlockLibrary *Resource::getBlockLibrary(int block) {
	if (_blockLibraries.contains(block)) {
		return _blockLibraries[block];
	}
	BlockLibrary *b = new BlockLibrary(f->createReadStreamForIndex(_firstLibrary + block));
	_blockLibraries[block] = b;
	return b;
}

void Resource::loadGridDefaults() {
	Common::SeekableReadStream *stream = f->createReadStreamForIndex(0);
	_firstGrid = stream->readUint16LE();
	_firstGridFragment = stream->readUint16LE();
	_firstLibrary = stream->readUint16LE();
	_firstBlock = stream->readUint16LE();
	delete stream;
}

} // end of namespace Twin
