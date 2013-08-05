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

class ScriptTrackV2 : public Script {
public:
	ScriptTrackV2(Common::SeekableReadStream *stream);
	byte getLabel() { return _label; }
private:
	void execute(byte opcode) override;

	byte _label;

	//Opcodes
	void END();
	void NOP();
	void BODY();
	void ANIM();
	void GOTO_POINT();
	void WAIT_ANIM();

	void ANGLE();
	void POS_POINT();
	void LABEL();
	void GOTO();
	void STOP();

	void SAMPLE();

	void BACKGROUND();
	void WAIT_NUM_SECOND();

	void OPEN_LEFT();
	void OPEN_RIGHT();
	void OPEN_UP();
	void OPEN_DOWN();

	void WAIT_DOOR();
	void SAMPLE_RND();
	void SAMPLE_ALWAYS();
	void SAMPLE_STOP();
	void PLAY_ACF();
	void REPEAT_SAMPLE();
	void SIMPLE_SAMPLE();
	void FACE_HERO();
	void ANGLE_RND();
	void REPLACE();
	void WAIT_NUM_DECIMAL();

	void SPRITE();
	void WAIT_NUM_SECOND_RND();
};

} // end of namespace Twin

#endif
