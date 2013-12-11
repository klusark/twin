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


} // end of namespace Twin
