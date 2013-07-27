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

#if defined(WIN32) && !defined(__SYMBIAN32__)
#include <windows.h>
// winnt.h defines ARRAYSIZE, but we want our own one...
#undef ARRAYSIZE
#endif

#include "engines/twin/gfx_opengl.h"
#include "engines/twin/model.h"
#include "math/vector3d.h"
#include "common/textconsole.h"
#include "engines/twin/colour_palette.h"
#include "engines/twin/grid.h"
#include "engines/twin/resource.h"
#include "engines/twin/block_library.h"
#include "engines/twin/block.h"

#ifdef USE_OPENGL

#if defined (SDL_BACKEND) && defined(GL_ARB_fragment_program)

// We need SDL.h for SDL_GL_GetProcAddress.
#include "backends/platform/sdl/sdl-sys.h"

#endif

namespace Twin {

byte *GfxOpenGL::setupScreen(int screenW, int screenH, bool fullscreen) {
	_pixelFormat = g_system->setupScreen(screenW, screenH, fullscreen, true).getFormat();
	_screenWidth = screenW;
	_screenHeight = screenH;
	_scaleW = 1.0f;
	_scaleH = 1.0f;

	_isFullscreen = g_system->getFeatureState(OSystem::kFeatureFullscreenMode);

	g_system->showMouse(!fullscreen);

	char GLDriver[1024];
	sprintf(GLDriver, "ResidualVM: %s/%s", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	g_system->setWindowCaption(GLDriver);

	glViewport(0, 0, _screenWidth, _screenHeight);

	_cameraX = 0;
	_cameraY = 0;
	_cameraZ = 0;
	_rotX = 0;
	_rotY = 0;
	_rotZ = 0;

	return NULL;
}

void GfxOpenGL::clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GfxOpenGL::flipBuffer() {
	g_system->updateScreen();
}

int GfxOpenGL::createBitmap(const Graphics::Surface *surf) {
	// TODO: Care about NPOT-problems, and pixel-formats
	GLuint texNum;
	glGenTextures(1, &texNum);
	GLint format = GL_RGBA;
	GLint type = GL_UNSIGNED_BYTE;

	glBindTexture(GL_TEXTURE_2D, texNum);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, format, surf->w, surf->h, 0, format, type, surf->pixels);

	return texNum;
}

void GfxOpenGL::drawBitmap(int texID, int x, int y, int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, _screenWidth, _screenHeight, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texID);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(x * _scaleW, y * _scaleH);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f((x + w) * _scaleW, y * _scaleH);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f((x + w) * _scaleW, (y + h)  * _scaleH);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(x * _scaleW, (y + h) * _scaleH);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void GfxOpenGL::destroyBitmap(uint32 texID) {
	glDeleteTextures(1, &texID);
}

void GfxOpenGL::drawModel(Model *m) {
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)_screenWidth/(GLfloat)_screenHeight,0.01f,10.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4ub(255, 255, 255, 255);

	gluLookAt( 0, 0, 1, 0, 0, 0, 0, 1, 0 );
	glPushMatrix();

	glTranslatef((_cameraX/(float)_screenWidth), -(_cameraY/(float)_screenHeight), (_cameraZ/(float)_screenHeight));
	glRotatef(_rotY, 1, 0, 0);
	glRotatef(_rotX, 0, 1, 0);

	for (uint j = 0; j < m->_numPolygons; j++) {
		Polygon *p = &m->_polygons[j];
		glBegin(GL_POLYGON);

		for (int i = 0; i < m->_polygons->_num; ++i) {
			uint32 vert = p->_data[i];
			if (vert > m->_numVerticies || vert == 0) {
				break;
			}

			glColor4ub(_palette->_palette[p->_colour]._r, _palette->_palette[p->_colour]._g, _palette->_palette[p->_colour]._b, 255);
			Vertex *v = &m->_verticies[vert];

			if (v->_bone == 0) {
				continue;
			}
			Math::Vector3d mv = v->getPos(m);

			Normal *n = &m->_normals[vert];

			glNormal3f(n->_x, n->_y, n->_z);
			glVertex3fv(mv.getData());

		}

		glEnd();

	}

	for (uint j = 0; j < m->_numPoints; j++) {
		Point *p = &m->_points[j];
		glColor4ub(_palette->_palette[p->_colour]._r, _palette->_palette[p->_colour]._g, _palette->_palette[p->_colour]._b, 255);

		Vertex *v1 = &m->_verticies[p->_v1];
		Math::Vector3d vec1 = v1->getPos(m);
		Vertex *v2 = &m->_verticies[p->_v2];
		Math::Vector3d vec2 = v2->getPos(m);

		glBegin(GL_LINES);
		glVertex3fv(vec1.getData());
		glVertex3fv(vec2.getData());
		glEnd();
	}

	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
}

void GfxOpenGL::drawBlock(Block *block, int32 x, int32 y, int32 z) {
	int xb = x;
	int yb = y;
	int zb = z;
	x = (xb - zb) * 24 + 288;
	y = ((xb + zb) * 12) - (yb * 15) + 215;
	GLuint texNum;
	if (block->_renderData == NULL) {
		block->_renderData = new GLint;

		glGenTextures(1, &texNum);
		*(GLuint *)(block->_renderData) = texNum;
		GLint format = GL_RGBA;
		GLint type = GL_UNSIGNED_BYTE;

		byte *pixels = new byte[block->_width * block->_height * 4];
		for (int i = 0; i < block->_width * block->_height; ++i) {
			byte val = block->_data[i];
			pixels[i*4 + 0] = _palette->_palette[val]._r;
			pixels[i*4 + 1] = _palette->_palette[val]._g;
			pixels[i*4 + 2] = _palette->_palette[val]._b;
			if (val == 0) {
				pixels[i*4 + 3] = 0;
			} else {
				pixels[i*4 + 3] = 255;
			}
		}

		glBindTexture(GL_TEXTURE_2D, texNum);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, format, block->_width, block->_height, 0, format, type, pixels);
	}
	texNum = *(GLuint *)block->_renderData;

	int w = block->_width;
	int h = block->_height;

	x += block->_offsetX;
	y += block->_offsetY;

	if (x + _cameraX > _screenWidth || x + _cameraX < -48 || y + _cameraY > _screenHeight || y + _cameraY < -40) {
		return;
	}

	glColor4ub(255, 255, 255, 255);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, _screenWidth, _screenHeight, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);
	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);


	glPushMatrix();
	glTranslatef(_cameraX, _cameraY, 0.0f);

	glBindTexture(GL_TEXTURE_2D, texNum);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(x * _scaleW, y * _scaleH);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f((x + w) * _scaleW, y * _scaleH);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f((x + w) * _scaleW, (y + h)  * _scaleH);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(x * _scaleW, (y + h) * _scaleH);
	glEnd();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
}

void GfxOpenGL::drawGrid(Grid *g) {
	BlockLibrary *b = g_resource->getBlockLibrary(g->getLibrary());
	for (int z = 0; z < 64; z++) {
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 25; y++) {
				Square *s = g->getGridAt(x, y, z);
				if (s->_layout != 0) {
					BlockInfo *info = b->getBlock(s->_layout-1);
					uint32 brick = s->_brick;
					if (info->_numBlocks > brick && brick >= 0) {
						int index = info->_blocks[brick]._index - 1;
						if (index >= 0) {
							Block *gb = g_resource->getBlock(index);
							drawBlock(gb, x, y, z);
						}
					}
				}
			}
		}
	}


}

} // end of namespace Twin

#endif
