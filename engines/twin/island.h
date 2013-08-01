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
class Model;

struct IslandVertex {
	Math::Vector3d _pos;

	//intencity
	float r;
	float g;
	float b;

	bool _hasTexture;
	bool _hasColour;
	byte _colour;

	//not sure if this should be a...
	float a;

	float u;
	float v;
};

struct IslandFace {
	IslandVertex _vertices[3];
};

/* This structure maps an object positioning, index in OBL, and rotation data entry in the ILE files */
struct IslandObjectInfo {
	uint32 _index;
	Math::Vector3d _pos;
	byte _angle;
	Model *_model;
};

struct IslandSection {
	uint16 _numFaces;
	IslandFace *_faces;
	byte y;
	byte x;
	uint32 _numObjects;
	IslandObjectInfo *_objects;
};

class Island {
public:
	Island(Hqr *hqr, Hqr *obl);

private:

	Hqr *_ile;
	Hqr *_obl;
	byte *_sectionsLayout;
	//uint16 *_heightMap;
	
	void loadIsland();
	void loadIslandSection(int32 sectionIdx, int32 idx);

public:
	IslandSection *_sections;
	uint32 _numSections;
	byte *_texture;
	void *_renderData;
};

} // end of namespace Twin

#endif
