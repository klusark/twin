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
#define  FORBIDDEN_SYMBOL_EXCEPTION_rand

#include "engines/twin/player.h"
#include "engines/twin/resource.h"
#include "engines/twin/twin.h"

namespace Twin {

Player::Player() {
	_entityID = 0;
	_controlMode = kManual;
	_behavior = kNormal;
	_heroMoved = 0;
	_heroAction = 0;
	_body = 0;
}

void Player::update(uint32 delta) {
	Actor::update(delta);

	if (g_twin->getKey(KeyNormal)) {
		_behavior = kNormal;
		_entity = g_resource->getEntity(0, _body, 0);
		_entityID = 0;
	}
	if (g_twin->getKey(KeyAthletic)) {
		_behavior = kAthletic;
		_entity = g_resource->getEntity(1, _body, 0);
		_entityID = 1;
	}
	if (g_twin->getKey(KeyAggressive)) {
		_behavior = kAggressive;
		_entity = g_resource->getEntity(2, _body, 0);
		_entityID = 2;
	}
	if (g_twin->getKey(KeyDiscreet)) {
		_behavior = kDiscrete;
		_entity = g_resource->getEntity(3, _body, 0);
		_entityID = 3;
	}
}

void Player::handleKeyDown(Common::KeyCode key) {
	if (key == Common::KEYCODE_UP) {
		delete _entity;
		_entity = g_resource->getEntity(0, 0, kForward);
	}
}

void Player::handleKeyUp(Common::KeyCode key) {

}

void Player::updateControlManual() {

	if (_isHero) { // take this out when we want to give manual movements to other characters than Hero


		// If press W for action
		/*if (skipIntro == 0x11) {
			heroAction = 1;
		}*/

		// Process hero actions
		switch (_behavior) {
		case kNormal:
			if (g_twin->getKey(KeyAction)) {
				_heroAction = 1;
			}
			break;
		case kAthletic:
			if (g_twin->getKey(KeyAction)) {
				setAnimation(kJump, true);
				//initAnim(kJump, 1, 0, actorIdx);
			}
			break;
		case kAggressive:
			if (g_twin->getKey(KeyAction)) {
				if (true /*autoAgressive*/) {
					_heroMoved = 1;
					//actor->angle = getRealAngle(&actor->move);
					if (_anim == 0) {
						int32 aggresiveMode = rand() % 3;

						switch (aggresiveMode) {
						case 0:
 							setAnimation(kKick, true);
							//initAnim(kKick, 1, 0, actorIdx);
							break;
						case 1:
							setAnimation(kRightPunch, true);
							//initAnim(kRightPunch, 1, 0, actorIdx);
							break;
						case 2:
							setAnimation(kLeftPunch, true);
							//initAnim(kLeftPunch, 1, 0, actorIdx);
							break;
						}
					}
				} else {
				/*	if (key & 8) {
						initAnim(kRightPunch, 1, 0, actorIdx);
					}

					if (key & 4) {
						initAnim(kLeftPunch, 1, 0, actorIdx);
					}

					if (key & 1) {
						initAnim(kKick, 1, 0, actorIdx);
					}*/
				}
			}
			break;
		case kDiscrete:
			if (g_twin->getKey(KeyAction)) {
				setAnimation(kHide);
				//initAnim(kHide, 0, 255, actorIdx);
			}
			break;
		}
	}

	/*if ((loopPressedKey & 8) && !gameFlags[GAMEFLAG_INVENTORY_DISABLED]) {
		if (usingSabre == 0) { // Use Magic Ball
			if (gameFlags[GAMEFLAG_HAS_MAGICBALL]) {
				if (magicBallIdx == -1) {
					initAnim(kThrowBall, 1, 0, actorIdx);
				}

				heroMoved = 1;
				actor->angle = getRealAngle(&actor->move);
			}
		} else {
			if (gameFlags[GAMEFLAG_HAS_SABRE]) {
				if (actor->body != 2) {
					initModelActor(2, actorIdx);
				}

				initAnim(kSabreAttack, 1, 0, actorIdx);

				heroMoved = 1;
				actor->angle = getRealAngle(&actor->move);
			}
		}
	}
	*/
	int loopPressedKey = 0;
	if (!loopPressedKey || _heroAction) {
		int16 tempAngle;

		if (g_twin->getKey(KeyDown) || g_twin->getKey(KeyUp)) {  // if continue walking
			_heroMoved = 0; // don't break animation
		}

		/*if (key != heroPressedKey || loopPressedKey != heroPressedKey2) {*/
			if (g_twin->keysChanged() &&_heroMoved) {
				setAnimation(kStanding);
				//initAnim(kStanding, 0, 255, actorIdx);
			}
		//}

		_heroMoved = 0;

		if (g_twin->getKey(KeyUp)) { // walk forward
			//if (!currentActorInZone) {
				setAnimation(kForward);
				//initAnim(kForward, 0, 255, actorIdx);
			//}
			_heroMoved = 1;
		}

		if (g_twin->getKey(KeyDown) && !g_twin->getKey(KeyUp)) { // walk backward
			setAnimation(kBackward);
			//initAnim(kBackward, 0, 255, actorIdx);
			_heroMoved = 1;
		}

		if (g_twin->getKey(KeyLeft)) { // turn left
			_heroMoved = 1;
			if (_anim == 0) {
				setAnimation(kTurnLeft);
				//initAnim(kTurnLeft, 0, 255, actorIdx);
			} else {
				//if (!actor->dynamicFlags.bIsRotationByAnim) {
					_angle += -2;//getRealAngle(&actor->move);
				//}
			}
		}

		if (g_twin->getKey(KeyRight)) { // turn right
			_heroMoved = 1;
			if (_anim == 0) {
				setAnimation(kTurnRight);
				//initAnim(kTurnRight, 0, 255, actorIdx);
			} else {
				//if (!actor->dynamicFlags.bIsRotationByAnim) {
					_angle += 2;//getRealAngle(&actor->move);
				//}
			}
		}

		tempAngle = 0;

/*		if (key & 4) {
			tempAngle = 0x100;
		}

		if (key & 8) {
			tempAngle = -0x100;
		}

		moveActor(actor->angle, actor->angle + tempAngle, actor->speed, &actor->move);

		heroPressedKey  = key;
		heroPressedKey2 = loopPressedKey;*/
	}
}

} // end of namespace Twin
