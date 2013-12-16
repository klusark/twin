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

#ifndef TWIN_ACTOR_H
#define TWIN_ACTOR_H

#include "math/angle.h"

#include "engines/twin/block_library.h"
#include "engines/twin/scene.h"
#include "engines/twin/resource.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Entity;
class Script;
class Point;
class Sprite;

enum ControlMode {
	kNoMove			= 0,
	kManual			= 1,
	kFollow			= 2,
	kTrack			= 3,
	kFollow2		= 4,
	kTrackAttack	= 5,
	kSameXZ			= 6,
	kRandom			= 7
};

enum AnimationTypes {
	kStanding			= 0,
	kForward			= 1,
	kBackward			= 2,
	kTurnLeft			= 3,
	kTurnRight			= 4,
	kHit				= 5,
	kBigHit				= 6,
	kFall				= 7,
	kLanding			= 8,
	kLandingHit			= 9,
	kLandDeath			= 10,
	kAction				= 11,
	kClimbLadder		= 12,
	kTopLadder			= 13,
	kJump				= 14,
	kThrowBall			= 15,
	kHide				= 16,
	kKick				= 17,
	kRightPunch			= 18,
	kLeftPunch			= 19,
	kFoundItem			= 20,
	kDrawn				= 21,
	kHit2				= 22,
	kSabreAttack		= 23
};

class Actor {
public:
	Actor(Common::SeekableReadStream *stream);
	Actor();
	void draw();
	virtual void update(uint32 delta);
	void setAnimation(uint16 anim, bool stop = false);
	void setBody(byte body);
	void setPos(uint16 x, uint16 y, uint16 z);
	void gotoPoint(Point *p, bool *done);
	void gotoPoint3D(Point *p, bool *done);
	void kill() { _dead = true; _lifePoints = 0; }
	void setInvisible(bool state) { _isInvisible = state; }
	Zone *getCurrentZone() { return _currZone; }

	void turnToAngle(Math::Angle angle);
	void setAngle(Math::Angle angle) { _angle = angle; _angle.normalize(0); }
	Math::Angle getAngleTo(Actor *a);
	void faceActor(Actor *a) { _facingActor = a; }
	bool collidesWith(Actor *a);
	void resetActions();
	byte getNumKeys() { return _numKeys; }
	uint16 getNumGold() { return _numGold; }
	uint16 getLifePoints() { return _lifePoints; }
	void hitBy(Actor *a, byte damage) { _lifePoints -= damage; _lastHitBy = a; }
	bool isZonable() { return _canDetectZones; }
	bool isStandingOnActor(Actor *other);
	void processMovement();
	void processCollision();
	void applyBrickShape(ShapeType brickShape, const Point &brick);
//private:
	void loadLBA2(Common::SeekableReadStream *stream);
	void loadLBA(Common::SeekableReadStream *stream);

	void updateControl();
	virtual void updateControlManual() { };
	void stopFalling();
	void setBoundingBox();

	int _heroYBeforeFall;

	bool _isHero;
	bool _heroMoved;
	uint16 _entityID;
	byte _body;
	byte _anim;
	byte _lastAnim;
	Point _pos;
	Point _previousPos;
	Point _processPos;
	ControlMode _controlMode;
	Zone *_currZone;

	uint16 _spriteID;

	Script *_trackScript;
	Script *_lifeScript;

	Entity *_entity;
	Sprite *_sprite;

	Point *_dest;
	bool *_destDone;
	bool _dest3D;

	bool _isMoving;
	bool _isInvisible;
	bool _dead;
	bool _canDetectZones;
	Math::Angle _angle;
	Math::Angle _angleY;
	Math::Angle _dstAngle;
	bool _turning;

	byte _numKeys;
	uint16 _numGold;

	int16 _speed;
	uint16 _speedDistance;
	Point _speedStart;

	uint16 _lifePoints;

	Actor *_facingActor;
	Actor *_lastHitBy;
	Actor *_standingOn;

	BoundingBox *_box;

	bool _isFalling;

	ShapeType _brickShape;

	// flags
	bool _carrier;
};

} // end of namespace Twin

#endif
