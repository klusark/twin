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

} // end of namespace Twin

#endif
