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
#include "engines/twin/player.h"
#include "engines/twin/scene.h"
#include "engines/twin/twin.h"
#include "engines/twin/resource.h"
#include "engines/twin/animation.h"

namespace Twin {



ScriptLifeV2::ScriptLifeV2(Common::SeekableReadStream *stream, ScriptTrack *track) : 
	ScriptLife(stream, track) {

}


void ScriptLifeV2::execute(byte opcode) {
	switch (opcode) {
		#define OPCODE(op, func) case op: func(); break
		#define OPCODEI(op, func) OPCODE(op, func)
			LIFE_OPCODES_V2
		#undef OPCODE
		#undef OPCODEI
	default:
		warning("asdf");
	};
}


void ScriptLifeV2::loadFuncConditionParam(byte cond) {
	switch (cond) {
		#define COND_OPCODE(op, func, param) case op: if (param != 0) { loadConditionParam(); } break
		#define COND_OPCODEI(op, func, param) COND_OPCODE(op, func, param)
			LIFE_COND_OPCODES_V2
		#undef COND_OPCODE
		#undef COND_OPCODEI

	default:
		warning("asdf");
	}
}

bool ScriptLifeV2::checkFuncCondition(byte cond) {
	byte oper = getParamByte();
	switch (cond) {
		#define COND_OPCODE(op, func, params) case op: return func(oper)
		#define COND_OPCODEI(op, func, param) COND_OPCODE(op, func, param)
			LIFE_COND_OPCODES_V2
		#undef COND_OPCODE
		#undef COND_OPCODEI

	default:
		warning("asdf");
	}
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
	return true;
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

bool ScriptLifeV2::COL_DECORS(byte oper) {
	byte param1 = getParamByte();
	return false;
}

bool ScriptLifeV2::COL_DECORS_OBJ(byte oper) {
	byte param1 = getParamByte();
	return false;
}

bool ScriptLifeV2::PROCESSOR(byte oper) {
	byte param1 = getParamByte();
	return true;
}

bool ScriptLifeV2::OBJECT_DISPLAYED(byte oper) {
	byte param1 = getParamByte();
	return false;
}

bool ScriptLifeV2::ANGLE_OBJ(byte oper) {
	byte param1 = getParamByte();
	return false;
}

#define STUB_SCRIPT(func) void ScriptLifeV2::func() { }



void ScriptLifeV2::BODY() {
	byte body = getParamByte();
	_actor->setBody(body);
}

void ScriptLifeV2::BODY_OBJ() {
	byte actor = getParamByte();
	byte body = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	a->setBody(body);
}

void ScriptLifeV2::ANIM() {
	uint32 id = getParamUint16();
	_actor->setAnimation(id);
}

void ScriptLifeV2::ANIM_OBJ() {
	byte actor = getParamByte();
	uint32 id = getParamUint16();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	a->setAnimation(id);
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
	uint16 address = getParamUint16();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	a->_trackScript->jumpAddress(address);
	a->_trackScript->start();
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
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	static_cast<ScriptLifeV2 *>(a->_lifeScript)->_comportementAddress = address;
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
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	a->kill();
}

void ScriptLifeV2::SUICIDE() {
	_actor->kill();
}

void ScriptLifeV2::USE_ONE_LITTLE_KEY() {
	_actor->_numKeys -= 1;
}

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

void ScriptLifeV2::INC_CHAPTER() {
	++_chapter;
}

void ScriptLifeV2::FOUND_OBJECT() {
	byte item = getParamByte();
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

void ScriptLifeV2::OBJ_COL() {
	byte unknown = getParamByte();
}

void ScriptLifeV2::BRICK_COL() {
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
	_actor->setInvisible(state);
}

void ScriptLifeV2::SHADOW_OBJ() {
	byte param1 = getParamByte();
	byte param2 = getParamByte();
}

void ScriptLifeV2::POS_POINT() {
	byte point = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Point *p = s->getPoint(point);
	_actor->setPos(p->_x, p->_y, p->_z);
}

void ScriptLifeV2::HIT_OBJ() {
	byte actor = getParamByte();
	byte damage = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	a->hitBy(_actor, damage);
}

void ScriptLifeV2::PLAY_ACF() {
	const char * s = getParamString();
	warning("PLAY_ACF: %s", s);
}

void ScriptLifeV2::SET_HOLO_POS() {
	byte id = getParamByte();
}

void ScriptLifeV2::SET_GRM() {
	byte param1 = getParamByte();
	byte param2 = getParamByte();
}

void ScriptLifeV2::SET_CHANGE_CUBE() {
	uint16 param = getParamUint16();
}

void ScriptLifeV2::BETA() {
	uint16 angle = getParamUint16();
	_actor->setAngle((360 + 90)-((angle * 360) / 4096.f));
}

void ScriptLifeV2::SET_FRAME() {
	byte sprite = getParamByte();
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

void ScriptLifeV2::TRACK_TO_VAR_GAME() {
	byte var = getParamByte();
	setGameVar(var, _track->getLabelAddress());
}

void ScriptLifeV2::VAR_GAME_TO_TRACK() {
	byte var = getParamByte();
	_track->jumpAddress(getGameVar(var));
}

void ScriptLifeV2::ANIM_TEXTURE() {
	byte param = getParamByte();
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
	loadFuncConditionParam(_currentState._switchCond);

	_currentState._isInSwitch = true;
}
void ScriptLifeV2::OR_CASE() {
	uint16 address = getParamUint16();
	if (!checkFuncCondition(_currentState._switchCond)) {
		jumpAddress(address);
	} else {
		_currentState._orCase = true;
	}
}
void ScriptLifeV2::CASE() {
	uint16 address = getParamUint16();
	bool cond = checkFuncCondition(_currentState._switchCond);
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

void ScriptLifeV2::SAMPLE_RND() {
	uint16 id = getParamUint16();
}

void ScriptLifeV2::SAMPLE_ALWAYS() {
	uint16 id = getParamUint16();
}

void ScriptLifeV2::SAMPLE_STOP() {
	uint16 id = getParamUint16();
}

void ScriptLifeV2::REPEAT_SAMPLE() {
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

void ScriptLifeV2::SET_RAIL() {
	byte param1 = getParamByte();
	byte param2 = getParamByte();
}

void ScriptLifeV2::NO_BODY() {
	//TODO: Probably shouldn't kill the actor, but that hides them for now
	_actor->kill();
}

void ScriptLifeV2::FLOW_POINT() {
	byte param1 = getParamByte();
	byte param2 = getParamByte();
}

void ScriptLifeV2::FLOW_OBJ() {
	byte param1 = getParamByte();
	byte param2 = getParamByte();
}

void ScriptLifeV2::SET_ANIM_DIAL() {
	uint16 unknown = getParamUint16();
}

void ScriptLifeV2::NEW_SAMPLE() {
	uint16 param1 = getParamUint16();
	uint16 param2 = getParamUint16();
	byte param3 = getParamByte();
	uint16 param4 = getParamUint16();
}

} // end of namespace Twin
