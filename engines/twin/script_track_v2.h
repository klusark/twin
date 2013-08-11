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

#ifndef TWIN_SCRIPT_TRACK_V2_H
#define TWIN_SCRIPT_TRACK_V2_H

#include "engines/twin/script.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {


#define TRACK_OPCODES					\
	OPCODE(0x00, END);					\
	OPCODE(0x01, NOP);					\
	OPCODE(0x02, BODY);					\
	OPCODE(0x03, ANIM);					\
	OPCODE(0x04, GOTO_POINT);			\
	OPCODE(0x05, WAIT_ANIM);			\
										\
	OPCODE(0x07, ANGLE);				\
	OPCODE(0x08, POS_POINT);			\
	OPCODE(0x09, LABEL);				\
	OPCODE(0x0A, GOTO);					\
	OPCODE(0x0B, STOP);					\
										\
	OPCODE(0x0D, WAIT_NUM_ANIM);		\
	OPCODE(0x0E, SAMPLE);				\
										\
	OPCODE(0x10, SPEED);				\
	OPCODE(0x11, BACKGROUND);			\
	OPCODE(0x12, WAIT_NUM_SECOND);		\
										\
	OPCODE(0x15, OPEN_LEFT);			\
	OPCODE(0x16, OPEN_RIGHT);			\
	OPCODE(0x17, OPEN_UP);				\
	OPCODE(0x18, OPEN_DOWN);			\
										\
	OPCODE(0x1A, WAIT_DOOR);			\
	OPCODE(0x1B, SAMPLE_RND);			\
	OPCODE(0x1C, SAMPLE_ALWAYS);		\
	OPCODE(0x1D, SAMPLE_STOP);			\
	OPCODE(0x1E, PLAY_ACF);				\
	OPCODE(0x1F, REPEAT_SAMPLE);		\
	OPCODE(0x20, SIMPLE_SAMPLE);		\
	OPCODE(0x21, FACE_HERO);			\
	OPCODE(0x22, ANGLE_RND);			\
	OPCODE(0x23, REPLACE);				\
	OPCODE(0x24, WAIT_NUM_DECIMAL);		\
										\
	OPCODE(0x26, SPRITE);				\
	OPCODE(0x27, WAIT_NUM_SECOND_RND);	\
										\
	OPCODE(0x2E, STOP_ANIM_3DS);		\
										\
	OPCODE(0x31, WAIT_NUM_DECIMAL_RND);	\
										\
	OPCODE(0x33, FREQUENCY);			\
	OPCODE(0x34, VOLUME);				\


class ScriptTrackV2 : public Script {
public:
	ScriptTrackV2(Common::SeekableReadStream *stream);
	byte getLabel() { return _label; }
	uint16 getLabelAddress() { return _labelAddress; }
private:
	void execute(byte opcode) override;

	byte _label;
	uint16 _labelAddress;

	//Opcodes
	#define OPCODE(op, func) void func()
		TRACK_OPCODES
	#undef OPCODE
};

} // end of namespace Twin

#endif
