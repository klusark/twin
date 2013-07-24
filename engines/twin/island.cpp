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

namespace Twin {

/* This structure is used to map the triangle declarations in the ILE files */
struct GroundTriangle {
	uint32 textureBank : 4;
	uint32 useTexture : 2;
	uint32 useColor : 2;
	uint32 unk1 : 4 ;
	uint32 unk2 : 4;
	uint32 orientation : 1;
	uint32 unk3 : 1;
	uint32 unk4 : 1;
	uint32 textureIndex : 13;
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

/* This structure maps an object positioning, index in OBL, and rotation data entry in the ILE files */
struct IslandObjectInfo {
	uint32 index;
	int32 ox;
	int32 oy;
	int32 oz;
	uint16 a5;
	uint16 a5bis;
	byte a6;
	byte angle;
	uint16 a6bis;
	uint16 a7;
	uint16 a7bis;
	uint16 a8;
	uint16 a8bis;
	uint16 a9;
	uint16 a9bis;
	uint16 a10;
	uint16 a10bis;
	uint16 a11;
	uint16 a11bis;
	uint16 a12;
	uint16 a12bis;
};

/* This structure maps an object header in the OBL files */
struct IslandObjectHeader {
	byte	unk0[0x44];
	uint32	polygonSectionOffset;
	uint32	lineSectionSize;
	uint32	lineSectionOffset;
	uint32	sphereSectionSize;
	uint32	sphereSectionOffset;
	uint32	cutSectionSize;
	uint32	cutSectionOffset;
	uint32	unk1;
	uint16	numVerticesType1;
	uint16	numVerticesType2;
};

struct OBLVertex {
	int16	x;
	int16	y;
	int16	z;
	int16	unk0;
};

struct OBLIntensity {
	byte	i[8];
};

struct OBLPolygonHeader {
	byte	id;
	byte	type;
	uint16	numPolygon;
	uint16	size;
	byte	unk0;
	byte	unk1;
};

struct OBLPolygon {
	uint16		idx[4];
	byte		color;
	byte		unk0[3];
	IslandUV	uv[4];	
};

Island::Island(Hqr *hqr) {
	_ile = hqr;
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
	int32 entries = _ile->getNumIndices();
	int32 sections = (entries - 3) / 6; // get the number of sections in this ILE file

	// 0 - layout sections
	Common::SeekableReadStream *stream = _ile->createReadStreamForIndex(0);
	_sectionsLayout = new byte[256];
	stream->read(_sectionsLayout, 256);
	delete stream;

	// 1 - ground texture
	//stream = hqr->createReadStreamForIndex(1);
	//delete stream;

	// 2 - object texture
	//stream = hqr->createReadStreamForIndex(1);
	//delete stream;

	// TODO here we should also set Sea and Sky textures from Ress.HQR file (from idx 12 to 27)
	// We should figure out where the matching with this textures and each island are.

	for (int32 s = 0; s < sections; s++) {
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
	// 7 (4) - Height Maps
	int16 heightmap[65][65];
	Common::SeekableReadStream *stream = _ile->createReadStreamForIndex(entryIdx + 4);
	_sectionsLayout = new byte[256];
	stream->read(heightmap, 65*65);
	delete stream;

	// 6 (3) - Texture UV mapping
	GroundTextureInfo *textureInfo;
	stream = _ile->createReadStreamForIndex(entryIdx + 3);
	textureInfo = new GroundTextureInfo[stream->size() / 12];
	stream->read(textureInfo, 65*65);
	delete stream;

	// 8 (5) - Intensity
	byte intensity[65][65];
	stream = _ile->createReadStreamForIndex(entryIdx + 5);
	stream->read(intensity, 65*65);
	delete stream;

	// 5 (2) - Quads (2 triangles)
	GroundSquare squares[64][64];
	stream = _ile->createReadStreamForIndex(entryIdx + 2);
	stream->read(squares, 64 * 64 * 8);
	delete stream;

	// Parse data
	int16 maxHeight = 0;
	
	Common::Array<float> vertices;
	vertices.reserve(9 * 64 * 64 * 4 * 2);
	
	Common::Array<uint16> faces;
	faces.reserve(6 * 64 * 64);

	// Vertex Count - In order of verts that make tris
	uint16  idx = 0;
	uint16 uvOrder[6] = {0, 1, 2, 2, 0, 1};
	uint16 idxOrder1[6] = {0, 2, 3, 0, 3, 1};
	uint16 idxOrder2[6] = {0, 2, 1, 1, 2, 3};

	// For Every QUAD
	for (int32 x = 0; x < 64; ++x)
	{
		for (int32 y = 0; y < 64; ++y)
		{
			// Pass the pointer from the quad database to this quad (2 triangles passed)
			GroundTriangle *tri = squares[x][y].triangle;
			// Orientation
			uint16 *idxOrder = (tri[0].orientation == 0) ? idxOrder1 : idxOrder2;

			// For both tris in this quad...
			for (int32 t = 0; t < 2; ++t)
			{
				// Data Usage for this Tri
				int mdMax = 0;

				if (tri[t].useColor)
					mdMax++;
				if (tri[t].useTexture)
					mdMax++;

				// For all the data usage in this tri
				for (int32 md = 0; md < mdMax; ++md)
				{
					// For each vertex, offset by triangle num in quad
					for (int32 i = 3 * t; i < (3 + 3 * t); ++i)
					{
						int32 xi = x + idxOrder[i] / 2;
						int32 yi = y + idxOrder[i] % 2;

						// Vertex position
						vertices.push_back(((float)(xi) - 32) / 0x20);
						vertices.push_back(((float)(heightmap[xi][yi])) / 0x4000);
						vertices.push_back(((float)(64 - yi) - 32) / 0x20);

						if (heightmap[xi][yi] > maxHeight)
							maxHeight = heightmap[xi][yi];

						// Vertex color and UV
						if (tri[t].useTexture && (mdMax < 2 || md == 1))
						{
							float intens = intensity[xi][yi] / 16.0f + 0.3;
							
							if (intens > 1.0)
								intens = 1.0;

							vertices.push_back(intens);
							vertices.push_back(intens);
							vertices.push_back(intens);
							vertices.push_back(1.0);
							vertices.push_back((textureInfo[tri[t].textureIndex].uv[uvOrder[i]].u / 65535.0f) * 0.5);
							vertices.push_back(textureInfo[tri[t].textureIndex].uv[uvOrder[i]].v / 65535.0f);

						}
						else if (mdMax < 2 || md == 0)
						{
							byte colorIdx = (tri[t].textureBank * 16) + intensity[xi][yi];
							
							// TODO get palette RGB components for colorIdx
							float r = 0;
							float g = 0;
							float b = 0;

							vertices.push_back(r);
							vertices.push_back(g);
							vertices.push_back(b);
							vertices.push_back(1.0);
							vertices.push_back(0.75f);
							vertices.push_back(0.5f);
						}
					}

					/* Index */
					faces.push_back(idx);
					faces.push_back(idx + 1);
					faces.push_back(idx + 2);
					idx += 3;
				}
			}
		}

	}

	delete [] textureInfo;
}

} // end of namespace Twin
