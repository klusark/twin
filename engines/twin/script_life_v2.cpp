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


#define OPCODE(op, func) case op: func(); break
#define COND_OPCODE(op, func) case op: return func()

ScriptLifeV2::ScriptLifeV2(Common::SeekableReadStream *stream, ScriptTrackV2 *track) : 
	Script(stream), _isInSwitch(false), _track(track), _comportementAddress(0) {

}

bool ScriptLifeV2::testCond(uint16 a, uint16 b, byte oper) {
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
	warning("%x", opcode);

	switch (opcode) {
		OPCODE(0x00, END);

		OPCODE(0x0C, IF);
		OPCODE(0x0D, SWIF);
		OPCODE(0x0E, ONEIF);
		OPCODE(0x0F, ELSE);

		OPCODE(0x11, BODY);
		OPCODE(0x12, BODY_OBJ);
		OPCODE(0x13, ANIM);
		OPCODE(0x14, ANIM_OBJ);
		OPCODE(0x15, SET_CAMERA);
		OPCODE(0x16, CAMERA_CENTER);
		OPCODE(0x17, SET_TRACK);
		OPCODE(0x18, SET_TRACK_OBJ);
		OPCODE(0x19, MESSAGE);

		OPCODE(0x1B, SET_DIRMODE);
		OPCODE(0x1C, SET_DIRMODE_OBJ);
		OPCODE(0x1D, CAM_FOLLOW);
		OPCODE(0x1E, SET_BEHAVIOUR);
		OPCODE(0x1F, SET_VAR_CUBE);

		OPCODE(0x21, SET_COMPORTEMENT);
		OPCODE(0x22, SET_COMPORTEMENT_OBJ);
		OPCODE(0x23, END_COMPORTEMENT);
		OPCODE(0x24, SET_VAR_GAME);
		OPCODE(0x25, KILL_OBJ);
		OPCODE(0x26, SUICIDE);
		OPCODE(0x27, USE_ONE_LITTLE_KEY);
		OPCODE(0x28, GIVE_GOLD_PIECES);
		OPCODE(0x29, END_LIFE);

		OPCODE(0x33, GIVE_BONUS);

		OPCODE(0x37, OR_IF);

		OPCODE(0x3A, POS_POINT);

		OPCODE(0x48, SET_HOLO_POS);

		OPCODE(0x50, BETA);

		OPCODE(0x5C, CINEMA_MODE);
		OPCODE(0x5D, SAVE_HERO);
		OPCODE(0x5E, RESTORE_HERO);

		OPCODE(0x70, AND_IF);
		OPCODE(0x71, SWITCH);
		OPCODE(0x72, OR_CASE);
		OPCODE(0x73, CASE);
		OPCODE(0x74, DEFAULT);
		OPCODE(0x75, BREAK);
		OPCODE(0x76, END_SWITCH);
		OPCODE(0x77, SET_HIT_ZONE);

		OPCODE(0x7A, SAMPLE);

		OPCODE(0x80, ADD_VAR_GAME);
		OPCODE(0x81, SUB_VAR_GAME);

		OPCODE(0x93, SET_ANIM_DIAL);
	default:
		warning("asdf");
	};
}
bool ScriptLifeV2::checkCondition() {
	byte cond = getParamByte();
	return checkCondition(cond);
}

bool ScriptLifeV2::checkCondition(byte cond) {

	switch (cond) {
		COND_OPCODE(0x00, COL);
		COND_OPCODE(0x01, COL_OBJ);
		COND_OPCODE(0x02, DISTANCE);
		COND_OPCODE(0x03, ZONE);
		COND_OPCODE(0x04, ZONE_OBJ);

		COND_OPCODE(0x09, CURRENT_TRACK);
		COND_OPCODE(0x0A, CURRENT_TRACK_OBJ);
		COND_OPCODE(0x0B, VAR_CUBE);
		COND_OPCODE(0x0C, CONE_VIEW);
		COND_OPCODE(0x0D, HIT_BY);
		COND_OPCODE(0x0E, ACTION);
		COND_OPCODE(0x0F, VAR_GAME);
		COND_OPCODE(0x10, LIFE_POINT);
		COND_OPCODE(0x11, LIFE_POINT_OBJ);
		COND_OPCODE(0x12, NUM_LITTLE_KEYS);
		COND_OPCODE(0x13, NUM_GOLD_PIECES);
		COND_OPCODE(0x14, BEHAVIOUR);
		COND_OPCODE(0x15, CHAPTER);

		COND_OPCODE(0x24, ANGLE);
		COND_OPCODE(0x25, DISTANCE_MESSAGE);

	default:
		warning("asdf");
	}
	return false;
}

bool ScriptLifeV2::COL() {
	byte oper = getParamByte();
	byte actor = getParamByte();
	return false;
}

bool ScriptLifeV2::COL_OBJ() {
	byte oper = getParamByte();
	byte actor1 = getParamByte();
	byte actor2 = getParamByte();
	return false;
}

bool ScriptLifeV2::DISTANCE() {
	byte oper = getParamByte();
	byte actor = getParamByte();
	uint16 distance = getParamUint16();
	return false;
}

bool ScriptLifeV2::ZONE() {
	byte oper = getParamByte();
	byte zone = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Zone *z = s->getZone(zone);
	return z->isActorInside(_actor);
}

bool ScriptLifeV2::ZONE_OBJ() {
	byte oper = getParamByte();
	byte actor = getParamByte();
	byte zone = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Actor *a = s->getActor(actor);
	Zone *z = s->getZone(zone);
	return z->isActorInside(a);
}

bool ScriptLifeV2::CURRENT_TRACK() {
	byte oper = getParamByte();
	byte track = getParamByte();
	return testCond(track, _track->getLabel(), oper);
}

bool ScriptLifeV2::CURRENT_TRACK_OBJ() {
	byte oper = getParamByte();
	byte actor = getParamByte();
	byte track = getParamByte();
	return false;
}

bool ScriptLifeV2::VAR_CUBE() {
	byte oper = getParamByte();
	byte var = getParamByte();
	byte val = getParamByte();
	return false;
}

bool ScriptLifeV2::CONE_VIEW() {
	byte oper = getParamByte();
	byte actor = getParamByte();
	uint16 value = getParamUint16();
	return false;
}

bool ScriptLifeV2::HIT_BY() {
	byte oper = getParamByte();
	byte actor = getParamByte();
	return false;
}
bool ScriptLifeV2::ACTION() {
	byte oper = getParamByte();
	byte actor = getParamByte();
	return false;
}
bool ScriptLifeV2::VAR_GAME() {
	byte varID = _switchParam;
	if (!_isInSwitch) {
		varID = getParamByte();
	}
	byte oper = getParamByte();
	uint16 value = getParamUint16();
	uint16 varVal = getGameVar(varID);
	return testCond(value, varVal, oper);
}
bool ScriptLifeV2::LIFE_POINT() {
	byte oper = getParamByte();
	uint16 value = getParamUint16();
	return false;
}
bool ScriptLifeV2::LIFE_POINT_OBJ() {
	byte oper = getParamByte();
	byte actor = getParamByte();
	uint16 value = getParamUint16();
	return false;
}
bool ScriptLifeV2::NUM_LITTLE_KEYS() {
	byte oper = getParamByte();
	byte numKeys = getParamByte();
	return false;
}
bool ScriptLifeV2::NUM_GOLD_PIECES() {
	byte oper = getParamByte();
	uint16 numGold = getParamUint16();
	return false;
}
bool ScriptLifeV2::BEHAVIOUR() {
	byte oper = getParamByte();
	byte numKeys = getParamByte();
	return false;
}
bool ScriptLifeV2::CHAPTER() {
	byte oper = getParamByte();
	byte value = getParamByte();
	return value == 0;
}

bool ScriptLifeV2::ANGLE() {
	byte oper = getParamByte();
	byte param1 = getParamByte();
	int16 param2 = getParamInt16();
	return false;
}

bool ScriptLifeV2::DISTANCE_MESSAGE() {
	byte oper = getParamByte();
	byte param1 = getParamByte();
	uint16 param2 = getParamUint16();
	return false;
}

#define STUB_SCRIPT(func) void ScriptLifeV2::func() { }

void ScriptLifeV2::END() {
	stop();
}

void ScriptLifeV2::IF() {
	bool cond = checkCondition();

	uint16 address = getParamUint16();
	if (!cond) {
		jumpAddress(address);
	}
}
void ScriptLifeV2::SWIF() {
	bool cond = checkCondition();
	uint16 address = getParamUint16();
}

void ScriptLifeV2::ONEIF() {
	bool cond = checkCondition();
	uint16 address = getParamUint16();
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

void ScriptLifeV2::CAM_FOLLOW() {
	byte actor = getParamByte();
}

void ScriptLifeV2::SET_BEHAVIOUR() {
	byte param = getParamByte();
}

void ScriptLifeV2::SET_VAR_CUBE() {
	byte id = getParamByte();
	byte value = getParamByte();
}

void ScriptLifeV2::SET_DIRMODE() {
	byte dirmode = getParamByte();
	if (dirmode == 2 || dirmode == 4) {
		byte actor = getParamByte();
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

void ScriptLifeV2::GIVE_BONUS() {
	byte unknown = getParamByte();
}

void ScriptLifeV2::OR_IF() {
	bool cond = checkCondition();
	uint16 address = getParamUint16();
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
}

void ScriptLifeV2::SAVE_HERO() {
}

void ScriptLifeV2::RESTORE_HERO() {
}

void ScriptLifeV2::CINEMA_MODE() {
	byte val = getParamByte();
}

void ScriptLifeV2::AND_IF() {
	bool cond = checkCondition();
	uint16 address = getParamUint16();
}

void ScriptLifeV2::SWITCH() {
	//bool cond = checkCondition();
	//uint16 address = getParamUint16();
	_switchCond = getParamByte();
	_switchParam = getParamByte();
	_isInSwitch = true;
}
void ScriptLifeV2::OR_CASE() {
	error("not done");
}
void ScriptLifeV2::CASE() {
	uint16 address = getParamUint16();
	if (!checkCondition(_switchCond)) {
		jumpAddress(address);
	}
}
void ScriptLifeV2::DEFAULT() {
	// do nothing
}
void ScriptLifeV2::BREAK() {
	uint16 address = getParamUint16();
	jumpAddress(address);
}
void ScriptLifeV2::END_SWITCH() {
	_isInSwitch = false;
}

void ScriptLifeV2::SET_HIT_ZONE() {
	byte param1 = getParamByte();
}

void ScriptLifeV2::SAMPLE() {
	uint16 id = getParamUint16();
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

void ScriptLifeV2::SET_ANIM_DIAL() {
	uint16 unknown = getParamUint16();
}

} // end of namespace Twin
