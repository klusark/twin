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
		float x, y ,z;
		Vertex *v = &_verticies[i];
		x = stream->readSint16LE() / 16384.0f;
		y = stream->readSint16LE() / 16384.0f;
		z = stream->readSint16LE() / 16384.0f;
		v->_pos.set(x, y, z);
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
	memset(_polygons, 0, _numPolygons * sizeof(Polygon));
	uint32 currPoly = 0;
	while (currPoly < _numPolygons) {
		int flag = stream->readUint16LE();
		int num = stream->readUint16LE();
		int size = stream->readUint16LE();
		int num3 = stream->readUint16LE();

		int val = ((size - 8) / num);
		for (int j = 0; j < num; ++j) {
			Polygon *p = &_polygons[currPoly];
			p->_num = val / 2;
			p->_colour = 0;
			p->_hasTex = flag & 0x8 && val > 16;
			int texc = 0;
			for (int i = 0; i < (val / 2); ++i) {
				if (i == 14 && p->_hasTex) {
					p->_tex = stream->readByte();
					stream->readByte();
					stream->readByte();
					stream->readByte();
					++i;
				} else if (i > 5 && p->_hasTex) {
					//??
					stream->readByte();

					byte x = stream->readByte();

					//??
					stream->readByte();

					byte y = stream->readByte();
					if (texc < 4) {
						p->_texX[texc] = x;
						p->_texY[texc] = y;
					}
					++texc;
					++i;
				} else {
					if (p->_hasTex && i == 3 && val != 32) {
						p->_tex = stream->readByte();
						stream->readByte();
						//p->_data[i] = stream->readByte();
					} else if (i == 4) {
						uint16 data = stream->readUint16LE();
						p->_colour = (data << 4) >> 4;
					} else {
						uint16 data = stream->readUint16LE();
						if (i < 4) {
							p->_data[i] = data;
						}
					}
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
		t->_renderData = NULL;
		t->_x = stream->readByte();
		t->_y = stream->readByte();
		t->_w = stream->readByte();
		t->_h = stream->readByte();
	}

}


Math::Vector3d Vertex::getPos(Model *m) {
	Math::Vector3d vec(_pos);
	uint16 bone = _bone;
	if (bone == 0) {
		return vec;
	}
	Bone *b = &m->_bones[bone];
	while (b) {
		Vertex *bv = &m->_verticies[b->_vertex];
		vec += bv->_pos;
		bone = b->_parent;
		if (bone == 0xffff) {
			break;
		}
		b = &m->_bones[bone];
	}
	return vec;
}

} // end of namespace Twin
