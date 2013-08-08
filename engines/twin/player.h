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

#ifndef TWIN_PLAYER_H
#define TWIN_PLAYER_H

#include "engines/twin/actor.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

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

class Player : public Actor {
public:
	Player();
};

} // end of namespace Twin

#endif
