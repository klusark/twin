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

#include "engines/twin/script_life_v2.h"
#include "engines/twin/script_track_v2.h"
#include "engines/twin/actor.h"
#include "engines/twin/scene.h"
#include "engines/twin/twin.h"

namespace Twin {



ScriptLifeV2::ScriptLifeV2(Common::SeekableReadStream *stream, ScriptTrackV2 *track) : 
	Script(stream), _track(track), _comportementAddress(0), _savedTrack(0) {

}

bool ScriptLifeV2::testCond(uint16 b, uint16 a, byte oper) {
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

void ScriptLifeV2::execute(byte opcode) {
	switch (opcode) {
		#define OPCODE(op, func) case op: func(); break
			LIFE_OPCODES
		#undef OPCODE
	default:
		warning("asdf");
	};
}
bool ScriptLifeV2::checkCondition() {
	byte cond = getParamByte();
	loadConditionParam(cond);
	return checkCondition(cond);
}

void ScriptLifeV2::loadConditionParam() {
	byte param = getParamByte();
	_currentState._param = param;
}

void ScriptLifeV2::loadConditionParam(byte cond) {
	switch (cond) {
		#define COND_OPCODE(op, func, param) case op: if (param != 0) { loadConditionParam(); } break
			LIFE_COND_OPCODES
		#undef COND_OPCODE

	default:
		warning("asdf");
	}
}

bool ScriptLifeV2::checkCondition(byte cond) {
	byte oper = getParamByte();
	switch (cond) {
		#define COND_OPCODE(op, func, params) case op: return func(oper)
			LIFE_COND_OPCODES
		#undef COND_OPCODE

	default:
		warning("asdf");
	}
	return false;
}

bool ScriptLifeV2::COL(byte oper) {
	byte actor = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	return a->collidesWith(_actor);
}

bool ScriptLifeV2::COL_OBJ(byte oper) {
	byte actor2 = getParamByte();
	return false;
}

bool ScriptLifeV2::DISTANCE(byte oper) {
	uint16 distance = getParamUint16();
	byte actor = _currentState._param;
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	uint16 actualDistance = a->_pos.getDistanceTo(&_actor->_pos);
	return testCond(distance, actualDistance, oper);
}

bool ScriptLifeV2::ZONE(byte oper) {
	byte zone = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Zone *z = s->getZone(zone + 1);
	return z->isActorInside(_actor);
}

bool ScriptLifeV2::ZONE_OBJ(byte oper) {
	byte actor = _currentState._param;
	byte zone = getParamByte();

	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	Zone *z = s->getZone(zone);
	return z->isActorInside(a);
}

bool ScriptLifeV2::CURRENT_TRACK(byte oper) {
	byte track = getParamByte();
	return testCond(track, _track->getLabel(), oper);
}

bool ScriptLifeV2::CURRENT_TRACK_OBJ(byte oper) {
	byte actor = _currentState._param;
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	byte track = getParamByte();
	return testCond(track, ((ScriptTrackV2 *)a->_trackScript)->getLabel(), oper);
}

bool ScriptLifeV2::VAR_CUBE(byte oper) {
	byte varID = _currentState._param;
	byte value = getParamByte();
	byte varVal = getCubeVar(varID);
	return testCond(value, varVal, oper);
}

bool ScriptLifeV2::CONE_VIEW(byte oper) {
	byte actor = _currentState._param;
	uint16 value = getParamUint16();
	return false;
}

bool ScriptLifeV2::HIT_BY(byte oper) {
	byte actor = getParamByte();
	return false;
}
bool ScriptLifeV2::ACTION(byte oper) {
	byte val = getParamByte();
	byte actual = g_twin->isPressingAction();
	return testCond(val, actual, oper);
}
bool ScriptLifeV2::VAR_GAME(byte oper) {
	byte varID = _currentState._param;
	uint16 value = getParamUint16();
	uint16 varVal = getGameVar(varID);
	return testCond(value, varVal, oper);
}
bool ScriptLifeV2::LIFE_POINT(byte oper) {
	uint16 value = getParamUint16();
	return false;
}
bool ScriptLifeV2::LIFE_POINT_OBJ(byte oper) {
	byte actor = _currentState._param;
	uint16 value = getParamUint16();
	return false;
}

bool ScriptLifeV2::NUM_LITTLE_KEYS(byte oper) {
	byte numKeys = getParamByte();
	return true;
}

bool ScriptLifeV2::NUM_GOLD_PIECES(byte oper) {
	uint16 numGold = getParamUint16();
	return false;
}

bool ScriptLifeV2::BEHAVIOUR(byte oper) {
	byte numKeys = getParamByte();
	return false;
}

bool ScriptLifeV2::CHAPTER(byte oper) {
	byte value = getParamByte();
	return testCond(value, 0, oper);
}

bool ScriptLifeV2::DISTANCE_3D(byte oper) {
	uint16 value = getParamUint16();
	return false;
}

bool ScriptLifeV2::USE_INVERNTORY(byte oper) {
	byte param1 = _currentState._param;
	byte param2 = getParamByte();
	return false;
}

bool ScriptLifeV2::CARRIED_BY(byte oper) {
	byte param1 = getParamByte();
	return false;
}

bool ScriptLifeV2::RND(byte oper) {
	byte value = getParamByte();
	return false;
}

bool ScriptLifeV2::RAIL(byte oper) {
	byte value = getParamByte();
	return false;
}

bool ScriptLifeV2::BETA_COND(byte oper) {
	uint16 value = getParamUint16();
	return false;
}

bool ScriptLifeV2::BETA_OBJ(byte oper) {
	uint16 value = getParamUint16();
	return false;
}

bool ScriptLifeV2::CARRIED_OBJ_BY(byte oper) {
	byte param1 = _currentState._param;
	byte param2 = getParamByte();
	return false;
}

bool ScriptLifeV2::ANGLE(byte oper) {
	byte param1 = _currentState._param;
	int16 param2 = getParamInt16();
	return false;
}

bool ScriptLifeV2::DISTANCE_MESSAGE(byte oper) {
	byte param1 = _currentState._param;
	uint16 param2 = getParamUint16();
	return false;
}

bool ScriptLifeV2::HIT_OBJ_BY(byte oper) {
	byte param1 = _currentState._param;
	byte param2 = getParamByte();
	return false;
}

bool ScriptLifeV2::PROCESSOR(byte oper) {
	byte param1 = getParamByte();
	return true;
}

#define STUB_SCRIPT(func) void ScriptLifeV2::func() { }

void ScriptLifeV2::END() {
	stop();
}

void ScriptLifeV2::SNIF() {
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (cond) {
		jumpAddress(address);
	} else {
		setOpcode(0x0D); // SWIF
	}
}

void ScriptLifeV2::NEVERIF() {
	bool cond = checkCondition();
	uint16 address = getParamUint16();
	jumpAddress(address);
}

void ScriptLifeV2::RETURN() {
	jumpAddress(_comportementAddress);
}

void ScriptLifeV2::IF() {
	_states.push(_currentState);
	_currentState._isInSwitch = false;
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (!cond) {
		jumpAddress(address);
	}
	_currentState = _states.pop();
}
void ScriptLifeV2::SWIF() {
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (!cond) {
		jumpAddress(address);
	} else {
		setOpcode(0x02); // SNIF
	}
}

void ScriptLifeV2::ONEIF() {
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (!cond) {
		jumpAddress(address);
	} else {
		setOpcode(0x04); // NEVERIF
	}
}

void ScriptLifeV2::ELSE() {
	uint16 address = getParamUint16();
	jumpAddress(address);
}

void ScriptLifeV2::BODY() {
	byte actor = getParamByte();
}

void ScriptLifeV2::BODY_OBJ() {
	byte actor = getParamByte();
	byte body = getParamByte();
}

void ScriptLifeV2::ANIM() {
	uint32 id = getParamUint16();
	_actor->setAnimation(id);
}

void ScriptLifeV2::ANIM_OBJ() {
	byte actor = getParamByte();
	uint32 id = getParamUint16();
}

void ScriptLifeV2::SET_CAMERA() {
	uint16 unknown = getParamUint16();
}

void ScriptLifeV2::CAMERA_CENTER() {
	byte unknown = getParamByte();
}

void ScriptLifeV2::SET_TRACK() {
	uint16 address = getParamUint16();
	_track->jumpAddress(address);
	_track->start();
}

void ScriptLifeV2::SET_TRACK_OBJ() {
	byte actor = getParamByte();
	uint16 id = getParamUint16();
}

void ScriptLifeV2::MESSAGE() {
	uint16 id = getParamUint16();
}

void ScriptLifeV2::CAN_FALL() {
	byte canfall = getParamByte();
}

void ScriptLifeV2::CAM_FOLLOW() {
	byte actor = getParamByte();
}

void ScriptLifeV2::SET_BEHAVIOUR() {
	byte param = getParamByte();
}

void ScriptLifeV2::SET_VAR_CUBE() {
	byte id = getParamByte();
	byte value = getParamByte();
	setCubeVar(id, value);
}

void ScriptLifeV2::SET_DIRMODE() {
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

void ScriptLifeV2::SET_DIRMODE_OBJ() {
	byte actor = getParamByte();
	byte dirmode = getParamByte();
	if (dirmode == 2 || dirmode == 4) {
		byte actor = getParamByte();
	}
}

void ScriptLifeV2::SET_COMPORTEMENT() {
	uint16 address = getParamUint16();
	_comportementAddress = address;
}
void ScriptLifeV2::SET_COMPORTEMENT_OBJ() {
	byte actor = getParamByte();
	uint16 address = getParamUint16();
}

void ScriptLifeV2::END_COMPORTEMENT() {
	jumpAddress(_comportementAddress);
	yield();
}

void ScriptLifeV2::SET_VAR_GAME() {
	byte var = getParamByte();
	uint16 value = getParamUint16();
	setGameVar(var, value);
}

void ScriptLifeV2::KILL_OBJ() {
	byte actor = getParamByte();
}

void ScriptLifeV2::SUICIDE() {
	_actor->kill();
}

STUB_SCRIPT(USE_ONE_LITTLE_KEY);

void ScriptLifeV2::GIVE_GOLD_PIECES() {
	int16 gold = getParamInt16();
}

void ScriptLifeV2::END_LIFE() {
	stop();
}

void ScriptLifeV2::STOP_CURRENT_TRACK() {
	_savedTrack = _track->getLabelAddress();
	_actor->resetActions();
}

void ScriptLifeV2::RESTORE_LAST_TRACK() {
	_track->jumpAddress(_savedTrack);
	_track->start();
}

void ScriptLifeV2::MESSAGE_OBJ() {
	byte actor = getParamByte();
	uint16 textid = getParamUint16();
}

void ScriptLifeV2::SET_DOOR_LEFT() {
	int16 distance = getParamInt16();
}

void ScriptLifeV2::SET_DOOR_RIGHT() {
	int16 distance = getParamInt16();
}

void ScriptLifeV2::SET_DOOR_UP() {
	int16 distance = getParamInt16();
}

void ScriptLifeV2::SET_DOOR_DOWN() {
	int16 distance = getParamInt16();
}

void ScriptLifeV2::GIVE_BONUS() {
	byte unknown = getParamByte();
}

void ScriptLifeV2::OR_IF() {
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (cond) {
		jumpAddress(address);
	}
}

void ScriptLifeV2::INVISIBLE() {
	byte state = getParamByte();
}

void ScriptLifeV2::POS_POINT() {
	byte point = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Point *p = s->getPoint(point);
	_actor->setPos(p->_x, p->_y, p->_z);
}


void ScriptLifeV2::SET_HOLO_POS() {
	byte id = getParamByte();
}

void ScriptLifeV2::BETA() {
	uint16 angle = getParamUint16();
	_actor->setAngle((360 + 90)-((angle * 360) / 4096.f));
}

void ScriptLifeV2::SET_FRAME() {
	uint16 sprite = getParamUint16();
}

void ScriptLifeV2::SET_SPRITE() {
	byte frame = getParamByte();
}

void ScriptLifeV2::NO_SHOCK() {
	byte shock = getParamByte();
}

void ScriptLifeV2::SAVE_HERO() {
}

void ScriptLifeV2::RESTORE_HERO() {
}

void ScriptLifeV2::ANIM_SET() {
	uint16 anim = getParamUint16();
}

void ScriptLifeV2::CINEMA_MODE() {
	byte val = getParamByte();
}

void ScriptLifeV2::AND_IF() {
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (!cond) {
		jumpAddress(address);
	}
}

void ScriptLifeV2::SWITCH() {
	_states.push(_currentState);

	_currentState._switchCond = getParamByte();
	loadConditionParam(_currentState._switchCond);

	_currentState._isInSwitch = true;
}
void ScriptLifeV2::OR_CASE() {
	uint16 address = getParamUint16();
	if (!checkCondition(_currentState._switchCond)) {
		jumpAddress(address);
	} else {
		_currentState._orCase = true;
	}
}
void ScriptLifeV2::CASE() {
	uint16 address = getParamUint16();
	bool cond = checkCondition(_currentState._switchCond);
	if (!_currentState._orCase && !cond) {
		jumpAddress(address);
	}
	_currentState._orCase = false;
}
void ScriptLifeV2::DEFAULT() {
	// do nothing
}
void ScriptLifeV2::BREAK() {
	uint16 address = getParamUint16();
	jumpAddress(address);
}
void ScriptLifeV2::END_SWITCH() {
	_currentState = _states.pop();
}

void ScriptLifeV2::SET_HIT_ZONE() {
	byte param1 = getParamByte();
	byte param2 = getParamByte();
}

void ScriptLifeV2::SAVE_COMPORTEMENT() {
}

void ScriptLifeV2::RESTORE_COMPORTEMENT() {
}

void ScriptLifeV2::SAMPLE() {
	uint16 id = getParamUint16();
}

void ScriptLifeV2::BACKGROUND() {
	byte id = getParamByte();
}

void ScriptLifeV2::ADD_VAR_GAME() {
	byte var = getParamByte();
	uint16 value = getParamUint16();
	uint16 val = getGameVar(var);
	setGameVar(var, val + value);
}
void ScriptLifeV2::SUB_VAR_GAME() {
	byte var = getParamByte();
	uint16 value = getParamUint16();
	uint16 val = getGameVar(var);
	setGameVar(var, val - value);
}

void ScriptLifeV2::NO_BODY() {
	//TODO: Probably shouldn't kill the actor, but that hides them for now
	_actor->kill();
}

void ScriptLifeV2::FLOW_POINT() {
	byte param1 = getParamByte();
	byte param2 = getParamByte();
}

void ScriptLifeV2::SET_ANIM_DIAL() {
	uint16 unknown = getParamUint16();
}

} // end of namespace Twin