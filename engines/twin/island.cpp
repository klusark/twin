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
#include "common/array.h"

#include "engines/twin/island.h"
#include "engines/twin/twin.h"
#include "engines/twin/hqr.h"
#include "engines/twin/model.h"

namespace Twin {

/* This structure is used to map the triangle declarations in the ILE files */
struct GroundTriangle {
	byte textureBank;
	byte useTexture;
	byte useColor;
	byte unk1;
	byte unk2;
	byte orientation;
	byte unk3;
	byte unk4;
	uint16 textureIndex;
};

/* This structure maps the grid's quad declaration in the ILE files */
struct GroundSquare {
	GroundTriangle	triangle[2];
};

struct IslandUV {
	uint16 u;
	uint16 v;
};

/* This structure maps an UV declaration entry in the ILE files */
struct GroundTextureInfo {
	IslandUV uv[3];
};


Island::Island(Hqr *hqr, Hqr *obl) {
	_ile = hqr;
	_obl = obl;
	_drawList = NULL;
	loadIsland();
}

/*
	HQR file entries
		0 - sections layout (max of 256 sections)
		1 - ground textures
		2 - objects textures

		Island Sections (Num Entries HQR - 3 / 6 = Total Sections available)
*/
void Island::loadIsland() {

	_renderData = NULL;

	int32 entries = _ile->getNumIndices();
	_numSections = (entries - 3) / 6; // get the number of sections in this ILE file

	// 0 - layout sections
	Common::SeekableReadStream *stream = _ile->createReadStreamForIndex(0);
	_sectionsLayout = new byte[256];
	stream->read(_sectionsLayout, 256);
	delete stream;

	// 1 - ground texture
	stream = _ile->createReadStreamForIndex(1);
	_texture = new byte[256 * 256];
	stream->read(_texture, 256 *256);
	delete stream;

	// 2 - object texture
	//stream = hqr->createReadStreamForIndex(1);
	//delete stream;

	// TODO here we should also set Sea and Sky textures from Ress.HQR file (from idx 12 to 27)
	// We should figure out where the matching with this textures and each island are.
	_sections = new IslandSection[_numSections];

	for (int i = 0; i < 256; ++i) {
		byte c = _sectionsLayout[i];
		if (c != 0) {
			_sections[c - 1].x = i % 16;
			_sections[c - 1].y = i / 16;
		}
	}

	for (uint32 s = 0; s < _numSections; s++) {
		int32 i = (s * 6) + 3; // section index start
		loadIslandSection(s, i);
	}
}

/*
	Island Sections (Num Entries HQR - 3 / 6 = Total Sections available)
		3 (0) - Objects layout
		4 (1) - Objects data
		5 (2) - Quads (2 triangles)
		6 (3) - Texture UV mapping
		7 (4) - Height Maps
		8 (5) - Intensity
*/
void Island::loadIslandSection(int sectionIdx, int entryIdx) {
	IslandSection *section = &_sections[sectionIdx];
	Common::SeekableReadStream *stream = NULL;

	// 3 (0) - Objects layout
	stream = _ile->createReadStreamForIndex(entryIdx + 0);
	stream->seek(8);
	section->_numObjects = stream->readUint32LE();
	delete stream;


	// 4 (1) - Objects data
	if (section->_numObjects != 0) {
		section->_objects = new IslandObjectInfo[section->_numObjects];
		stream = _ile->createReadStreamForIndex(entryIdx + 1);
		for (uint32 i = 0; i < section->_numObjects; ++i) {
			IslandObjectInfo *o = &section->_objects[i];
			o->_index = stream->readUint32LE();
			float x, y, z;
			x = stream->readSint32LE() / 16384.0f;
			y = stream->readSint32LE() / 16384.0f;
			z = stream->readSint32LE() / 16384.0f;
			o->_pos.set(x, y, z);
			stream->seek(5, SEEK_CUR);
			o->_angle = stream->readByte();
			stream->seek(26, SEEK_CUR);
			o->_model = new Model(_obl->createReadStreamForIndex(o->_index));
		}
		delete stream;
	}

	// 7 (4) - Height Maps
	int16 heightmap[65][65];
	stream = _ile->createReadStreamForIndex(entryIdx + 4);
	_sectionsLayout = new byte[256];
	stream->read(heightmap, sizeof(heightmap));
	delete stream;

	// 6 (3) - Texture UV mapping
	GroundTextureInfo *textureInfo;
	stream = _ile->createReadStreamForIndex(entryIdx + 3);
	int numGroundTextreInfos = stream->size() / 12;
	textureInfo = new GroundTextureInfo[numGroundTextreInfos];
	for (int i = 0; i < numGroundTextreInfos; ++i) {
		GroundTextureInfo *t = &textureInfo[i];
		for (int j = 0; j < 3; ++j) {
			t->uv[j].u = stream->readUint16LE();
			t->uv[j].v = stream->readUint16LE();
		}
	}
	delete stream;

	// 8 (5) - Intensity
	byte intensity[65][65];
	stream = _ile->createReadStreamForIndex(entryIdx + 5);
	stream->read(intensity, sizeof(intensity));
	delete stream;

	// 5 (2) - Quads (2 triangles)
	GroundSquare squares[64][64];
	stream = _ile->createReadStreamForIndex(entryIdx + 2);
	for (int y = 0; y < 64; ++y) {
		for (int x = 0; x < 64; ++x) {
			for (int i = 0; i < 2; ++i) {
				GroundTriangle *g = &squares[y][x].triangle[i];
				byte a = stream->readByte();
				byte b = stream->readByte();
				byte c = stream->readByte();
				byte d = stream->readByte();
				g->textureBank =  (a & 0x0F) >> 0;
				g->useTexture =   (a & 0x30) >> 4;
				g->useColor =     (a & 0xC0) >> 6;
				g->unk1 =         (b & 0x0F) >> 0;
				g->unk2 =         (b & 0xF0) >> 4;
				g->orientation =  (c & 0x01) >> 0;
				g->unk3 =         (c & 0x02) >> 1;
				g->unk4 =         (c & 0x04) >> 2;
				g->textureIndex = d;
				g->textureIndex <<= 5;
				g->textureIndex |= ((c & 0xF8) >> 3);
			}
		}
	}
	delete stream;


	// Vertex Count - In order of verts that make tris
	uint16  idx = 0;
	uint16 uvOrder[6] = {0, 1, 2, 2, 0, 1};
	uint16 idxOrder1[6] = {0, 2, 3, 0, 3, 1};
	uint16 idxOrder2[6] = {0, 2, 1, 1, 2, 3};

	section->_numFaces = 64 * 64 * 2;
	section->_faces = new IslandFace[section->_numFaces];

	// For Every QUAD
	for (int32 x = 0; x < 64; ++x) {
		for (int32 y = 0; y < 64; ++y) {
			// Pass the pointer from the quad database to this quad (2 triangles passed)
			GroundTriangle *tri = squares[x][y].triangle;
			// Orientation
			uint16 *idxOrder = (tri[0].orientation == 0) ? idxOrder1 : idxOrder2;

			// For both tris in this quad...
			for (int32 t = 0; t < 2; ++t) {
				IslandFace *f = &section->_faces[idx];
				f->_hasTexture = tri[t].useTexture;
				f->_hasColour = tri[t].useColor;
				++idx;
				int j = 0;
				// For each vertex, offset by triangle num in quad
				for (int32 i = 3 * t; i < (3 + 3 * t); ++i) {
					int32 xi = x + idxOrder[i] / 2;
					int32 yi = y + idxOrder[i] % 2;
					IslandVertex *v = &f->_vertices[j];
					++j;
					float x = ((float)(xi) - 32) / 0x20;
					float y = ((float)(heightmap[xi][yi])) / 0x4000;
					float z = ((float)(64 - yi) - 32) / 0x20;
					v->_pos.set(x, y, z);


					v->r = 1;
					v->g = 1;
					v->b = 1;
					v->a = 1.0;
					// Vertex color and UV
					if (f->_hasTexture) {
						float intens = intensity[xi][yi] / 16.0f + 0.3;

						if (intens > 1.0)
							intens = 1.0;

						v->r = intens;
						v->g = intens;
						v->b = intens;

						v->u = textureInfo[tri[t].textureIndex].uv[uvOrder[i]].u / 65535.0f;
						v->v = textureInfo[tri[t].textureIndex].uv[uvOrder[i]].v / 65535.0f;

					}
					if (f->_hasColour) {
						byte colorIdx = (tri[t].textureBank * 16) + intensity[xi][yi];
						v->_colour = colorIdx;

					}
				}
			}
		}

	}

	delete [] textureInfo;
}

} // end of namespace Twin
