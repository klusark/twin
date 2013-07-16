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

// Based on Twin-E code, Copyright (C) 2002-2010 The TwinE team

#include "common/stream.h"

#include "engines/twin/fla_decoder.h"
#include "engines/twin/gfx_opengl.h"

namespace Twin {

FlaDecoder::FlaDecoder() {
	_videoTrack = nullptr;
	_surface = nullptr;
}

FlaDecoder::~FlaDecoder() {
	delete _videoTrack;
	if (_surface) {
		_surface->free();
		delete _surface;
	}
}

bool FlaDecoder::loadStream(Common::SeekableReadStream *stream) {
	_stream = stream;

	stream->seek(6, SEEK_SET); // Version
	uint32 numFrames = stream->readUint32LE();
	byte fps = stream->readByte();
	stream->readByte();
	uint16 width = stream->readUint16LE();
	uint16 height = stream->readUint16LE();
	uint32 numSamples = stream->readUint16LE();
	stream->readUint16LE();

	// 4 * numSamples entries to skip.
	stream->seek(4 * numSamples, SEEK_CUR);

	_curFrame = 0;
	_videoTrack = new FlaVideoTrack(width, height, fps, numFrames);
	addTrack(_videoTrack);
	return true;
}

const Graphics::Surface *FlaDecoder::decodeNextFrame() {
	byte numOps = _stream->readByte();
	_stream->readByte();
	_stream->readUint32LE(); // Frame-data-size


	for (int i = 0; i < numOps; i++) {
		uint16 op = _stream->readUint16LE();
		uint16 blockSize = _stream->readUint16LE();
		uint32 nextPos = _stream->pos() + blockSize;

		if (op == 1) { // Palette-update
			_videoTrack->updatePalette(_stream);
		} else if (op == 3) { // Audio-op
			warning("TODO: Audio");
		} else if (op == 6) {
			_videoTrack->decodeUpdateFrame(_stream);
		} else if (op == 8) {
			_videoTrack->decodeFullFrame(_stream);
		} else {
			warning("Unknown opcode");
		}
		_stream->seek(nextPos, SEEK_SET);
	}
	_videoTrack->setCurFrame(_curFrame);
	_curFrame++;
	if (_surface) {
		_surface->free();
		delete _surface;
	}
	_surface = VideoDecoder::decodeNextFrame()->convertTo(Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24), _videoTrack->getPalette());
	return _surface;
}

FlaDecoder::FlaVideoTrack::FlaVideoTrack(int width, int height, int fps, int numFrames) {
	_format = Graphics::PixelFormat::createFormatCLUT8();
	_width = width;
	_height = height;
	_nbframes = numFrames;
	_frameRate = Common::Rational(fps, 1);
	_surface = new Graphics::Surface();
	_surface->create(_width, _height, _format);
	_palette = new byte [256 * 3];
}

FlaDecoder::FlaVideoTrack::~FlaVideoTrack() {
	delete[] _palette;
	if (_surface) {
		_surface->free();
		delete _surface;
	}
}

void FlaDecoder::FlaVideoTrack::updatePalette(Common::SeekableReadStream *stream) {
	uint16 palSize = stream->readUint16LE();
	uint16 offset = stream->readUint16LE();
	stream->read(_palette + offset * 3, 3 * palSize);
}

// Note: The RLE-count is inverted between updates and full frames.
void FlaDecoder::FlaVideoTrack::decodeFullFrame(Common::SeekableReadStream *stream) {
	for (int j = 0; j < _surface->h; j++) {
		byte *line = (byte *)_surface->getBasePtr(0, j);
		byte length = stream->readByte();
		for (int k = 0; k < length; k++) {
			char rleCount = stream->readByte();
			if (rleCount > 0) { // RLE
				byte pixel = stream->readByte();
				while (rleCount > 0) {
					*line = pixel;
					line++;
					rleCount--;
				}
			} else {
				stream->read(line, -rleCount);
				line -= rleCount;
			}
		}
	}
}

void FlaDecoder::FlaVideoTrack::decodeUpdateFrame(Common::SeekableReadStream *stream) {
	uint16 startPos = stream->readUint16LE();
	uint16 height = stream->readSint16LE();

	for (int j = startPos; j < startPos + height; j++) {
		byte *line = (byte *)_surface->getBasePtr(0, j);
		byte length = stream->readByte();
		for (int k = 0; k < length; k++) {
			byte skip = stream->readByte();
			line += skip;
			char rleCount = stream->readByte();
			if (rleCount < 0) { // RLE
				byte pixel = stream->readByte();
				while (rleCount < 0) {
					*line = pixel;
					line++;
					rleCount++;
				}
			} else {
				stream->read(line, rleCount);
				line += rleCount;
			}
		}
	}
}

const Graphics::Surface *FlaDecoder::FlaVideoTrack::decodeNextFrame() {
	return _surface;
}

} // end of namespace Twin
