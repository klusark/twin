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

#include "engines/twin/island.h"
#include "engines/twin/twin.h"

namespace Twin {

Island::Island(Hqr *hqr) {
	int indeces = hqr->getNumIndeces();

	_hightMaps = new uint16*[(indeces-2)/6];

	int mapid = 0;
	// The rest of the data isn't known to me right now.
	// there is probably texture data and texture coords
	for (int i = 7; i < indeces; i += 6) {
		Common::SeekableReadStream *stream = hqr->createReadStreamForIndex(i);
		loadHeightMap(stream, mapid);
		delete stream;
		++mapid;
	}
}

void Island::loadHeightMap(Common::SeekableReadStream *stream, int mapid) {
	_hightMaps[mapid] = new uint16[65*65];
	for (int y = 0; y < 65; ++y) {
		for (int x = 0; x < 65; ++x) {
			_hightMaps[mapid][y*65 + x] = stream->readUint16LE();
		}
	}

}

} // end of namespace Twin
