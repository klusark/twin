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

#include "engines/twin/script_life_v1.h"
#include "engines/twin/script_track_v1.h"
#include "engines/twin/actor.h"
#include "engines/twin/player.h"
#include "engines/twin/scene.h"
#include "engines/twin/twin.h"
#include "engines/twin/resource.h"
#include "engines/twin/animation.h"

namespace Twin {



ScriptLifeV1::ScriptLifeV1(Common::SeekableReadStream *stream, ScriptTrackV1 *track) : 
	Script(stream), _track(track), _comportementAddress(0), _savedTrack(0) {

}

bool ScriptLifeV1::testCond(uint16 b, uint16 a, byte oper) {
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

void ScriptLifeV1::execute(byte opcode) {
	switch (opcode) {
		#define OPCODE(op, func) case op: func(); break
			LIFE_OPCODES_V1
		#undef OPCODE
	default:
		warning("asdf");
	};
}
bool ScriptLifeV1::checkCondition() {
	byte cond = getParamByte();
	loadConditionParam(cond);
	return checkCondition(cond);
}

void ScriptLifeV1::loadConditionParam() {
	byte param = getParamByte();
	//_currentState._param = param;
}

void ScriptLifeV1::loadConditionParam(byte cond) {
	switch (cond) {
		#define COND_OPCODE(op, func, param) case op: if (param != 0) { loadConditionParam(); } break
			LIFE_COND_OPCODES_V1
		#undef COND_OPCODE

	default:
		warning("asdf");
	}
}

bool ScriptLifeV1::checkCondition(byte cond) {
	byte oper = getParamByte();
	switch (cond) {
		#define COND_OPCODE(op, func, params) case op: return func(oper)
			LIFE_COND_OPCODES_V1
		#undef COND_OPCODE

	default:
		warning("asdf");
	}
	return false;
}

} // end of namespace Twin
