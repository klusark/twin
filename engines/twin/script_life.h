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

#ifndef TWIN_SCRIPT_LIFE_H
#define TWIN_SCRIPT_LIFE_H

#include "common/stack.h"

#include "engines/twin/script.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {


class ScriptTrack;

class State {
public:
	State() : _isInSwitch(false), _orCase(false) {}
	bool _isInSwitch;
	byte _switchCond;
	byte _param;
	bool _orCase;
};

class ScriptLife : public Script {
public:
	ScriptLife(Common::SeekableReadStream *stream, ScriptTrack *track);

protected:
	bool testCond(uint16 a, uint16 b, byte oper);

	ScriptTrack *_track;

	uint16 _comportementAddress;

	bool checkCondition();
	void loadConditionParam();
	virtual bool checkFuncCondition(byte cond) = 0;
	virtual void loadFuncConditionParam(byte cond) = 0;

	int16 _savedTrack;

	Common::Stack<State> _states;
	State _currentState;

};

} // end of namespace Twin

#endif
