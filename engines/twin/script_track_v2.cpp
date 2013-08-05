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

#include "engines/twin/script_track_v2.h"
#include "engines/twin/actor.h"
#include "engines/twin/twin.h"

namespace Twin {


#define OPCODE(op, func) case op: func(); break;

ScriptTrackV2::ScriptTrackV2(Common::SeekableReadStream *stream) : Script(stream) {

}

void ScriptTrackV2::execute(byte opcode) {
	switch (opcode) {
		OPCODE(0x00, END);
		OPCODE(0x01, NOP);
		OPCODE(0x02, BODY);
		OPCODE(0x03, ANIM);
		OPCODE(0x04, GOTO_POINT);
		OPCODE(0x05, WAIT_ANIM);

		OPCODE(0x09, LABEL);
		OPCODE(0x0A, GOTO);
		OPCODE(0x0B, STOP);

		OPCODE(0x0E, SAMPLE);

		OPCODE(0x11, BACKGROUND);
		OPCODE(0x12, WAIT_NUM_SECOND);

		OPCODE(0x15, OPEN_LEFT);
		OPCODE(0x16, OPEN_RIGHT);
		OPCODE(0x17, OPEN_UP);
		OPCODE(0x18, OPEN_DOWN);

		OPCODE(0x1A, WAIT_DOOR);
		 
		OPCODE(0x23, REPLACE);
		OPCODE(0x24, WAIT_NUM_DECIMAL);

		OPCODE(0x26, SPRITE);
		 
	default:
		warning("asdf");
	};
}

#define STUB_SCRIPT(func) void ScriptTrackV2::func() { }

void ScriptTrackV2::END() {
	stop();
}
STUB_SCRIPT(NOP);
void ScriptTrackV2::BODY() {
	byte id = getParamByte();
}
void ScriptTrackV2::ANIM() {
	uint16 id = getParamUint16();
	if (_actor) {
		_actor->setAnimation(id);
	}
}
void ScriptTrackV2::GOTO_POINT() {
	uint16 id = getParamByte();
}
STUB_SCRIPT(WAIT_ANIM);

void ScriptTrackV2::LABEL() {
	byte id = getParamByte();
}

void ScriptTrackV2::GOTO() {
	uint16 id = getParamUint16();
	jump(id);
}

void ScriptTrackV2::STOP() {
	stop();
}

void ScriptTrackV2::SAMPLE() {
	uint16 id = getParamUint16();
}

void ScriptTrackV2::BACKGROUND() {
	byte id = getParamByte();
}

void ScriptTrackV2::WAIT_NUM_SECOND() {
	uint16 numSeconds = getParamUint16();
}

void ScriptTrackV2::OPEN_LEFT() {
	int16 distance = getParamInt16();
}
void ScriptTrackV2::OPEN_RIGHT() {
	int16 distance = getParamInt16();
}
void ScriptTrackV2::OPEN_UP() {
	int16 distance = getParamInt16();
}
void ScriptTrackV2::OPEN_DOWN() {
	int16 distance = getParamInt16();
}
STUB_SCRIPT(WAIT_DOOR);

STUB_SCRIPT(REPLACE);

void ScriptTrackV2::WAIT_NUM_DECIMAL() {
	byte id = getParamByte();
}

void ScriptTrackV2::SPRITE() {
	uint16 id = getParamUint16();
}
} // end of namespace Twin
