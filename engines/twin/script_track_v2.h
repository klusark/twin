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
#include "engines/twin/script_track.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {


#define TRACK_OPCODES_V2				\
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
	OPCODEI(0x13, NO_BODY);				\
	OPCODEI(0x14, BETA);				\
	OPCODEI(0x15, OPEN_LEFT);			\
	OPCODEI(0x16, OPEN_RIGHT);			\
	OPCODEI(0x17, OPEN_UP);				\
	OPCODEI(0x18, OPEN_DOWN);			\
	OPCODEI(0x19, CLOSE);				\
	OPCODEI(0x1A, WAIT_DOOR);			\
	OPCODEI(0x1B, SAMPLE_RND);			\
	OPCODEI(0x1C, SAMPLE_ALWAYS);		\
	OPCODEI(0x1D, SAMPLE_STOP);			\
	OPCODEI(0x1E, PLAY_ACF);			\
	OPCODEI(0x1F, REPEAT_SAMPLE);		\
	OPCODEI(0x20, SIMPLE_SAMPLE);		\
	OPCODEI(0x21, FACE_HERO);			\
	OPCODEI(0x22, ANGLE_RND);			\
	OPCODE(0x23, REPLACE);				\
	OPCODE(0x24, WAIT_NUM_DECIMAL);		\
										\
	OPCODE(0x26, SPRITE);				\
	OPCODE(0x27, WAIT_NUM_SECOND_RND);	\
										\
	OPCODE(0x29, SET_FRAME);			\
	OPCODE(0x2A, SET_FRAME_3DS);		\
	OPCODE(0x2B, SET_START_3DS);		\
	OPCODE(0x2C, SET_END_3DS);			\
	OPCODE(0x2D, START_ANIM_3DS);		\
	OPCODE(0x2E, STOP_ANIM_3DS);		\
	OPCODE(0x2F, WAIT_ANIM_3DS);		\
	OPCODE(0x30, WAIT_FRAME_3DS);		\
	OPCODE(0x31, WAIT_NUM_DECIMAL_RND);	\
	OPCODE(0x32, INTERVAL);				\
	OPCODE(0x33, FREQUENCY);			\
	OPCODE(0x34, VOLUME);				\


class ScriptTrackV2 : public ScriptTrack {
public:
	ScriptTrackV2(Common::SeekableReadStream *stream);
private:
	void execute(byte opcode) override;

	//Opcodes
	#define OPCODE(op, func) void func()
	#define OPCODEI(op, func)
		TRACK_OPCODES_V2
	#undef OPCODE
	#undef OPCODEI
};

} // end of namespace Twin

#endif
