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

#ifndef TWIN_ANIMATION_H
#define TWIN_ANIMATION_H


namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Boneframe {
public:
	uint16 _type;
	int16 _x;
	int16 _y;
	int16 _z;
};

class Keyframe {
public:
	uint16 _length;
	int16 _x;
	int16 _y;
	int16 _z;
	Boneframe *_bones;
};

class Animation {
public:
	Animation(Common::SeekableReadStream *stream);
	~Animation();
private:
	void loadLBA2(Common::SeekableReadStream *stream);

	uint16 _numKeys;
	uint16 _numBones;
	Keyframe *_keyframes;
};

} // end of namespace Twin

#endif
