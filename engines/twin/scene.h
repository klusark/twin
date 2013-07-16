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

#ifndef TWIN_SCENE_H
#define TWIN_SCENE_H


namespace Common {
class SeekableReadStream;
}

namespace Twin {


class Scene {
public:
	Scene(Common::SeekableReadStream *stream);
private:
	void loadLBA2(Common::SeekableReadStream *stream);

	class SceneActor {
	public:
		uint16 _entity;
		byte _body;
		byte _anim;
		uint16 _sprite;
		uint16 _x;
		uint16 _y;
		uint16 _z;
		uint16 _moveScriptSize;
		uint8 *_moveScript;

		uint16 _lifeScriptSize;
		uint8 *_lifeScript;
	};
	byte _textBank;
	byte _gameOverScene;
	uint16 _ambience[4];
	uint16 _repeat[4];
	uint16 _round[4];

	uint16 _heroX;
	uint16 _heroY;
	uint16 _heroZ;

	uint16 _moveScriptSize;
	uint8 *_moveScript;

	uint16 _lifeScriptSize;
	uint8 *_lifeScript;

	uint16 _numActors;
	SceneActor *_actors;

	uint16 _numZones;

};

} // end of namespace Twin

#endif
