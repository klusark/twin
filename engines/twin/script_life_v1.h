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
#include "engines/twin/script_life.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

#define LIFE_OPCODES_V1					\
	OPCODEI(0x00, END);					\
										\
	OPCODEI(0x02, SNIF);				\
										\
	OPCODEI(0x04, NEVERIF);				\
										\
	OPCODEI(0x0B, RETURN);				\
	OPCODEI(0x0C, IF);					\
	OPCODEI(0x0D, SWIF);				\
	OPCODEI(0x0E, ONEIF);				\
	OPCODEI(0x0F, ELSE);				\



#define LIFE_COND_OPCODES_V1				\
	COND_OPCODE(0x00, COL, 0);				\
	COND_OPCODE(0x01, COL_OBJ, 1);			\
	COND_OPCODE(0x02, DISTANCE, 1);			\
	COND_OPCODE(0x03, ZONE, 0);				\
	COND_OPCODE(0x04, ZONE_OBJ, 1);			\
	COND_OPCODE(0x05, BODY_COND, 0);		\
	COND_OPCODE(0x06, BODY_OBJ_COND, 1);	\
	COND_OPCODE(0x07, ANIM_COND, 0);		\
	COND_OPCODE(0x08, ANIM_OBJ_COND, 1);	\
	COND_OPCODE(0x09, CURRENT_TRACK, 0);	\
	COND_OPCODE(0x0A, CURRENT_TRACK_OBJ, 1);\
	COND_OPCODE(0x0B, VAR_CUBE, 1);			\
	COND_OPCODE(0x0C, CONE_VIEW, 1);		\
	COND_OPCODE(0x0D, HIT_BY, 0);			\
	COND_OPCODE(0x0E, ACTION, 0);			\
	COND_OPCODE(0x0F, VAR_GAME, 1);			\
	COND_OPCODE(0x10, LIFE_POINT, 0);		\
	COND_OPCODE(0x11, LIFE_POINT_OBJ, 1);	\
	COND_OPCODE(0x12, NUM_LITTLE_KEYS, 0);	\
	COND_OPCODE(0x13, NUM_GOLD_PIECES, 0);	\
	COND_OPCODE(0x14, BEHAVIOUR, 0);		\
	COND_OPCODE(0x15, CHAPTER, 0);			\
	COND_OPCODE(0x16, DISTANCE_3D, 1);		\
											\
	COND_OPCODE(0x19, USE_INVERNTORY, 1);	\
											\
	COND_OPCODE(0x1C, CARRIED_BY, 0);		\


class ScriptLifeV1 : public ScriptLife {
public:
	ScriptLifeV1(Common::SeekableReadStream *stream, ScriptTrack *track);

private:
	bool testCond(uint16 a, uint16 b, byte oper);

	void execute(byte opcode) override;

	bool checkFuncCondition(byte cond) override;
	void loadFuncConditionParam(byte cond) override;


	//Opcodes
	#define OPCODE(op, func) void func()
	#define OPCODEI(op, func)
		LIFE_OPCODES_V1
	#undef OPCODE
	#undef OPCODEI
};

} // end of namespace Twin

#endif
