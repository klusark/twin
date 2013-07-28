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
class Grid;
class Block;
class Texture;
class Island;

class GfxOpenGL {
	int _screenWidth;
	int _screenHeight;
	bool _isFullscreen;
	float _scaleW;
	float _scaleH;
	Graphics::PixelFormat _pixelFormat;
	ColourPalette *_palette;
	int _cameraX;
	int _cameraY;
	int _cameraZ;

	int _rotX;
	int _rotY;
	int _rotZ;

	byte *_modelPixels;
	uint32 _modelMaterial;

public:
	GfxOpenGL() {}
	byte *setupScreen(int screenW, int screenH, bool fullscreen);
	void clearScreen();
	void flipBuffer();
	int createBitmap(const Graphics::Surface *surf);
	void drawBitmap(int texID, int x, int y, int w, int h);
	void destroyBitmap(uint32 texID);
	void drawModel(Model *m);
	void drawBlock(Block *block, int32 x, int32 y, int32 z);
	void drawGrid(Grid *g);
	void drawSphere(double radius, int slices, int stacks);
	void drawIsland(Island *island);
	void setColourPalette(ColourPalette *p) { _palette = p; }
	void moveCamera(int x, int y, int z) { _cameraX += x; _cameraY += y; _cameraZ += z; }
	void rotateObject(int x, int y, int z) { _rotX += x; _rotY += y; _rotZ += z; }
	void loadModelTexture(Common::SeekableReadStream *stream);

private:
	void bindTexture(Texture *t);
	void loadTexture(byte *buf, uint32 *texId, byte **tex, uint32 width, uint32 height);


};

} // end of namespace Twin

#endif // USE_OPENGL

#endif
