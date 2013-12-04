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

#ifndef TWIN_SCRIPT_LIFE_V1_H
#define TWIN_SCRIPT_LIFE_V1_H

#include "common/stack.h"

#include "engines/twin/script.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

#define LIFE_OPCODES_V1



#define LIFE_COND_OPCODES_V1


class ScriptTrackV1;



class ScriptLifeV1 : public Script {
public:
	ScriptLifeV1(Common::SeekableReadStream *stream, ScriptTrackV1 *track);

private:
	bool testCond(uint16 a, uint16 b, byte oper);

	ScriptTrackV1 *_track;

	uint16 _comportementAddress;

	void execute(byte opcode) override;

	bool checkCondition();
	bool checkCondition(byte cond);
	void loadConditionParam();
	void loadConditionParam(byte cond);


	int16 _savedTrack;

	//Condition
	#define COND_OPCODE(op, func, param) bool func(byte oper)
		LIFE_COND_OPCODES_V1
	#undef COND_OPCODE

	//Opcodes
	#define OPCODE(op, func) void func()
		LIFE_OPCODES_V1
	#undef OPCODE
};

} // end of namespace Twin

#endif
