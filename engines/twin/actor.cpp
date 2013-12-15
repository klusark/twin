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
#include "engines/twin/model.h"
#include "common/textconsole.h"

namespace Twin {

Actor::Actor(Common::SeekableReadStream *stream) :
		_entity(nullptr), _dest(nullptr), _dead(false), _isHero(false), _angle(0),
		_facingActor(nullptr), _turning(false), _isMoving(false), _isInvisible(false),
		_numKeys(0), _numGold(0), _lifePoints(0), _sprite(nullptr), _heroMoved(false), _speed(0),
		_currZone(nullptr), _canDetectZones(false), _standingOn(nullptr), _carrier(false), _box(nullptr),
		_isFalling(false) {
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	} else if (g_twin->getGameType() == GType_LBA) {
		loadLBA(stream);
	}
	setBoundingBox();

}

Actor::Actor() : _entity(nullptr), _dest(nullptr), _dead(false), _facingActor(nullptr), _turning(false), _isMoving(false),
		_isInvisible(false), _numKeys(0), _numGold(0), _lifePoints(100), _sprite(nullptr), _heroMoved(false), _speed(0),
		_currZone(nullptr), _standingOn(nullptr), _carrier(false), _box(nullptr), _isFalling(false),
		_heroYBeforeFall(0) {
	_entity = g_resource->getEntity(0, 0, 0);
	_pos._x = 0;
	_pos._y = 0;
	_pos._z = 0;
	_canDetectZones = true;

	setBoundingBox();
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
	if (flags & 0x4000) {
		_carrier = true;
	}
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
		_sprite = g_resource->getSprite(_spriteID);
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

	processMovement();

	_previousPos = _pos;

	_trackScript->run(delta);

	if (_entity) {
		_entity->_anim->update(delta);
	}

	_processPos = _pos;

	if (_entity && delta != 0) {
		Keyframe *k = _entity->_anim->getKeyframe();
		float speed = (k->_z * (int)delta) / 250.0f;
		speed += _speed;
		float speed2 = (k->_x * (int)delta) / 250.0f;

		_processPos._x += _angle.getCosine() * speed;
		_processPos._z += _angle.getSine() * speed;

		_processPos._x += _angle.getSine() * speed2;
		_processPos._z += _angle.getCosine() * speed2;
	}

	processCollision();

	//return;

	_lifeScript->run(delta);


	_pos = _processPos;

	if ((int16)_pos._z < 0) {
		warning("ASDF");
		_pos._z = 0;
	}

	/*Point before = _pos;



	if (_sprite && _speed) {
		_pos._x += _angle.getCosine() * (_speed * (int)delta) / 250.0f;
		_pos._z += _angle.getSine() * (_speed * (int)delta) / 250.0f;
		if (_dest3D) {
			_pos._y += _angleY.getSine() * (_speed * (int)delta) / 250.0f;
		}
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

		if (_dest3D) {
			float ydiff = (int)_dest->_y - (int)_pos._y;

			double rads = atan2(ydiff, _pos.getDistanceTo(_dest));

			if (ydiff < 0) {
				rads += 2 * M_PI;
			}
			_angleY = Math::Angle::fromRadians(rads);
		}

		uint16 distance = 0;
		if (_dest3D) {
			distance = _pos.get3DDistanceTo(_dest);
		} else {
			distance = _pos.getDistanceTo(_dest);
		}
		if ((_dest3D && distance < 100) || (!_dest3D && distance < 500)) {
			_dest = nullptr;
			*_destDone = false;
			_destDone = nullptr;
			_dest3D = false;
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
	}*/



	/*Scene *s = g_twin->getCurrentScene();
	for (int i = 0; i < s->_numActors; ++i) {
		Actor *a = s->getActor(i);
		if (a->_carrier && a != this && isStandingOnActor(a)) {
			_standingOn = a;
		}
	}

	Grid *grid = s->getGrid();
	if (grid && _entity) {
		grid->applyBrickShape(this);
	}*/
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
		setBoundingBox();
	}
}

void Actor::gotoPoint(Point *p, bool *done) {
	return;
	_dest = p;
	_destDone = done;
	_isMoving = true;
	_dest3D = false;
}

void Actor::gotoPoint3D(Point *p, bool *done) {
	gotoPoint(p, done);
	_dest3D = true;
}

Math::Angle Actor::getAngleTo(Actor *a) {
	return _pos.getAngleTo(&a->_pos);
}

void Actor::turnToAngle(Math::Angle angle) {
	if (_sprite) {
		_angle = angle;
	} else {
		_turning = true;
		_dstAngle = angle;
	}
}

bool Actor::collidesWith(Actor *a) {
	if (!_box || !a->_box) {
		return false;
	}
	int16 x1 = _box->_x1 + _pos._x;
	int16 x2 = _box->_x2 + _pos._x;
	int16 y1 = _box->_y1 + _pos._y;
	int16 y2 = _box->_y2 + _pos._y;
	int16 z1 = _box->_z1 + _pos._z;
	int16 z2 = _box->_z2 + _pos._z;

	int16 ox1 = a->_box->_x1 + a->_pos._x;
	int16 ox2 = a->_box->_x2 + a->_pos._x;
	int16 oy1 = a->_box->_y1 + a->_pos._y;
	int16 oy2 = a->_box->_y2 + a->_pos._y;
	int16 oz1 = a->_box->_z1 + a->_pos._z;
	int16 oz2 = a->_box->_z2 + a->_pos._z;

	if (x1 > ox2 || x2 < ox1 ||
		y1 > oy2 || y2 < oy1 ||
		z1 > oz2 || z2 < oz1) {
		return false;
	}
	return true;
}

bool Actor::isStandingOnActor(Actor *other) {
	int32 x1Left, y1Left, z1Left, x1Right, y1Right, z1Right;
	int32 x2Left, y2Left, z2Left, x2Right, y2Right, z2Right;
	Actor *actor1;
	Actor *actor2;

	actor1 = this;
	actor2 = other;

	if (actor1->_box == nullptr || actor2->_box == nullptr) {
		return false;
	}

	// Current actor (actor 1)
	x1Left = actor1->_pos._x + actor1->_box->_x1;
	x1Right = actor1->_pos._x + actor1->_box->_x2;

	y1Left = actor1->_pos._y + actor1->_box->_y1;
	y1Right = actor1->_pos._y + actor1->_box->_y2;

	z1Left = actor1->_pos._z + actor1->_box->_z1;
	z1Right = actor1->_pos._z + actor1->_box->_z2;

	// Actor 2
	x2Left = actor2->_pos._x + actor2->_box->_x1;
	x2Right = actor2->_pos._x + actor2->_box->_x2;

	y2Left = actor2->_pos._y + actor2->_box->_y1;
	y2Right = actor2->_pos._y + actor2->_box->_y2;

	z2Left = actor2->_pos._z + actor2->_box->_z1;
	z2Right = actor2->_pos._z + actor2->_box->_z2;

	if (x1Left >= x2Right)
		return 0; // not standing

	if (x1Right <= x2Left)
		return 0;

	if (y1Left > (y2Right + 1))
		return 0;

	if (y1Left <= (y2Right - 0x100))
		return 0;

	if (y1Right <= y2Left)
		return 0;

	if (z1Left >= z2Right)
		return 0;

	if (z1Right <= z2Left)
		return 0;

	return 1; // standing
}

void Actor::processMovement() {


	if (_isFalling) {
	} else {
		updateControl();
	}
}

void Actor::updateControl() {
	switch (_controlMode) {
	case kNoMove:
		break;
	case kManual:
		updateControlManual();
		break;
	case kSameXZ:
		{
			//TODO: Make this get the following actor
			Actor *a = g_twin->getCurrentScene()->getActor(7);
			_pos._x = a->_pos._x;
			_pos._z = a->_pos._z;
		}
		break;
	default:
		warning("Control mode not handled: %d", _controlMode);
		break;
	}
}

void Actor::setBoundingBox() {
	if (_entity) {
		if (g_twin->getGameType() == GType_LBA2 && _entity->_hasBox) {
			_box = &_entity->_box;
		} else {
			_box = &_entity->_model->_box;
		}
	}
	if (_sprite) {
		_box = &_sprite->_info->_box;
	}
}

int32 getAverageValue(float var0, float var1, float var2, float var3) {
	if (var3 <= 0) {
		return var0;
	}

	if (var3 >= var2) {
		return var1;
	}

	return ((((var1 - var0) * var3) / var2) + var0);
}

void Actor::applyBrickShape(ShapeType shape, const Point &brick) {
	if (!shape) {
		return;
	}

	int brkX = (brick._x << 9) - 0x100;
	int brkY = brick._y << 8;
	int brkZ = (brick._z << 9) - 0x100;



	// double-side stairs
	if (shape >= kDoubleSideStairsTop1 && shape <= kDoubleSideStairsRight2) {
		switch (shape) {
		case kDoubleSideStairsTop1:
			if (_processPos._z - brick._z <= _processPos._x - brick._x) {
				shape = kStairsTopLeft;
			} else {
				shape = kStairsTopRight;
			}
			break;
		case kDoubleSideStairsBottom1:
			if (_processPos._z - brick._z <= _processPos._x - brick._x) {
				shape = kStairsBottomLeft;
			} else {
				shape = kStairsBottomRight;
			}
			break;
		case kDoubleSideStairsLeft1:
			if (512 - _processPos._x - brick._x <= _processPos._z - brick._z) {
				shape = kStairsTopLeft;
			} else {
				shape = kStairsBottomLeft;
			}
			break;
		case kDoubleSideStairsRight1:
			if (512 - _processPos._x - brick._x <= _processPos._z - brick._z) {
				shape = kStairsTopRight;
			} else {
				shape = kStairsBottomRight;
			}
			break;
		case kDoubleSideStairsTop2:
			if (_processPos._x - brick._x >= _processPos._z - brick._z) {
				shape = kStairsTopRight;
			} else {
				shape = kStairsTopLeft;
			}
			break;
		case kDoubleSideStairsBottom2:
			if (_processPos._z - brick._z <= _processPos._x - brick._x) {
				shape = kStairsBottomRight;
			} else {
				shape = kStairsBottomLeft;
			}
			break;
		case kDoubleSideStairsLeft2:
			if (512 - _processPos._x - brick._x <= _processPos._z - brick._z) {
				shape = kStairsBottomLeft;
			} else {
				shape = kStairsTopLeft;
			}
			break;
		case kDoubleSideStairsRight2:
			if (512 - _processPos._x - brick._x <= _processPos._z - brick._z) {
				shape = kStairsBottomRight;
			} else {
				shape = kStairsTopRight;
			}
			break;
		}
	}

	switch (shape) {
	case kStairsTopLeft:
		_processPos._y = brkY + getAverageValue(0, 0x100, 0x200, _processPos._x - brkX);
		break;
	case kStairsTopRight:
		_processPos._y = brkY + getAverageValue(0, 0x100, 0x200, _processPos._z - brkZ);
		break;
	case kStairsBottomLeft:
		_processPos._y = brkY + getAverageValue(0x100, 0, 0x200, _processPos._z - brkZ);
		break;
	case kStairsBottomRight:
		_processPos._y = brkY + getAverageValue(0x100, 0, 0x200, _processPos._x - brkX);
		break;
	default:
		break;
	}
}

void Actor::processCollision() {
	Scene *s = g_twin->getCurrentScene();
	Grid *grid = s->getGrid();



	if (_standingOn != nullptr) {
		_pos += (_standingOn->_pos - _standingOn->_previousPos);

		if (!isStandingOnActor(_standingOn)) {
			_standingOn = nullptr;
		}
	}

	// actor falling Y speed
	if (_isFalling) {
		/*_processPos._x = previousActorX;
		processActorY = previousActorY + loopActorStep; // add step to fall
		_processPos._z = previousActorZ;*/
		_processPos._y -= 10;
	}



	// actor collisions with bricks
	if (_isHero/*actor->staticFlags.bComputeCollisionWithBricks*/) {
		ShapeType brickShape;
		//_collisionPos._y = 0;

		Point collide;
		brickShape = grid->getBrickShape(_previousPos, &collide);

		if (brickShape) {
			if (brickShape != kSolid) {
				applyBrickShape(brickShape, collide);
			} /*else { // this shouldn't happen (collision should avoid it)
				actor->Y = processActorY = (processActorY / 256) * 256 + 256; // go upper
			}*/
		}

		if (true /*actor->staticFlags.bComputeCollisionWithObj*/) {
			//checkCollisionWithActors(actorIdx);
		}

		if (_standingOn && _isFalling) {
			stopFalling();
		}

		//causeActorDamage = 0;

		//_processCollisionPos = _processPos;

		/*
		if (_isHero && !actor->staticFlags.bComputeLowCollision) {
			// check hero collisions with bricks
			checkHeroCollisionWithBricks(actor->boudingBox.X.bottomLeft, actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.bottomLeft, 1);
			checkHeroCollisionWithBricks(actor->boudingBox.X.topRight,   actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.bottomLeft, 2);
			checkHeroCollisionWithBricks(actor->boudingBox.X.topRight,   actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.topRight,   4);
			checkHeroCollisionWithBricks(actor->boudingBox.X.bottomLeft, actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.topRight,   8);
		} else {
			// check other actors collisions with bricks
			checkActorCollisionWithBricks(actor->boudingBox.X.bottomLeft, actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.bottomLeft, 1);
			checkActorCollisionWithBricks(actor->boudingBox.X.topRight,   actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.bottomLeft, 2);
			checkActorCollisionWithBricks(actor->boudingBox.X.topRight,   actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.topRight,   4);
			checkActorCollisionWithBricks(actor->boudingBox.X.bottomLeft, actor->boudingBox.Y.bottomLeft, actor->boudingBox.Z.topRight,   8);
		}*/

		// process wall hit while running
		/*if (causeActorDamage && !actor->dynamicFlags.bIsFalling && !currentlyProcessedActorIdx && heroBehaviour == kAthletic && actor->anim == kForward) {
			rotateActor(actor->boudingBox.X.bottomLeft, actor->boudingBox.Z.bottomLeft, actor->angle + 0x580);

			destX += _processPos._x;
			destZ += _processPos._z;

			if (destX >= 0 && destZ >= 0 && destX <= 0x7E00 && destZ <= 0x7E00) {
				if (getBrickShape(destX, processActorY + 0x100, destZ) && cfgfile.WallCollision == 1) { // avoid wall hit damage
					addExtraSpecial(actor->X, actor->Y + 1000, actor->Z, kHitStars);
					initAnim(kBigHit, 2, 0, currentlyProcessedActorIdx);

					if (currentlyProcessedActorIdx == 0) {
						heroMoved = 1;
					}

					actor->life--;
				}
			}
		}*/
		brickShape = grid->getBrickShape(_processPos, &collide);
		//brickShape = getBrickShape(_processPos._x, processActorY, _processPos._z);
		_brickShape = brickShape;

		if (brickShape) {
			if (brickShape == kSolid) {
				if (_isFalling) {
					stopFalling();
					_processPos._y = (collide._y << 8) + 0x100;
				} else {
					/*if (_isHero && heroBehaviour == kAthletic && actor->anim == brickShape && cfgfile.WallCollision == 1) { // avoid wall hit damage
						addExtraSpecial(actor->X, actor->Y + 1000, actor->Z, kHitStars);
						initAnim(kBigHit, 2, 0, currentlyProcessedActorIdx);

						if (!actorIdx) {
							heroMoved = 1;
						}

						actor->life--;
					}*/

					// no Z coordinate issue
					if (!grid->getBrickShape(_processPos._x, _processPos._y, _previousPos._z)) {
						_processPos._z = _previousPos._z;
					}

					// no X coordinate issue
					if (!grid->getBrickShape(_previousPos._x, _processPos._y, _processPos._z)) {
						_processPos._x = _previousPos._x;
					}

					// X and Z with issue, no move
					if (grid->getBrickShape(_processPos._x, _processPos._y, _previousPos._z) && grid->getBrickShape(_previousPos._x, _processPos._y, _processPos._z)) {
						_processPos = _previousPos;
					}
				}
			} else {
				if (_isFalling) {
					stopFalling();
				}

				applyBrickShape(brickShape, collide);
			}

			//_isFalling = false;
		} else {
			if (/*actor->staticFlags.bCanFall && */_standingOn == nullptr) {
				Point collide;
				brickShape = grid->getBrickShape(_processPos._x, _processPos._y - 1, _processPos._z, &collide);

				if (brickShape) {
					if (_isFalling) {
						stopFalling();
					}

					applyBrickShape(brickShape, collide);
				} else {
					//if (!actor->dynamicFlags.bIsRotationByAnim) {
						_isFalling = true;

						if (_isHero && _heroYBeforeFall == 0) {
							_heroYBeforeFall = _processPos._y;
						}

						//initAnim(kFall, 0, 255, actorIdx);
						setAnimation(kFall);

					//}
				}
			}
		}

		// if under the map, than die
		/*if (collisionY == -1) {
			actor->life = 0;
		}*/
	} /*else {
		if (actor->staticFlags.bComputeCollisionWithObj) {
			checkCollisionWithActors(actorIdx);
		}
	}*/
}

void Actor::stopFalling() {
	_isFalling = false;
	int32 fall;

	if (_isHero) {
		fall = _heroYBeforeFall - _processPos._y;

		if (fall >= 0x1000) {
			//addExtraSpecial(processActorPtr->X, processActorPtr->Y + 1000, processActorPtr->Z, kHitStars);
			//processActorPtr->life--;
			//initAnim(kLandingHit, 2, 0, currentlyProcessedActorIdx);
			setAnimation(kLandingHit);
		} else if (fall >= 0x800) {
			//addExtraSpecial(processActorPtr->X, processActorPtr->Y + 1000, processActorPtr->Z, kHitStars);
			//processActorPtr->life--;
			//initAnim(kLandingHit, 2, 0, currentlyProcessedActorIdx);
			setAnimation(kLandingHit);
		} else if (fall > 10) {
			//initAnim(kLanding, 2, 0, currentlyProcessedActorIdx);
			setAnimation(kLanding);
		} else {
			//initAnim(kStanding, 0, 0, currentlyProcessedActorIdx);
			setAnimation(kStanding);
		}

		_heroYBeforeFall = 0;
	} else {
		setAnimation(kLanding);
		//initAnim(kLanding, 2, processActorPtr->animExtra, currentlyProcessedActorIdx);
	}
}

} // end of namespace Twin
