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
#include "engines/twin/player.h"
#include "engines/twin/scene.h"
#include "engines/twin/twin.h"
#include "engines/twin/script_track_v1.h"
#include "engines/twin/script_life_v1.h"
#include "engines/twin/script_track_v2.h"
#include "engines/twin/script_life_v2.h"

namespace Twin {

Scene::Scene(Common::SeekableReadStream *stream) : _grid(nullptr) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	} else if (g_twin->getGameType() == GType_LBA) {
		loadLBA(stream);
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

	Player *_hero = g_twin->getPlayer();
	//Not sure when this should actually be set...
	_hero->setPos(_heroX, _heroY, _heroZ);
	_hero->_isHero = true;

	_trackScript = new ScriptTrackV2(stream);
	_trackScript->setActor(_hero);

	_lifeScript = new ScriptLifeV2(stream, (ScriptTrackV2 *)_trackScript);
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
		z->_type = (ZoneType)stream->readUint16LE();
		z->_snap = stream->readUint16LE();
	}

	_numPoints = stream->readUint16LE();
	_points = new Point[_numPoints];
	for (int i = 0; i < _numPoints; ++i) {
		Point *p = &_points[i];
		p->_x = stream->readUint32LE();
		p->_y = stream->readUint32LE();
		p->_z = stream->readUint32LE();
	}

	//Figure out what this data is
	_numEndThings = stream->readUint16LE();
	for (int i = 0; i < _numEndThings; ++i) {
		stream->readUint16LE();
		stream->readUint16LE();
	}

	stream->readUint16LE();
}

void Scene::loadLBA(Common::SeekableReadStream *stream) {
	stream->readByte();
	_textBank = stream->readByte();
	stream->readUint32LE();

	stream->readUint16LE();
	stream->readUint16LE();

	_ambience[0] = stream->readUint16LE();
	_repeat[0] = stream->readUint16LE();
	_round[0] = stream->readUint16LE();

	_ambience[1] = stream->readUint16LE();
	_repeat[1] = stream->readUint16LE();
	_round[1] = stream->readUint16LE();

	_ambience[2] = stream->readUint16LE();
	_repeat[2] = stream->readUint16LE();
	_round[2] = stream->readUint16LE();

	_ambience[3] = stream->readUint16LE();
	_repeat[3] = stream->readUint16LE();
	_round[3] = stream->readUint16LE();

	stream->readUint16LE();
	stream->readUint16LE();

	stream->readByte();

	_heroX = stream->readUint16LE();
	_heroY = stream->readUint16LE();
	_heroZ = stream->readUint16LE();

	Player *_hero = g_twin->getPlayer();
	//Not sure when this should actually be set...
	_hero->setPos(_heroX, _heroY, _heroZ);
	_hero->_isHero = true;

	_trackScript = new ScriptTrackV1(stream);
	_trackScript->setActor(_hero);

	_lifeScript = new ScriptLifeV1(stream, (ScriptTrackV1 *)_trackScript);
	_lifeScript->setActor(_hero);

	_hero->_lifeScript = _lifeScript;
	_hero->_trackScript = _trackScript;


	_numActors = stream->readUint16LE();

	_actors = new Actor *[_numActors];
	_actors[0] = _hero;
	for (int i = 1; i < _numActors; ++i) {
		_actors[i] = new Actor(stream);
	}

	_numZones = stream->readUint16LE();
	_zones = new Zone[_numZones];
	for (int i = 0; i < _numZones; ++i) {
		Zone *z = &_zones[i];
		z->_x1 = stream->readUint16LE();
		z->_y1 = stream->readUint16LE();
		z->_z1 = stream->readUint16LE();
		z->_x2 = stream->readUint16LE();
		z->_y2 = stream->readUint16LE();
		z->_z2 = stream->readUint16LE();
		z->_type = (ZoneType)stream->readUint16LE();
		z->_info[0] = stream->readUint16LE();
		z->_info[1] = stream->readUint16LE();
		z->_info[2] = stream->readUint16LE();
		z->_info[3] = stream->readUint16LE();
		z->_snap = stream->readUint16LE();
		z->_id = z->_info[0];
	}

	_numPoints = stream->readUint16LE();
	_points = new Point[_numPoints];
	for (int i = 0; i < _numPoints; ++i) {
		Point *p = &_points[i];
		p->_x = stream->readUint16LE();
		p->_y = stream->readUint16LE();
		p->_z = stream->readUint16LE();
	}
}

void Scene::update(uint32 delta) {
	for (int i = 0; i < _numActors; ++i) {
		Actor *a = _actors[i];
		processActorZone(a);
		a->update(delta);
	}
	for (int i = 0; i < _numActors; ++i) {
		Actor *a = _actors[i];
		a->_lastHitBy = nullptr;
	}
}
void Scene::draw() {
	if (_grid) {
		g_renderer->drawGrid(_grid);
	}
	for (int i = 0; i < _numActors; ++i) {
		_actors[i]->draw();
	}
}

void Scene::processActorZone(Actor *a) {
	if (!a->isZonable()) {
		return;
	}
	a->_currZone = nullptr;
	for (int j = 0; j < _numZones; ++j) {
		Zone *z = &_zones[j];
		if (z->isActorInside(a)) {
			switch (z->_type) {
			case kCube:
				if (a->_isHero) {
					if (g_twin->getGameType() == GType_LBA2) {
						g_twin->changeScene(z->_snap, z->_info[0], z->_info[1], z->_info[2]);
					} else {
						g_twin->changeScene(z->_info[0], z->_info[1], z->_info[2], z->_info[3]);
					}
				}
				break;
			case kObject:
				if (g_twin->getKey(KeyAction)) {
					a->setAnimation(kAction, true);
					a->_numKeys = 1;
				}
				break;
			case kSceneric:
				a->_currZone = z;
				break;
			}
		}
	}
}

byte Zone::getId() {
	return _id;
}

bool Zone::isActorInside(Actor *a) {
	if (a->_pos._x >= _x1 && a->_pos._x <= _x2 && a->_pos._y >= _y1 && a->_pos._y <= _y2 && a->_pos._z >= _z1 && a->_pos._z <= _z2) {
		return true;
	}
	return false;
}

Math::Angle Point::getAngleTo(Point *p) {
	float xdiff = (int)p->_x - (int)_x;
	float ydiff = (int)p->_z - (int)_z;

	double rads = atan2(ydiff, xdiff);

	if (ydiff < 0) {
		rads += 2 * M_PI;
	}

	return Math::Angle::fromRadians(rads);
}

uint16 Point::getDistanceTo(Point *p) {
	float xdiff = (int)p->_x - (int)_x;
	float ydiff = (int)p->_z - (int)_z;

	return sqrt(xdiff * xdiff + ydiff * ydiff);
}

uint16 Point::get3DDistanceTo(Point *p) {
	float xdiff = (int)p->_x - (int)_x;
	float ydiff = (int)p->_y - (int)_y;
	float zdiff = (int)p->_z - (int)_z;

	return sqrt(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
}

void Point::operator += (const Point &other) {
	_x += other._x;
	_y += other._y;
	_z += other._z;
}
Point Point::operator - (const Point &other) const {
	Point p;
	p._x = _x - other._x;
	p._y = _y - other._y;
	p._z = _z - other._z;
	return p;
}

} // end of namespace Twin
