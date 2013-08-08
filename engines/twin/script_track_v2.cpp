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

namespace Twin {


ScriptTrackV2::ScriptTrackV2(Common::SeekableReadStream *stream) : Script(stream), _label(0) {
	stop();
}

void ScriptTrackV2::execute(byte opcode) {
	switch (opcode) {
		#define OPCODE(op, func) case op: warning(#func); func(); break
			TRACK_OPCODES
		#undef OPCODE
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
	byte id = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	_isExecuting = false;
	_actor->gotoPoint(s->getPoint(id), &_isExecuting);
}
STUB_SCRIPT(WAIT_ANIM);

void ScriptTrackV2::ANGLE() {
	int16 angle = getParamInt16();
	_actor->faceActor(nullptr);
	_actor->turnToAngle(((angle * 360) / 4096) + 90);
}

void ScriptTrackV2::POS_POINT() {
	byte id = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Point *p = s->getPoint(id);
	_actor->setPos(p->_x, p->_y, p->_z);
}

void ScriptTrackV2::LABEL() {
	byte id = getParamByte();
	_label = id;
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

void ScriptTrackV2::SPEED() {
	int16 speed = getParamInt16();
}

void ScriptTrackV2::BACKGROUND() {
	byte id = getParamByte();
}

void ScriptTrackV2::WAIT_NUM_SECOND() {
	byte numSeconds = getParamByte();
	jump(4);
	_waitTime = numSeconds * 1000;
	_waitedTime = 0;
	_isWaiting = true;
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

void ScriptTrackV2::SAMPLE_RND() {
	int16 param = getParamInt16();
}

void ScriptTrackV2::SAMPLE_ALWAYS() {
	int16 param = getParamInt16();
}

void ScriptTrackV2::SAMPLE_STOP() {
	int16 param = getParamInt16();
}

void ScriptTrackV2::PLAY_ACF() {
	error("Read a null terminated string here");
}

void ScriptTrackV2::REPEAT_SAMPLE() {
	int16 param = getParamInt16();
}

void ScriptTrackV2::SIMPLE_SAMPLE() {
	int16 param = getParamInt16();
}

void ScriptTrackV2::FACE_HERO() {
	uint16 param = getParamUint16();
	Player *p = g_twin->getPlayer();
	_actor->faceActor(p);

}

void ScriptTrackV2::ANGLE_RND() {
	int16 param1 = getParamInt16();
	uint16 param2 = getParamUint16();
}

STUB_SCRIPT(REPLACE);

void ScriptTrackV2::WAIT_NUM_DECIMAL() {
	byte id = getParamByte();
}

void ScriptTrackV2::SPRITE() {
	uint16 id = getParamUint16();
}

void ScriptTrackV2::WAIT_NUM_SECOND_RND() {
	byte numSeconds = getParamByte();
	jump(4);
}

} // end of namespace Twin
