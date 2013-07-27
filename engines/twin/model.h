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

#include "math/mathfwd.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Model;

class Polygon {
public:
	uint16 *_data;
	uint16 _num;
	byte _colour;
};

class Vertex {
public:
	Math::Vector3d getPos(Model *m);
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

class Texture {
public:
	byte _x;
	byte _y;
	byte _w;
	byte _h;
};

class Point {
public:
	uint16 _v1;
	uint16 _v2;
	byte _colour;
};

class Sphere {
public:
	uint16 _vertex;
	float _size;
	byte _colour;
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
	uint32 _numSpheres;
	uint32 _numTextures;
	uint32 _numPoints;

	Polygon *_polygons;
	Vertex *_verticies;
	Normal *_normals;
	Bone *_bones;
	Texture *_textures;
	Point *_points;
	Sphere *_spheres;
};

} // end of namespace Twin

#endif
