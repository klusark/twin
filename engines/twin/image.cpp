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

#include "common/textconsole.h"

#include "graphics/surface.h"

#include "engines/twin/image.h"
#include "engines/twin/hqr.h"

namespace Twin {

Image::Image(const Common::String &resName, uint32 index, GfxOpenGL *renderer) : _renderer(renderer) {
	Hqr resFile;
	resFile.open(resName);
	Common::SeekableReadStream *imgStream = resFile.createReadStreamForIndex(index);
	Common::SeekableReadStream *palStream = resFile.createReadStreamForIndex(index + 1);

	if (imgStream->size() != 640 * 480) {
		error("Image size is not %d but %d", 640 * 480, imgStream->size());
	}

	Graphics::Surface image;
	image.create(640, 480, Graphics::PixelFormat::createFormatCLUT8());

	_palette = new byte[imgStream->size()]; // Should be RGB888

	imgStream->read(image.getBasePtr(0, 0), imgStream->size());
	palStream->read(_palette, palStream->size());

	_surface = image.convertTo(Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24), _palette);

	_texId = _renderer->createBitmap(_surface);

	delete imgStream;
	delete palStream;
	image.free();
}

Image::~Image() {
	_renderer->destroyBitmap(_texId);
	_surface->free();
	delete _surface;
	delete[] _palette;
}

void Image::display() {
	_renderer->drawBitmap(_texId, 0, 0, _surface->w , _surface->h);
}

} // end of namespace Twin