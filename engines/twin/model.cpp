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

#include "math/vector3d.h"

#include "engines/twin/model.h"
#include "engines/twin/twin.h"

namespace Twin {

Model::Model(Common::SeekableReadStream *stream) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	}
}

void Model::loadLBA2(Common::SeekableReadStream *stream) {
	stream->seek(0x20);
	_numBones = stream->readUint32LE();
	uint32 bonesOffset = stream->readUint32LE();

	_numVerticies = stream->readUint32LE();
	uint32 verticiesOffset = stream->readUint32LE();

	_numNormals = stream->readUint32LE();
	uint32 normalsOffset = stream->readUint32LE();

	uint32 numUnknown1 = stream->readUint32LE();
	uint32 offsetUnknown1 = stream->readUint32LE();

	_numPolygons = stream->readUint32LE();
	uint32 polygonOffset = stream->readUint32LE();

	_numPoints = stream->readUint32LE();
	uint32 pointsOffset = stream->readUint32LE();

	_numSpheres = stream->readUint32LE();
	uint32 sphereOffset = stream->readUint32LE();

	_numTextures = stream->readUint32LE();
	uint32 texturesOffset = stream->readUint32LE();

	stream->seek(bonesOffset);

	_bones = new Bone[_numBones];
	for (uint32 i = 0; i < _numBones; ++i) {
		Bone *b = &_bones[i];
		b->_parent = stream->readSint16LE();
		b->_vertex = stream->readSint16LE();
		stream->readSint16LE();
		stream->readSint16LE();

	}

	_verticies = new Vertex[_numVerticies];
	for (uint32 i = 0; i < _numVerticies; ++i) {
		int16 x, y ,z;
		Vertex *v = &_verticies[i];
		x = stream->readSint16LE();
		y = stream->readSint16LE();
		z = stream->readSint16LE();
		v->_x = ((float)x) / 16384.0f;
		v->_y = ((float)y) / 16384.0f;
		v->_z = ((float)z) / 16384.0f;
		v->_bone = stream->readUint16LE();
	}

	_normals = new Normal[_numNormals];
	for (uint32 i = 0; i < _numNormals; ++i) {
		Normal *n = &_normals[i];
		n->_x = stream->readSint16LE();
		n->_y = stream->readSint16LE();
		n->_z = stream->readSint16LE();
		stream->readUint16LE();
	}

	for (uint32 i = 0; i < numUnknown1; ++i) {
		stream->readUint32LE();
		stream->readUint32LE();
	}

	_polygons = new Polygon[_numPolygons];
	uint32 currPoly = 0;
	while (currPoly < _numPolygons) {
		int flag = stream->readUint16LE();
		int num = stream->readUint16LE();
		int size = stream->readUint16LE();
		int num3 = stream->readUint16LE();
		int val = ((size - 8) / num);
		for (int i = 0; i < num; ++i) {
			_polygons[currPoly]._data = new uint16[val / 2];
			_polygons[currPoly]._num = val / 2;
			for (int i = 0; i < (val / 2); ++i) {
				uint16 data = stream->readUint16LE();
				_polygons[currPoly]._data[i] = data;
				if (data > _numVerticies) {
					_polygons[currPoly]._colour = (data << 4) >> 4;
				}
			}
			++currPoly;
		}
	}

	_points = new Point[_numPoints];
	for (uint32 i = 0; i < _numPoints; ++i) {
		Point *p = &_points[i];
		stream->readUint16LE();
		p->_colour = stream->readUint16LE();
		p->_v1 = stream->readUint16LE();
		p->_v2 = stream->readUint16LE();
	}

	_spheres = new Sphere[_numSpheres];
	for (uint32 i = 0; i < _numSpheres; ++i) {
		Sphere *s = &_spheres[i];
		stream->readUint16LE();
		s->_colour = stream->readUint16LE();
		s->_vertex = stream->readUint16LE();
		s->_size = stream->readUint16LE() / 16384.0;
	}

	_textures = new Texture[_numTextures];
	for (uint32 i = 0; i < _numTextures; ++i) {
		Texture *t = &_textures[i];
		t->_x = stream->readByte();
		t->_y = stream->readByte();
		t->_w = stream->readByte();
		t->_h = stream->readByte();
	}

}


Math::Vector3d Vertex::getPos(Model *m) {
	Math::Vector3d vec(_x, _y, _z);
	Bone *b = &m->_bones[_bone];
	while (b) {
		Vertex *bv = &m->_verticies[b->_vertex];
		Math::Vector3d bone_vec(bv->_x, bv->_y, bv->_z);
		vec += bone_vec;
		if (b->_parent == 0xffff) {
			break;
		}
		b = &m->_bones[b->_parent];
	}
	return vec;
}

} // end of namespace Twin
