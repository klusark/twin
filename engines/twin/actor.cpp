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

#include "math/vector3d.h"

#include "common/stream.h"

#include "engines/twin/actor.h"
#include "engines/twin/twin.h"
#include "engines/twin/resource.h"
#include "engines/twin/gfx_base.h"
#include "engines/twin/animation.h"
#include "engines/twin/script_track_v2.h"
#include "engines/twin/script_life_v2.h"
#include "engines/twin/script_track_v1.h"
#include "engines/twin/script_life_v1.h"
#include "engines/twin/scene.h"
#include "engines/twin/grid.h"
#include "engines/twin/sprite.h"
#include "common/textconsole.h"

namespace Twin {

Actor::Actor(Common::SeekableReadStream *stream) :
		_entity(nullptr), _dest(nullptr), _dead(false), _isHero(false), _angle(0),
		_facingActor(nullptr), _turning(false), _isMoving(false), _isInvisible(false),
		_numKeys(0), _numGold(0), _lifePoints(0), _sprite(nullptr), _heroMoved(false), _speed(0),
		_currZone(nullptr), _canDetectZones(false) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	} else if (g_twin->getGameType() == GType_LBA) {
		loadLBA(stream);
	}
}

Actor::Actor() : _entity(nullptr), _dest(nullptr), _dead(false), _facingActor(nullptr), _turning(false), _isMoving(false),
		_isInvisible(false), _numKeys(0), _numGold(0), _lifePoints(100), _sprite(nullptr), _heroMoved(false), _speed(0),
		_currZone(nullptr) {
	_entity = g_resource->getEntity(0, 0, 0);
	_pos._x = 0;
	_pos._y = 0;
	_pos._z = 0;
	_canDetectZones = true;
}


void Actor::loadLBA2(Common::SeekableReadStream *stream) {
	//These sizes are probably mostly wrong, but the overall actor size is right
	uint16 flags = stream->readUint16LE();
	uint16 flag2 = stream->readUint16LE();

	if (flags & 0x4) {
		_canDetectZones = true;
	}

	_entityID = stream->readUint16LE();
	_body = stream->readByte();
	stream->readByte();
	_anim = stream->readByte();
	_spriteID = stream->readUint16LE();
	_pos._x = stream->readUint16LE();
	_pos._y = stream->readUint16LE();
	_pos._z = stream->readUint16LE();
	if (_entityID != 0xffff) {
		_entity = g_resource->getEntity(_entityID, 0, 0);
	} else {
		_sprite = g_resource->getSprite(_spriteID);
	}

	stream->readByte();
	stream->readUint16LE();
	_angle = (360+90) - ((stream->readUint16LE() * 360) / 4096);
	stream->readUint16LE();
	_controlMode = (ControlMode)stream->readByte();
	stream->readByte();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readByte();
	stream->readByte();
	stream->readByte();
	stream->readUint16LE();
	_lifePoints = stream->readByte();

	if (flag2 & 0x004) {
		stream->readByte();
		stream->readByte();
		stream->readByte();
		stream->readByte();
		stream->readByte();
		stream->readByte();
	}

	_trackScript = new ScriptTrackV2(stream);
	_trackScript->setActor(this);


	_lifeScript = new ScriptLifeV2(stream, (ScriptTrackV2 *)_trackScript);
	_lifeScript->setActor(this);
}

void Actor::loadLBA(Common::SeekableReadStream *stream) {

	uint16 flags = stream->readUint16LE();
	_entityID = stream->readUint16LE();

	_body = stream->readByte();
	stream->readByte();
	_spriteID = stream->readUint16LE();

	_pos._x = stream->readUint16LE();
	_pos._y = stream->readUint16LE();
	_pos._z = stream->readUint16LE();

	if (_entityID != 0xffff) {
		_entity = g_resource->getEntity(_entityID, 0, 0);
	} else {
		//_sprite = g_resource->getSprite(_spriteID);
	}


	stream->readByte();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readUint16LE();
	stream->readByte();
	stream->readByte();
	stream->readByte();
	stream->readByte();


	_trackScript = new ScriptTrackV1(stream);
	_trackScript->setActor(this);


	_lifeScript = new ScriptLifeV1(stream, (ScriptTrackV1 *)_trackScript);
	_lifeScript->setActor(this);

}

void Actor::update(uint32 delta) {
	if (_dead) {
		return;
	}
	_lifeScript->run(delta);
	_trackScript->run(delta);
	if (_entity) {
		_entity->_anim->update(delta);
	}

	//Probably wrong
	if (!_dest) {
		updateControl();
	}

	Point before = _pos;

	if (_entity && delta != 0) {
		Keyframe *k = _entity->_anim->getKeyframe();
		float speed = (k->_z * (int)delta) / 250.0f;
		speed += _speed;
		float speed2 = (k->_x * (int)delta) / 250.0f;

		_pos._x += _angle.getCosine() * speed;
		_pos._z += _angle.getSine() * speed;

		_pos._x += _angle.getSine() * speed2;
		_pos._z += _angle.getCosine() * speed2;
	}

	if (_sprite && _speed) {
		_pos._x += _angle.getCosine() * (_speed * (int)delta) / 250.0f;
		_pos._z += _angle.getSine() * (_speed * (int)delta) / 250.0f;
		if (_speedStart.getDistanceTo(&_pos) > _speedDistance) {
			int direction = 1;
			if (_speed < 0) {
				direction = -1;
			}
			_speed = 0;
			if (_angle == 0) {
				_pos._x = _speedStart._x + (_speedDistance * direction);
			}
			if (_angle == 90) {
				_pos._z = _speedStart._z + (_speedDistance * direction);
			}
			if (_angle == 180) {
				_pos._x = _speedStart._x - (_speedDistance * direction);
			}
			if (_angle == 270) {
				_pos._z = _speedStart._z - (_speedDistance * direction);
			}
		}
	}

	if (_dest) {
		turnToAngle(_pos.getAngleTo(_dest));

		if (_pos.getDistanceTo(_dest) < 500) {
			_dest = nullptr;
			*_destDone = false;
			_destDone = nullptr;
		}
	}

	if (_facingActor) {
		Math::Angle angle = getAngleTo(_facingActor);
		turnToAngle(angle);
	}

	if (_turning) {
		float turnSpeed = delta/10.0f;
		Math::Angle next = _angle;

		if ((_angle < _dstAngle && (_dstAngle - _angle < 180)) || (_dstAngle < _angle && (_angle - _dstAngle > 180))) {
			next = _angle + turnSpeed;
		} else {
			next = _angle - turnSpeed;
		}

		if ((_angle < _dstAngle && next > _dstAngle) || (_angle > _dstAngle && next < _dstAngle)) {
			next = _dstAngle;
			_turning = false;
		}
		next.normalize(0);

		_angle = next;
	}

	Grid *grid = g_twin->getCurrentScene()->getGrid();
	if (grid) {
		grid->applyBrickShape(this);
	}
}

void Actor::resetActions() {
	_turning = false;
	_dest = nullptr;
	_entity->_anim->_isWaiting = nullptr;
	_isMoving = false;
}

void Actor::draw() {
	if (_dead || _isInvisible) {
		return;
	}
	g_renderer->drawActor(this);
}

void  Actor::setPos(uint16 x, uint16 y, uint16 z) {
	_pos._x = x;
	_pos._y = y;
	_pos._z = z;
}

void Actor::setAnimation(uint16 anim) {
	if (_entity && _entity->_anim->getId() != anim) {
		delete _entity->_anim;
		_anim = anim;
		_entity->_anim = g_resource->getAnimation(_entityID, _anim, _entity->_model);
	}
}

void Actor::setBody(byte body) {
	if (_entity) {
		delete _entity;
		_body = body;
		_entity = g_resource->getEntity(_entityID, _body, _anim);
	}
}

void Actor::gotoPoint(Point *p, bool *done) {
	_dest = p;
	_destDone = done;
	_isMoving = true;
}

Math::Angle Actor::getAngleTo(Actor *a) {
	return _pos.getAngleTo(&a->_pos);
}

void Actor::turnToAngle(Math::Angle angle) {
	_turning = true;
	_dstAngle = angle;
}

bool Actor::collidesWith(Actor *a) {
	if (!_entity || !a->_entity) {
		return false;
	}
	int16 x1 = _entity->_x1 + _pos._x;
	int16 x2 = _entity->_x2 + _pos._x;
	int16 y1 = _entity->_y1 + _pos._y;
	int16 y2 = _entity->_y2 + _pos._y;
	int16 z1 = _entity->_z1 + _pos._z;
	int16 z2 = _entity->_z2 + _pos._z;

	int16 ox1 = a->_entity->_x1 + a->_pos._x;
	int16 ox2 = a->_entity->_x2 + a->_pos._x;
	int16 oy1 = a->_entity->_y1 + a->_pos._y;
	int16 oy2 = a->_entity->_y2 + a->_pos._y;
	int16 oz1 = a->_entity->_z1 + a->_pos._z;
	int16 oz2 = a->_entity->_z2 + a->_pos._z;

	if (x1 > ox2 || x2 < ox1 ||
		y1 > oy2 || y2 < oy1 ||
		z1 > oz2 || z2 < oz1) {
		return false;
	}
	return true;
}

void Actor::updateControl() {
	switch (_controlMode) {
	case kManual:
		if (!g_twin->getKey(KeyUp) && !g_twin->getKey(KeyDown) && !g_twin->getKey(KeyLeft) && !g_twin->getKey(KeyRight)) {
			if (_heroMoved) {
				setAnimation(kStanding);
				_heroMoved = false;
			}
		}

		if (g_twin->getKey(KeyUp)) {
			setAnimation(kForward);
			_heroMoved = true;
		}

		if (g_twin->getKey(KeyDown) && !g_twin->getKey(KeyUp)) { 
			setAnimation(kBackward);
			_heroMoved = true;
		}

		if (g_twin->getKey(KeyLeft)) {
			if (_entity->_anim->getId() == 0) {
				setAnimation(kTurnLeft);
			} else {
			/*	if (!actor->dynamicFlags.bIsRotationByAnim) {
					actor->angle = getRealAngle(&actor->move);
				}*/
			}
		}

		if (g_twin->getKey(KeyRight)) {
			if (_entity->_anim->getId() == 0) {
				setAnimation(kTurnRight);
			} else {
				/*if (!actor->dynamicFlags.bIsRotationByAnim) {
					actor->angle = getRealAngle(&actor->move);
				}*/
			}
		}
		break;
	case kSameXZ:
		{
			Actor *a = g_twin->getCurrentScene()->getActor(7);
			_pos._x = a->_pos._x;
			_pos._z = a->_pos._z;
		}
	}
}

} // end of namespace Twin
