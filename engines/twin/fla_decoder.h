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

#ifndef TWIN_FLA_DECODER_H
#define TWIN_FLA_DECODER_H

#include "common/str.h"

#include "video/video_decoder.h"

#include "graphics/surface.h"

namespace Twin {

class GfxOpenGL;

class FlaDecoder : public Video::VideoDecoder {
	Graphics::Surface *_surface;
	uint32 _curFrame;
	Common::SeekableReadStream *_stream;
public:
	FlaDecoder();
	virtual ~FlaDecoder();

	bool loadStream(Common::SeekableReadStream *stream);

	void open(const Common::String &filename);
	const Graphics::Surface *decodeNextFrame();

	// TODO: We don't have audio yet:
	virtual bool useAudioSync() const {
		return false;
	}
protected:
	class FlaVideoTrack : public FixedRateVideoTrack {
	public:
		FlaVideoTrack(int width, int height, int fps, int numFrames);
		virtual ~FlaVideoTrack();

		void updatePalette(Common::SeekableReadStream *stream);
		void decodeFullFrame(Common::SeekableReadStream *stream);
		void decodeUpdateFrame(Common::SeekableReadStream *stream);
		const byte *getPalette() const {
			return _palette;
		}

		uint16 getWidth() const { return _width; }
		uint16 getHeight() const {return _height; }
		Graphics::PixelFormat getPixelFormat() const { return _format; }
		int getCurFrame() const { return _curFrame; }
		void setCurFrame(int frame) { _curFrame = frame; }
		int getFrameCount() const {	return _nbframes; }
		Common::Rational getFrameRate() const {	return _frameRate; }

		const Graphics::Surface *decodeNextFrame();
	private:
		byte *_palette;
		int32 _curFrame;
		int _width, _height;
		Graphics::Surface *_surface;
		Graphics::PixelFormat _format;
		Common::Rational _frameRate;
		int32 _nbframes;
	};
private:
	FlaVideoTrack *_videoTrack;
};

} // end of namespace Twin

#endif