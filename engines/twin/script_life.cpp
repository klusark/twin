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
#include "common/textconsole.h"

#include "engines/twin/script_life.h"
#include "engines/twin/script_track.h"
#include "engines/twin/actor.h"
#include "engines/twin/player.h"
#include "engines/twin/scene.h"
#include "engines/twin/twin.h"
#include "engines/twin/resource.h"
#include "engines/twin/animation.h"

namespace Twin {



ScriptLife::ScriptLife(Common::SeekableReadStream *stream, ScriptTrack *track) : 
	Script(stream), _track(track), _comportementAddress(0), _savedTrack(0) {

}

bool ScriptLife::testCond(uint16 b, uint16 a, byte oper) {
	switch (oper) {
	case 0:
		return a == b;
	case 1:
		return a > b;
	case 2:
		return a < b;
	case 3:
		return a >= b;
	case 4:
		return a <= b;
	case 5:
		return a != b;
	default:
		warning("Unknown operator");
	}
	return false;
}

bool ScriptLife::checkCondition() {
	byte cond = getParamByte();
	loadFuncConditionParam(cond);
	return checkFuncCondition(cond);
}

void ScriptLife::loadConditionParam() {
	byte param = getParamByte();
	_currentState._param = param;
}

bool ScriptLife::COL(byte oper) {
	byte actor = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	return a->collidesWith(_actor);
}

bool ScriptLife::COL_OBJ(byte oper) {
	byte actor2 = getParamByte();
	byte actor1 = _currentState._param;
	Scene *s = g_twin->getCurrentScene();
	Actor *a1 = s->getActor(actor1);
	Actor *a2 = s->getActor(actor2);
	return a1->collidesWith(a2);
}

bool ScriptLife::DISTANCE(byte oper) {
	uint16 distance = getParamUint16();
	byte actor = _currentState._param;
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	uint16 actualDistance = a->_pos.getDistanceTo(&_actor->_pos);
	return testCond(distance, actualDistance, oper);
}

bool ScriptLife::ZONE(byte oper) {
	byte zone = getParamByte();
	Scene *s = g_twin->getCurrentScene();

	Zone *currZone = _actor->getCurrentZone();
	if (!currZone) {
		return false;
	}
	return testCond(zone, currZone->getId(), oper);
}

bool ScriptLife::ZONE_OBJ(byte oper) {
	byte actor = _currentState._param;
	byte zone = getParamByte();

	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	Zone *currZone = a->getCurrentZone();
	if (!currZone) {
		return false;
	}
	return testCond(zone, currZone->getId(), oper);
}

bool ScriptLife::BODY_COND(byte oper) {
	byte body = getParamByte();
	return testCond(body, _actor->_body, oper);
}

bool ScriptLife::BODY_OBJ_COND(byte oper) {
	byte actor = _currentState._param;
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	byte body = getParamByte();
	return testCond(body, a->_body, oper);
}

bool ScriptLife::ANIM_COND(byte oper) {
	byte body = getParamByte();
	return testCond(body, _actor->_entity->_anim->getId(), oper);
}

bool ScriptLife::ANIM_OBJ_COND(byte oper) {
	byte actor = _currentState._param;
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	byte body = getParamByte();
	return testCond(body, a->_entity->_anim->getId(), oper);
}

bool ScriptLife::CURRENT_TRACK(byte oper) {
	byte track = getParamByte();
	return testCond(track, _track->getLabel(), oper);
}

bool ScriptLife::CURRENT_TRACK_OBJ(byte oper) {
	byte actor = _currentState._param;
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	byte track = getParamByte();
	return testCond(track, ((ScriptTrack *)a->_trackScript)->getLabel(), oper);
}

bool ScriptLife::VAR_CUBE(byte oper) {
	byte varID = _currentState._param;
	byte value = getParamByte();
	byte varVal = getCubeVar(varID);
	return testCond(value, varVal, oper);
}

bool ScriptLife::CONE_VIEW(byte oper) {
	byte actor = _currentState._param;
	uint16 value = getParamUint16();
	return false;
}

bool ScriptLife::HIT_BY(byte oper) {
	byte actor = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	return a == _actor->_lastHitBy;
}

bool ScriptLife::ACTION(byte oper) {
	byte val = getParamByte();
	byte actual = g_twin->getKey(KeyAction);
	return testCond(val, actual, oper);
}

bool ScriptLife::VAR_GAME(byte oper) {
	byte varID = _currentState._param;
	uint16 value = getParamUint16();
	uint16 varVal = getGameVar(varID);
	return testCond(value, varVal, oper);
}

bool ScriptLife::LIFE_POINT(byte oper) {
	uint16 value = getParamUint16();
	return testCond(value, _actor->getLifePoints(), oper);
}

bool ScriptLife::LIFE_POINT_OBJ(byte oper) {
	byte actor = _currentState._param;
	uint16 value = getParamUint16();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	return testCond(value, a->getLifePoints(), oper);
}

bool ScriptLife::NUM_LITTLE_KEYS(byte oper) {
	byte numKeys = getParamByte();
	return testCond(numKeys, g_twin->getPlayer()->getNumKeys(), oper);
}

bool ScriptLife::NUM_GOLD_PIECES(byte oper) {
	uint16 numGold = getParamUint16();
	return testCond(numGold, _actor->getNumGold(), oper);
}

bool ScriptLife::BEHAVIOUR(byte oper) {
	byte numKeys = getParamByte();
	return false;
}

bool ScriptLife::CHAPTER(byte oper) {
	byte value = getParamByte();
	return testCond(value, getChapter(), oper);
}

bool ScriptLife::DISTANCE_3D(byte oper) {
	uint16 value = getParamUint16();
	return false;
}

bool ScriptLife::USE_INVERNTORY(byte oper) {
	byte param1 = _currentState._param;
	byte param2 = getParamByte();
	return false;
}

bool ScriptLife::CARRIED_BY(byte oper) {
	byte param1 = getParamByte();
	return false;
}

void ScriptLife::END() {
	stop();
}

void ScriptLife::SNIF() {
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (cond) {
		jumpAddress(address);
	} else {
		setOpcode(0x0D); // SWIF
	}
}

void ScriptLife::NEVERIF() {
	bool cond = checkCondition();
	uint16 address = getParamUint16();
	jumpAddress(address);
}

void ScriptLife::RETURN() {
	jumpAddress(_comportementAddress);
}

void ScriptLife::IF() {
	_states.push(_currentState);
	_currentState._isInSwitch = false;
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (!cond) {
		jumpAddress(address);
	}
	_currentState = _states.pop();
}

void ScriptLife::SWIF() {
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (!cond) {
		jumpAddress(address);
	} else {
		//setOpcode(0x02); // SNIF
	}
}

void ScriptLife::ONEIF() {
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (!cond) {
		jumpAddress(address);
	} else {
		setOpcode(0x04); // NEVERIF
	}
}

void ScriptLife::ELSE() {
	uint16 address = getParamUint16();
	jumpAddress(address);
}


void ScriptLife::BODY() {
	byte body = getParamByte();
	_actor->setBody(body);
}

void ScriptLife::BODY_OBJ() {
	byte actor = getParamByte();
	byte body = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	a->setBody(body);
}

void ScriptLife::ANIM() {
	uint32 id = getParamUint16();
	_actor->setAnimation(id);
}

void ScriptLife::ANIM_OBJ() {
	byte actor = getParamByte();
	uint32 id = getParamUint16();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	a->setAnimation(id);
}

void ScriptLife::SET_TRACK() {
	uint16 address = getParamUint16();
	_track->jumpAddress(address);
	_track->start();
}

void ScriptLife::SET_TRACK_OBJ() {
	byte actor = getParamByte();
	uint16 address = getParamUint16();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	a->_trackScript->jumpAddress(address);
	a->_trackScript->start();
}

void ScriptLife::MESSAGE() {
	uint16 id = getParamUint16();
}

void ScriptLife::CAN_FALL() {
	byte canfall = getParamByte();
}

void ScriptLife::CAM_FOLLOW() {
	byte actor = getParamByte();
}

void ScriptLife::SET_BEHAVIOUR() {
	byte param = getParamByte();
}

void ScriptLife::SET_VAR_CUBE() {
	byte id = getParamByte();
	byte value = getParamByte();
	setCubeVar(id, value);
}

void ScriptLife::SET_DIRMODE() {
	byte dirmode = getParamByte();
	if (dirmode == 2 || dirmode == 4) {
		byte actor = getParamByte();
		Scene *s = g_twin->getCurrentScene();
		Actor *a = s->getActor(actor);
		_actor->faceActor(a);
	} else {
		_actor->faceActor(nullptr);
	}
}

void ScriptLife::SET_DIRMODE_OBJ() {
	byte actor = getParamByte();
	byte dirmode = getParamByte();
	if (dirmode == 2 || dirmode == 4) {
		byte actor = getParamByte();
	}
}

void ScriptLife::SET_COMPORTEMENT() {
	uint16 address = getParamUint16();
	_comportementAddress = address;
}

void ScriptLife::SET_COMPORTEMENT_OBJ() {
	byte actor = getParamByte();
	uint16 address = getParamUint16();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	static_cast<ScriptLife *>(a->_lifeScript)->_comportementAddress = address;
}

void ScriptLife::END_COMPORTEMENT() {
	jumpAddress(_comportementAddress);
	yield();
}

void ScriptLife::SET_VAR_GAME() {
	byte var = getParamByte();
	uint16 value = getParamUint16();
	setGameVar(var, value);
}

void ScriptLife::KILL_OBJ() {
	byte actor = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	a->kill();
}

void ScriptLife::SUICIDE() {
	_actor->kill();
}

void ScriptLife::USE_ONE_LITTLE_KEY() {
	_actor->_numKeys -= 1;
}

void ScriptLife::GIVE_GOLD_PIECES() {
	int16 gold = getParamInt16();
}

void ScriptLife::END_LIFE() {
	stop();
}

void ScriptLife::STOP_CURRENT_TRACK() {
	_savedTrack = _track->getLabelAddress();
	_actor->resetActions();
}

void ScriptLife::RESTORE_LAST_TRACK() {
	_track->jumpAddress(_savedTrack);
	_track->start();
}

void ScriptLife::MESSAGE_OBJ() {
	byte actor = getParamByte();
	uint16 textid = getParamUint16();
}

void ScriptLife::INC_CHAPTER() {
	++_chapter;
}

void ScriptLife::FOUND_OBJECT() {
	byte item = getParamByte();
}

void ScriptLife::SET_DOOR_LEFT() {
	int16 distance = getParamInt16();
}

void ScriptLife::SET_DOOR_RIGHT() {
	int16 distance = getParamInt16();
}

void ScriptLife::SET_DOOR_UP() {
	int16 distance = getParamInt16();
}

void ScriptLife::SET_DOOR_DOWN() {
	int16 distance = getParamInt16();
}

void ScriptLife::GIVE_BONUS() {
	byte unknown = getParamByte();
}

void ScriptLife::SET_LIFE_POINT_OBJ() {
	byte actor = getParamByte();
	byte life = getParamByte();
}

} // end of namespace Twin
