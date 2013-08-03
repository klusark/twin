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

#include "video_player_smk.h"
#include "hqr.h"


namespace Twin {


VideoPlayerSmacker::VideoPlayerSmacker(GfxBase *renderer, Audio::Mixer *mixer) : VideoPlayer(renderer, mixer) {
	this->_smkDecoder = new Video::SmackerDecoder();
}


VideoPlayerSmacker::~VideoPlayerSmacker() {
	delete this->_smkDecoder;
}

void VideoPlayerSmacker::play(int index) {
	Hqr resFile;
	resFile.open("VIDEO/VIDEO.HQR");
	Common::SeekableReadStream *smkStream = resFile.createReadStreamForIndex(index);
	play(smkStream);
	delete smkStream;
}

void VideoPlayerSmacker::play(Common::SeekableReadStream *stream) {
	_smkDecoder->loadStream(stream);
	_smkDecoder->start();

	// TODO choose the right Audio Channel (some movies have different tracks that must be played)
	
	const byte* palette = _smkDecoder->getPalette();

	int32 w = _renderer->getScreenWidth();
	int32 h = (200 * w) / 320; // FIXME put this values in constants (frame size)
	int32 x = 0;
	int32 y = (_renderer->getScreenHeight() - h) / 2;

	bool done = false;
	Common::Event event;

	while (!done && !_smkDecoder->endOfVideo()) {
		// TODO find a better way to skip this
		while (g_system->getEventManager()->pollEvent(event)) {
			Common::EventType type = event.type;
			if (type == Common::EVENT_KEYDOWN || type == Common::EVENT_KEYUP) {
				if (event.kbd.keycode == Common::KEYCODE_ESCAPE) {
					done = true;
				}
			} 
		}

		if (_smkDecoder->needsUpdate()) {
			const Graphics::Surface *frame = _smkDecoder->decodeNextFrame();

			if (!frame) {
				continue;
			}

			if (_smkDecoder->hasDirtyPalette()) {
				setupPalette();
			}

			// TODO update frame to screen
			//((byte *)frame->pixels, frame->pitch, x, y, frame->w, frame->h)

			g_system->updateScreen();
		}
		g_system->delayMillis(10);
	}

	_smkDecoder->close();
}

void VideoPlayerSmacker::setupPalette() {
	// TODO setup _smkDecoder->getPalette()
}

} // end of namespace Twin
