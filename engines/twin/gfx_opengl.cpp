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

#include "math/vector3d.h"
#include "common/textconsole.h"

#include "engines/twin/gfx_opengl.h"
#include "engines/twin/model.h"
#include "engines/twin/colour_palette.h"
#include "engines/twin/grid.h"
#include "engines/twin/resource.h"
#include "engines/twin/block_library.h"
#include "engines/twin/block.h"
#include "engines/twin/island.h"
#include "engines/twin/actor.h"

#ifdef USE_OPENGL

#if defined (SDL_BACKEND) && defined(GL_ARB_fragment_program)

// We need SDL.h for SDL_GL_GetProcAddress.
#include "backends/platform/sdl/sdl-sys.h"

#endif

namespace Twin {

GfxOpenGL::~GfxOpenGL() {
	glDeleteTextures(1, &_modelMaterial);
	delete[] _modelPixels;
}

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
	_modelPixels = NULL;

	return NULL;
}

void GfxOpenGL::clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GfxOpenGL::flipBuffer() {
	g_system->updateScreen();
}

int GfxOpenGL::getScreenWidth() {
	return _screenWidth;
}

int GfxOpenGL::getScreenHeight() {
	return _screenHeight;
}

void GfxOpenGL::moveCamera(int x, int y, int z) {

	_cameraZ = 0;
	_cameraX = -(((x - z) * 24.0f) / 512.0f);
	_cameraY = -((((x + z) * 12.0f) - y * 30.0f) / 512.0f);
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

void GfxOpenGL::bindTexture(Texture *t) {
	if (t->_renderData == NULL) {
		if (t->_x == 0 && t->_y == 0 && t->_w == 255 && t->_h == 255) {
			t->_renderData = &_modelMaterial;
		} else {
			GLuint *texNum = new GLuint;
			GLint format = GL_RGBA;
			GLint type = GL_UNSIGNED_BYTE;
			glGenTextures(1, texNum);
			glBindTexture(GL_TEXTURE_2D, *texNum);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 256);
			glTexImage2D(GL_TEXTURE_2D, 0, format, t->_w + 1, t->_h + 1, 0, format, type, _modelPixels + t->_x * 4 + t->_y * 4 * 256);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			t->_renderData = texNum;
		}
	}

	GLuint texNum = *(GLuint *)t->_renderData;
	glBindTexture(GL_TEXTURE_2D, texNum);
}

void GfxOpenGL::loadModelTexture(Common::SeekableReadStream *stream) {
	byte *data = new byte[256 * 256];
	stream->read(data, 256 * 256);
	delete stream;

	loadTexture(data, (uint32 *)&_modelMaterial, &_modelPixels, 256, 256);
}

void GfxOpenGL::drawActor(Actor *a) {
	float orthoProjX = 320;
	float orthoProjY = 240;
	float projPosX = ((a->_x - a->_z) * 24.0f) / 512.0f + orthoProjX;
	float projPosY = (((a->_x + a->_z) * 12.0f) - a->_y * 30.0f) / 512.0f + orthoProjY;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, -1, 0, 0.1, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt( 0, 0, 1, 0, 0, 0, 0, 1, 0 );

	glTranslatef((_cameraX/(float)_screenWidth), -(_cameraY/(float)_screenHeight), (_cameraZ/(float)_screenHeight));
	glTranslatef(projPosX/(float)_screenWidth, -(projPosY/(float)_screenHeight ), 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glRotatef(45, 1, 0, 0);
	glRotatef(45, 0, 1, 0);
	glRotatef(-a->_angle.getDegrees(), 0, 1, 0);
	drawModel(a->_entity->_model, true);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void GfxOpenGL::freeModel(Model *m) {
	for (uint32 i = 0; i < m->_numTextures; ++i) {
		Texture *t = &m->_textures[i];
		if (t->_renderData != nullptr) {
			GLuint texid = *(GLuint *)t->_renderData;
			if (texid != _modelMaterial) {
				glDeleteTextures(1, &texid);
			}
			t->_renderData = nullptr;
		}
	}
}

void GfxOpenGL::drawModel(Model *m, bool fromIsland) {
	if (!fromIsland) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

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
	}

	for (uint j = 0; j < m->_numSpheres; ++j) {
		Sphere *s = &m->_spheres[j];
		Vertex *v = &m->_verticies[s->_vertex];
		Math::Vector3d vec = v->getPos(m);

		glColor4ub(_palette->_palette[s->_colour]._r, _palette->_palette[s->_colour]._g, _palette->_palette[s->_colour]._b, 255);

		glPushMatrix();
		glTranslatef(vec.x(), vec.y(), vec.z());
		drawSphere(s->_size, 8, 8);
		glPopMatrix();
	}

	for (uint j = 0; j < m->_numLines; ++j) {
		Line *p = &m->_lines[j];
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

	for (uint j = 0; j < m->_numPolygons; j++) {
		Polygon *p = &m->_polygons[j];
		if (p->_hasTex) {
			bindTexture(&m->_textures[p->_tex]);
			glEnable(GL_TEXTURE_2D);
		}

		glBegin(GL_POLYGON);

		for (int i = 0; i < 4; ++i) {
			uint32 vert = p->_data[i];
			if (vert > m->_numVerticies || vert == 0) {
				continue;
			}

			Vertex *v = &m->_verticies[vert];
			Math::Vector3d mv = v->getPos(m);

			Normal *n = &m->_normals[vert];
			if (p->_hasTex) {
				Texture *t = &m->_textures[p->_tex];
				float x = p->_texX[i];
				float y = p->_texY[i];
				x /= (t->_w + 1);
				y /= (t->_h + 1);
				glTexCoord2f(x, y);
				glColor4ub(255, 255, 255, 255);
			} else {
				byte a = 255;
				if (p->_hasTransparency) {
					a = 126;
				}
				glColor4ub(_palette->_palette[p->_colour]._r, _palette->_palette[p->_colour]._g, _palette->_palette[p->_colour]._b, a);
			}
			glNormal3f(n->_x, n->_y, n->_z);
			glVertex3fv(mv.getData());
		}

		glEnd();
		if (p->_hasTex) {
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_BLEND);
		}
	}

	if (!fromIsland) {
		glPopMatrix();
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}
}

void GfxOpenGL::drawBlock(Block *block, int32 x, int32 y, int32 z) {
	int xb = x;
	int yb = y;
	int zb = z;
	x = (xb - zb) * 24 + 288;
	y = ((xb + zb) * 12) - (yb * 15) + 215;

	if (block->_renderData == NULL) {
		block->_renderData = new GLuint;
		loadTexture(block->_data, (uint32 *)block->_renderData, NULL, block->_width, block->_height);
	}
	GLuint texNum = *(GLuint *)block->_renderData;

	int w = block->_width;
	int h = block->_height;

	x += block->_offsetX;
	y += block->_offsetY;

	if (x + _cameraX > _screenWidth || x + _cameraX < -48 || y + _cameraY > _screenHeight || y + _cameraY < -40) {
		return;
	}


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

}

void GfxOpenGL::drawGrid(Grid *g) {
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
	BlockLibrary *b = g_resource->getBlockLibrary(g->getLibrary());
	for (int z = 0; z < 64; z++) {
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 25; y++) {
				Square *s = g->getGridAt(x, y, z);
				if (s->_layout != 0) {
					BlockInfo *info = b->getBlock(s->_layout-1);
					uint32 brick = s->_brick;
					if (info->_numBlocks > brick) {
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
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
}

void GfxOpenGL::drawSphere(double radius, int slices, int stacks) {
	GLUquadricObj* Sphere = gluNewQuadric();
	gluQuadricNormals(Sphere, GLU_SMOOTH);
	gluQuadricDrawStyle(Sphere, GLU_FILL);
	gluSphere(Sphere, radius, slices, stacks);
	gluDeleteQuadric(Sphere);
}

void GfxOpenGL::drawIsland(Island *island) {
	if (island->_renderData == NULL) {
		island->_renderData = new GLuint;
		loadTexture(island->_texture, (GLuint *)island->_renderData, NULL, 256, 256);
	}

	GLuint &texid = *(GLuint *)island->_renderData;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)_screenWidth/(GLfloat)_screenHeight,0.01f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4ub(255, 255, 255, 255);

	gluLookAt( 0, 0, 1, 0, 0, 0, 0, 1, 0 );
	glPushMatrix();

	glTranslatef((_cameraX/(float)_screenWidth), -(_cameraY/(float)_screenHeight), (_cameraZ/(float)_screenHeight));
	glRotatef(_rotY, 1, 0, 0);
	glRotatef(_rotX, 0, 1, 0);


	glBindTexture(GL_TEXTURE_2D, texid);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);
	glDisable(GL_LIGHTING);

	if (!island->_drawList) {
		GLuint *list = new GLuint;
		*list = glGenLists(1);
		island->_drawList = list;

		glNewList(*list, GL_COMPILE);
		//glTranslatef(-8, -8, 0);
		for (uint32 i = 0; i < island->_numSections; ++i) {
			IslandSection *s = &island->_sections[i];
			glPushMatrix();
			glTranslatef((s->y- 7) * 2, 0, (-(s->x - 7)) * 2);
			glBegin(GL_TRIANGLES);
			for (int j = 0; j < s->_numFaces; ++j) {
				IslandFace *f = &s->_faces[j];
				if (f->_hasColour) {
					for (int k = 0; k < 3; ++k) {
						IslandVertex *v = &f->_vertices[k];

						glColor4ub(_palette->_palette[v->_colour]._r, _palette->_palette[v->_colour]._g, _palette->_palette[v->_colour]._b, 255);
						glVertex3fv(v->_pos.getData());
					}
				}
			}
			glEnd();
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_TRIANGLES);
			for (int j = 0; j < s->_numFaces; ++j) {
				IslandFace *f = &s->_faces[j];

				if (f->_hasTexture) {
					for (int k = 0; k < 3; ++k) {
						IslandVertex *v = &f->_vertices[k];
						glColor4ub(v->r * 255, v->g * 255, v->b * 255, v->a *255);
						glTexCoord2f(v->u, v->v);
						glVertex3fv(v->_pos.getData());
					}

				}
			}
			glEnd();
			glDisable(GL_TEXTURE_2D);

			for (uint32 k = 0; k < s->_numObjects; ++k) {
				IslandObjectInfo *info = &s->_objects[k];
				glPushMatrix();

				glTranslatef(info->_pos.z() - 1, info->_pos.y(), -(info->_pos.x() - 1));
				float a = ((info->_angle + 4) / 4.0f) * 90;
				glRotatef(a, 0, 1, 0);
				drawModel(info->_model, true);
				glPopMatrix();
			}
			glPopMatrix();

		}
		glEndList();
	}
	GLuint list = *(GLuint *)island->_drawList;
	glCallList(list);
	glPopMatrix();
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	
}

void GfxOpenGL::loadTexture(byte *buf, uint32 *texId, byte **tex, uint32 width, uint32 height) {
	GLuint texNum;
	glGenTextures(1, &texNum);
	*(GLuint *)(texId) = texNum;
	GLint format = GL_RGBA;
	GLint type = GL_UNSIGNED_BYTE;

	byte *pixels = new byte[width * height * 4];
	for (uint32 i = 0; i < width * height; ++i) {
		byte val = buf[i];
		pixels[i*4 + 0] = _palette->_palette[val]._r;
		pixels[i*4 + 1] = _palette->_palette[val]._g;
		pixels[i*4 + 2] = _palette->_palette[val]._b;
		pixels[i*4 + 3] = _palette->_palette[val]._a;
	}

	glBindTexture(GL_TEXTURE_2D, texNum);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, pixels);
	if (tex == NULL) {
		//delete pixels;
	} else {
		*tex = pixels;
	}
}

} // end of namespace Twin

#endif
