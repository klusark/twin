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

#ifndef TWIN_ISLAND_H
#define TWIN_ISLAND_H

#include "math/vector3d.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Hqr;

struct IslandVertex {
	Math::Vector3d _pos;

	float r;
	float g;
	float b;

	//not sure if this should be a...
	float a;

	float u;
	float v;
};

struct IslandFace {
	uint16 x;
	uint16 y;
	uint16 z;
};

struct IslandSection {
	Common::Array<IslandFace> faces;
	Common::Array<IslandVertex> vertices;
	byte y;
	byte x;
};

class Island {
public:
	Island(Hqr *hqr);

private:

	Hqr *_ile;
	byte *_sectionsLayout;
	//uint16 *_heightMap;
	
	void loadIsland();
	void loadIslandSection(int32 sectionIdx, int32 idx);

public:
	IslandSection *_sections;
	uint32 _numSections;
};

} // end of namespace Twin

#endif
