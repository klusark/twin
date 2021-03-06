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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef GRAPHICS_FRAMEBUFFER_H
#define GRAPHICS_FRAMEBUFFER_H

#include "graphics/opengles2/system_headers.h"

namespace Graphics {

class FrameBuffer {
public:
	FrameBuffer(uint width, uint height);
	FrameBuffer(GLuint texture_name, uint width, uint height, uint texture_width, uint texture_height);
#ifdef AMIGAOS
	~FrameBuffer() {}

	void attach() {}
	void detach() {}
#else
	~FrameBuffer();

	void attach();
	void detach();
#endif

	GLuint getColorTextureName() const { return _colorTexture; }
	uint getWidth() const { return _width; }
	uint getHeight() const { return _height; }
	uint getTexWidth() const { return _texWidth; }
	uint getTexHeight() const { return _texHeight; }

private:
	void init();
	bool _managedTexture;
	GLuint _colorTexture;
	GLuint _renderBuffers[2];
	GLuint _frameBuffer;
	uint _width, _height;
	uint _texWidth, _texHeight;
};

}

#endif
