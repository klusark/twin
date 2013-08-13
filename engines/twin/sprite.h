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

#ifndef TWIN_SPRITE_H
#define TWIN_SPRITE_H


namespace Common {
class SeekableReadStream;
}

namespace Twin {

class SpriteInfo {
public:
	int16 _x;
	int16 _y;
	int16 _x1;
	int16 _x2;
	int16 _y1;
	int16 _y2;
	int16 _z1;
	int16 _z2;
};

class Sprite {
public:
	Sprite(Common::SeekableReadStream *stream);
	~Sprite();
	void setSpriteInfo(SpriteInfo *info) { _info = info; }
//private:
	void loadLBA2(Common::SeekableReadStream *stream);

	byte _offsetX;
	byte _offsetY;
	byte _width;
	byte _height;
	byte *_data;

	SpriteInfo *_info;

	void *_renderData;

};

} // end of namespace Twin

#endif
