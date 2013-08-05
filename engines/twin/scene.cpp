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

#include "common/stream.h"

#include "engines/twin/actor.h"
#include "engines/twin/scene.h"
#include "engines/twin/twin.h"
#include "engines/twin/script_track_v2.h"
#include "engines/twin/script_life_v2.h"

namespace Twin {

Scene::Scene(Common::SeekableReadStream *stream) : _grid(nullptr) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	}
}


void Scene::loadLBA2(Common::SeekableReadStream *stream) {
	stream->readByte();
	_textBank = stream->readByte();
	_gameOverScene = stream->readByte();
	stream->readUint32LE();

	stream->readUint16LE();
	stream->readUint16LE();

	_ambience[0] = stream->readUint16LE();
	_repeat[0] = stream->readUint16LE();
	_round[0] = stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();

	_ambience[1] = stream->readUint16LE();
	_repeat[1] = stream->readUint16LE();
	_round[1] = stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();

	_ambience[2] = stream->readUint16LE();
	_repeat[2] = stream->readUint16LE();
	_round[2] = stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();

	_ambience[3] = stream->readUint16LE();
	_repeat[3] = stream->readUint16LE();
	_round[3] = stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();


	stream->readUint16LE();
	stream->readUint16LE();

	stream->readByte();

	_heroX = stream->readUint16LE();
	_heroY = stream->readUint16LE();
	_heroZ = stream->readUint16LE();

	_hero = new Actor();
	_hero->setPos(_heroX, _heroY, _heroZ);

	_trackScript = new ScriptTrackV2(stream);
	_trackScript->setActor(_hero);

	_lifeScript = new ScriptLifeV2(stream, _trackScript);
	_lifeScript->setActor(_hero);

	_hero->_lifeScript = _lifeScript;
	_hero->_trackScript = _trackScript;

	_numActors = stream->readUint16LE();

	_actors = new Actor *[_numActors];
	_actors[0] = _hero;
	for (int i = 1; i < _numActors; ++i) {
		_actors[i] = new Actor(stream);
	}

	stream->readUint32LE();
	_numZones = stream->readUint16LE();
	_zones = new Zone[_numZones];
	for (int i = 0; i < _numZones; ++i) {
		Zone *z = &_zones[i];
		z->_x1 = stream->readUint32LE();
		z->_y1 = stream->readUint32LE();
		z->_z1 = stream->readUint32LE();
		z->_x2 = stream->readUint32LE();
		z->_y2 = stream->readUint32LE();
		z->_z2 = stream->readUint32LE();
		z->_info[0] = stream->readUint32LE();
		z->_info[1] = stream->readUint32LE();
		z->_info[2] = stream->readUint32LE();
		z->_info[3] = stream->readUint32LE();
		z->_info[4] = stream->readUint32LE();
		z->_info[5] = stream->readUint32LE();
		z->_info[6] = stream->readUint32LE();
		z->_info[7] = stream->readUint32LE();
		z->_type = stream->readUint16LE();
		z->_snap = stream->readUint16LE();
	}

	_numTracks = stream->readUint16LE();

	for (int i = 0; i < _numTracks; ++i) {
		stream->readUint16LE();
		stream->readUint16LE();
		stream->readUint16LE();
		stream->readUint16LE();
		stream->readUint16LE();
		stream->readUint16LE();
	}

	//Figure out what this data is
	_numEndThings = stream->readUint16LE();
	for (int i = 0; i < _numEndThings; ++i) {
		stream->readUint16LE();
		stream->readUint16LE();
	}

	stream->readUint16LE();
}

void Scene::update(uint32 delta) {
	for (int i = 0; i < _numActors - 1; ++i) {
		_actors[i]->update(delta);
	}
}
void Scene::draw() {
	if (_grid) {
		g_renderer->drawGrid(_grid);
	}
	for (int i = 0; i < _numActors - 1; ++i) {
		_actors[i]->draw();
	}
}

} // end of namespace Twin
