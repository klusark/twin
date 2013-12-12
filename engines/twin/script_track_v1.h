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

#ifndef TWIN_SCRIPT_TRACK_V1_H
#define TWIN_SCRIPT_TRACK_V1_H

#include "engines/twin/script.h"
#include "engines/twin/script_track.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {


#define TRACK_OPCODES_V1				\
	OPCODEI(0x00, END);					\
	OPCODEI(0x01, NOP);					\
	OPCODEI(0x02, BODY);				\
	OPCODEI(0x03, ANIM);				\
	OPCODEI(0x04, GOTO_POINT);			\
	OPCODEI(0x05, WAIT_ANIM);			\
										\
	OPCODEI(0x07, ANGLE);				\
	OPCODEI(0x08, POS_POINT);			\
	OPCODEI(0x09, LABEL);				\
	OPCODEI(0x0A, GOTO);				\
	OPCODEI(0x0B, STOP);				\
	OPCODEI(0x0C, GOTO_SYM_POINT);		\
	OPCODEI(0x0D, WAIT_NUM_ANIM);		\
	OPCODEI(0x0E, SAMPLE);				\
	OPCODEI(0x0F, GOTO_POINT_3D);		\
	OPCODEI(0x10, SPEED);				\
	OPCODEI(0x11, BACKGROUND);			\
	OPCODEI(0x12, WAIT_NUM_SECOND);		\



class ScriptTrackV1 : public ScriptTrack {
public:
	ScriptTrackV1(Common::SeekableReadStream *stream);
private:
	void execute(byte opcode) override;

	//Opcodes
	#define OPCODE(op, func) void func()
	#define OPCODEI(op, func)
		TRACK_OPCODES_V1
	#undef OPCODE
	#undef OPCODEI
};

} // end of namespace Twin

#endif
