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

#ifndef TWIN_MODEL_H
#define TWIN_MODEL_H


namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Polygon {
public:
	uint16 *_data;
	uint16 _num;
	byte _colour;
};

class Vertex {
public:
	float _x;
	float _y;
	float _z;
	uint16 _bone;
};

class Normal {
public:
	float _x;
	float _y;
	float _z;
};

class Bone {
public:
	uint16 _parent;
	uint16 _vertex;
};


class Model {
public:
	Model(Common::SeekableReadStream *stream);
//private:
	void loadLBA2(Common::SeekableReadStream *stream);

	uint32 _numVerticies;
	uint32 _numBones;
	uint32 _numNormals;
	uint32 _numPolygons;
	uint32 _numSphere;

	Polygon *_polygons;
	Vertex *_verticies;
	Normal *_normals;
	Bone *_bones;
};

} // end of namespace Twin

#endif
