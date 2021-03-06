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
#include "engines/twin/player.h"
#include "engines/twin/twin.h"
#include "engines/twin/scene.h"
#include "engines/twin/resource.h"
#include "engines/twin/animation.h"

namespace Twin {


ScriptTrackV2::ScriptTrackV2(Common::SeekableReadStream *stream) : ScriptTrack(stream) {
}

void ScriptTrackV2::execute(byte opcode) {
	switch (opcode) {
		#define OPCODE(op, func) case op: func(); break
		#define OPCODEI(op, func) OPCODE(op, func)
			TRACK_OPCODES_V2
		#undef OPCODE
		#undef OPCODEI
	default:
		warning("asdf");
	};
}

#define STUB_SCRIPT(func) void ScriptTrackV2::func() { }

STUB_SCRIPT(REPLACE);

void ScriptTrackV2::WAIT_NUM_DECIMAL() {
	byte numSeconds = getParamByte();
	jump(4);
	_waitTime = numSeconds * 100;
	_waitedTime = 0;
	_isWaiting = true;
}

void ScriptTrackV2::SPRITE() {
	uint16 id = getParamUint16();
	_actor->_spriteID = id;
	_actor->_sprite = g_resource->getSprite(id);
}

void ScriptTrackV2::WAIT_NUM_SECOND_RND() {
	//TODO: Add the randomness
	byte numSeconds = getParamByte();
	jump(4);
	_waitTime = numSeconds * 1000;
	_waitedTime = 0;
	_isWaiting = true;
}

void ScriptTrackV2::SET_FRAME() {
	//unknown
	byte param = getParamByte();
}

void ScriptTrackV2::SET_FRAME_3DS() {
	//unknown
	byte param = getParamByte();
}

void ScriptTrackV2::SET_START_3DS() {
	//unknown
	byte param = getParamByte();
}

void ScriptTrackV2::SET_END_3DS() {
	//unknown
	byte param = getParamByte();
}

void ScriptTrackV2::START_ANIM_3DS() {
	//unknown
	byte param = getParamByte();
}

void ScriptTrackV2::STOP_ANIM_3DS() {
	//unknown
}

void ScriptTrackV2::WAIT_ANIM_3DS() {
	//unknown
}

void ScriptTrackV2::WAIT_FRAME_3DS() {
	//unknown
}

void ScriptTrackV2::WAIT_NUM_DECIMAL_RND() {
	//TODO: Add the randomness
	byte numSeconds = getParamByte();
	jump(4);
	_waitTime = numSeconds * 100;
	_waitedTime = 0;
	_isWaiting = true;
}

void ScriptTrackV2::INTERVAL() {
	uint16 param = getParamUint16();
}

void ScriptTrackV2::FREQUENCY() {
	uint16 param = getParamUint16();
}

void ScriptTrackV2::VOLUME() {
	byte param = getParamByte();
}

} // end of namespace Twin
