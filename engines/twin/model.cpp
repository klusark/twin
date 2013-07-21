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

	_numPolygon = stream->readUint32LE();
	uint32 polygonOffset = stream->readUint32LE();

	uint32 numUnknown2 = stream->readUint32LE();
	uint32 offsetUnknown2 = stream->readUint32LE();

	_numSphere = stream->readUint32LE();
	uint32 sphereOffset = stream->readUint32LE();

	uint32 numUnknown3 = stream->readUint32LE();
	uint32 offsetUnknown3 = stream->readUint32LE();

	stream->seek(bonesOffset);
	for (int i = 0; i < _numBones; ++i) {
		stream->readUint32LE();
		stream->readUint32LE();
	}

	for (int i = 0; i < _numVerticies; ++i) {
		stream->readUint32LE();
		stream->readUint32LE();
	}

	for (int i = 0; i < _numNormals; ++i) {
		stream->readUint32LE();
		stream->readUint32LE();
	}

	for (int i = 0; i < numUnknown1; ++i) {
		stream->readUint32LE();
		stream->readUint32LE();
	}

	for (int i = 0; i < _numPolygon; ++i) {
		// cant figure out how to read this data yet
	}

	stream->seek(offsetUnknown2);
	for (int i = 0; i < numUnknown2; ++i) {
		stream->readUint32LE();
		stream->readUint32LE();
	}

	for (int i = 0; i < _numSphere; ++i) {
		stream->readUint32LE();
		stream->readUint32LE();
	}

	for (int i = 0; i < numUnknown3; ++i) {
		stream->readUint32LE();
	}

}


} // end of namespace Twin
