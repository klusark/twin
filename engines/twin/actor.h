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

#include "engines/twin/scene.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Entity;
class Script;
class Point;

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
	void update(uint32 delta);
	void setAnimation(uint16 anim);
	void setPos(uint16 x, uint16 y, uint16 z);
	void gotoPoint(Point *p, bool *done);
	void kill() { _dead = true; }

	void turnToAngle(Math::Angle angle);
	void setAngle(Math::Angle angle) { _angle = angle; _angle.normalize(0); }
	Math::Angle getAngleTo(Actor *a);
	void faceActor(Actor *a) { _facingActor = a; }
	bool collidesWith(Actor *a);
	void resetActions();
//private:
	void loadLBA2(Common::SeekableReadStream *stream);

	void updateControl();

	bool _isHero;
	uint16 _entityID;
	byte _body;
	byte _anim;
	uint16 _sprite;
	Point _pos;
	ControlMode _controlMode;

	Script *_trackScript;
	Script *_lifeScript;

	Entity *_entity;

	Point *_dest;
	bool *_destDone;

	bool _isMoving;
	bool _dead;
	Math::Angle _angle;
	Math::Angle _dstAngle;
	bool _turning;

	Actor *_facingActor;
};

} // end of namespace Twin

#endif
