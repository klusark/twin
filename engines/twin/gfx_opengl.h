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

#ifndef TWIN_GFX_OPENGL_H
#define TWIN_GFX_OPENGL_H

#include "common/system.h"

#ifdef USE_OPENGL

#if defined (SDL_BACKEND) && !defined(__amigaos4__)
#include <SDL_opengl.h>
#undef ARRAYSIZE
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "graphics/surface.h"
#include "graphics/pixelformat.h"
#include "graphics/pixelbuffer.h"

namespace Twin {

class Model;
class ColourPalette;

class GfxOpenGL {
	int _screenWidth;
	int _screenHeight;
	bool _isFullscreen;
	float _scaleW;
	float _scaleH;
	Graphics::PixelFormat _pixelFormat;
	ColourPalette *_palette;
public:
	GfxOpenGL() {}
	byte *setupScreen(int screenW, int screenH, bool fullscreen);
	void clearScreen();
	void flipBuffer();
	int createBitmap(const Graphics::Surface *surf);
	void drawBitmap(int texID, int x, int y, int w, int h);
	void destroyBitmap(uint32 texID);
	void drawModel(Model *m);
	void setColourPalette(ColourPalette *p) { _palette = p; }
};

} // end of namespace Twin

#endif // USE_OPENGL

#endif
