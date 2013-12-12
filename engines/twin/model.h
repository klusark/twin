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

#include "math/vector3d.h"
#include "math/vector4d.h"
#include "math/matrix4.h"

#include "common/array.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Model;

class Polygon {
public:
	uint16 _data[4];
	uint16 _num;
	uint16 _tex;
	uint16 _numVerticies;
	byte _texX[4];
	byte _texY[4];
	byte _colour;
	bool _hasTransparency;
	bool _hasTex;

	bool operator < (const Polygon &poly) const;
};

class Vertex {
public:
	Math::Vector3d getPos(Model *m);
	Math::Vector3d _pos;
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
	void *_renderData;
};

class Line {
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

class Hierarchy {
public:
	Hierarchy() :_translation(0,0,0) {
		_isTransltion = true;
	}
	void computeWorldMatrix(Math::Matrix4 parentMtx);
	Math::Matrix4 computeLocalMatrix();
	Math::Matrix4 _worldMatrix;
	Common::Array<Hierarchy *> _children;
	Vertex *_vertex;
	uint16 _index;

	bool _isTransltion;
	Math::Vector3d _translation;
	Math::Angle _pitch;
	Math::Angle _yaw;
	Math::Angle _roll;
};

class Model {
public:
	Model(Common::SeekableReadStream *stream);
	~Model();
	void recalculateHierarchy();
//private:
	void loadLBA2(Common::SeekableReadStream *stream);
	void loadLBA(Common::SeekableReadStream *stream);
	void createHierarchy();

	uint32 _numVerticies;
	uint32 _numBones;
	uint32 _numNormals;
	uint32 _numPolygons;
	uint32 _numSpheres;
	uint32 _numTextures;
	uint32 _numLines;

	Polygon *_polygons;
	Vertex *_verticies;
	Normal *_normals;
	Bone *_bones;
	Texture *_textures;
	Line *_lines;
	Sphere *_spheres;
	Hierarchy *_heir;
	Hierarchy *_heirs;
};

} // end of namespace Twin

#endif
