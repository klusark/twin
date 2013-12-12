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
#include "common/algorithm.h"

#include "math/vector3d.h"
#include "math/vector4d.h"
#include "math/matrix4.h"

#include "engines/twin/model.h"
#include "engines/twin/twin.h"

namespace Twin {

Model::Model(Common::SeekableReadStream *stream) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	} else {
		loadLBA(stream);
	}
	delete stream;
}

Model::~Model() {
	g_renderer->freeModel(this);
	delete[] _polygons;
	delete[] _verticies;
	delete[] _bones;
	delete[] _normals;
	delete[] _lines;
	delete[] _heirs;
	delete[] _spheres;
	delete[] _textures;
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

	_numLines = stream->readUint32LE();
	uint32 linesOffset = stream->readUint32LE();

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
			p->_hasTransparency = flag & 0x2;
			p->_numVerticies = 0;
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
					} else if (i == 4) {
						uint16 data = stream->readUint16LE();
						p->_colour = (data & 0x00FF);
					} else {
						uint16 data = stream->readUint16LE();
						if (i < 4 && flag & 0x8000 || i < 3) {
							p->_data[i] = data;
							++p->_numVerticies;
						}
					}
				}

			}
			++currPoly;
		}
	}

	_lines = new Line[_numLines];
	for (uint32 i = 0; i < _numLines; ++i) {
		Line *p = &_lines[i];
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

	createHierarchy();

}

void Model::loadLBA(Common::SeekableReadStream *stream) {
	stream->seek(0x1A);


	_numVerticies = stream->readUint16LE();
	_verticies = new Vertex[_numVerticies];
	for (uint32 i = 0; i < _numVerticies; ++i) {
		float x, y ,z;
		Vertex *v = &_verticies[i];
		x = stream->readSint16LE() / 16384.0f;
		y = stream->readSint16LE() / 16384.0f;
		z = stream->readSint16LE() / 16384.0f;
		v->_pos.set(x, y, z);
		v->_bone = 0;
	}

	_numBones = stream->readUint16LE();
	_bones = new Bone[_numBones];
	for (uint32 i = 0; i < _numBones; ++i) {
		int16 firstPoint = stream->readSint16LE() / 6;
		int16 numOfPoints = stream->readSint16LE();
		int16 basePoint = stream->readSint16LE() / 6;
		int16 baseElement = stream->readSint16LE();
		int16 flag = stream->readSint16LE();
		int16 rotateZ = stream->readSint16LE();
		int16 rotateY = stream->readSint16LE();
		int16 rotateX = stream->readSint16LE();
		int32 numOfShades = stream->readSint32LE();
		int32 field_14 = stream->readSint32LE();
		int32 field_18 = stream->readSint32LE();
		int32 Y = stream->readSint32LE();
		int32 field_20 = stream->readSint32LE();
		int16 field_24 = stream->readSint16LE();
		Bone *b = &_bones[i];
		if (baseElement == -1) {
			b->_parent = 0xffff;
		} else {
			assert(baseElement % 38 == 0);
			b->_parent = baseElement / 38;
		}
		b->_vertex = basePoint;
		for (int j = 0; j < numOfPoints; ++j) {
			_verticies[firstPoint + j]._bone = i;
		}
	}

	uint32 numShades = stream->readUint16LE();
	for (uint32 i = 0; i < numShades; ++i) {
		uint16 a = stream->readUint16LE();
		uint16 b = stream->readUint16LE();
		uint16 c = stream->readUint16LE();
		uint16 d = stream->readUint16LE();
	}

	_numPolygons = stream->readUint16LE();
	_polygons = new Polygon[_numPolygons];
	memset(_polygons, 0, _numPolygons * sizeof(Polygon));
	for (uint32 i = 0; i < _numPolygons; ++i) {
		Polygon *p = &_polygons[i];
		int rendertype = stream->readByte();
		int numvert = stream->readByte();
		int colorindex = stream->readUint16LE();
		p->_numVerticies = numvert;
		p->_colour = colorindex;

		assert(numvert < 9);

		if (rendertype >= 9) {
			for (int j = 0; j < numvert; ++j) {
				uint16 shade = stream->readUint16LE();
				p->_data[j] = stream->readUint16LE() / 6;
			}
		} else if (rendertype >= 7) {
			uint16 shade = stream->readUint16LE();
			for (int j = 0; j < numvert; ++j) {
				p->_data[j] = stream->readUint16LE() / 6;
			}
		} else {
			for (int j = 0; j < numvert; ++j) {
				p->_data[j] = stream->readUint16LE() / 6;
			}
		}
	}

	_numLines = stream->readUint16LE();
	_lines = new Line[_numLines];
	for (uint32 i = 0; i < _numLines; ++i) {
		Line *p = &_lines[i];
		int a = stream->readUint16LE();
		p->_colour = stream->readUint16LE();
		p->_v1 = stream->readUint16LE() / 6;
		p->_v2 = stream->readUint16LE() / 6;
	}

	_numSpheres = stream->readUint16LE();
	_spheres = new Sphere[_numSpheres];
	for (uint32 i = 0; i < _numSpheres; ++i) {
		Sphere *s = &_spheres[i];
		stream->readUint16LE();
		s->_colour = stream->readUint16LE();
		s->_vertex = stream->readUint16LE() / 6;
		s->_size = stream->readUint16LE() / 16384.0;
	}


	createHierarchy();
}

void Model::createHierarchy() {
	Common::sort(_polygons, &_polygons[_numPolygons]);

	_heirs = new Hierarchy[_numBones];

	for (uint32 i = 0; i < _numBones; ++i) {
		Bone *b = &_bones[i];
		Hierarchy *h =  &_heirs[i];
		h->_index = i;
		h->_vertex = &_verticies[b->_vertex];
		if (b->_parent == 0xffff) {
			_heir = h;
			continue;
		}
		Hierarchy *ph =  &_heirs[b->_parent];
		ph->_children.push_back(h);
	}

	recalculateHierarchy();
}

void Model::recalculateHierarchy() {
	Math::Matrix4 matrix;
	matrix.setToIdentity();

	_heir->computeWorldMatrix(matrix);
}

Math::Matrix4 Hierarchy::computeLocalMatrix() {
	Math::Matrix4 matrix;
	matrix.setToIdentity();
	if (_index == 0) {
		return matrix;
	}
	Math::Vector3d pos = _vertex->_pos;
	if (_isTransltion) {
		pos += _translation;
	} else {
		matrix.buildFromPitchYawRoll(_pitch, _yaw, _roll);
	}
	matrix.setPosition(pos);
	return matrix;
}

void Hierarchy::computeWorldMatrix(Math::Matrix4 parentMtx) {
	Math::Matrix4 localMtx = computeLocalMatrix();
	_worldMatrix = parentMtx * localMtx;
	int size = _children.size();
	for (int i = 0; i < size; ++i) {
		_children[i]->computeWorldMatrix(_worldMatrix);
	}
}

Math::Vector3d Vertex::getPos(Model *m) {
	if (_bone == 0) {
		return _pos;
	}
	Math::Vector4d mv(_pos.x(), _pos.y(), _pos.z(), 1);
	Hierarchy *h = &m->_heir[_bone];
	mv = h->_worldMatrix * mv;
	return Math::Vector3d(mv.getData());
}

bool Polygon::operator < (const Polygon &poly) const {
	if (!_hasTransparency && poly._hasTransparency) {
		return true;
	}
	return false;
}

} // end of namespace Twin
