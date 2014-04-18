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

#include "engines/twin/script_track.h"
#include "engines/twin/actor.h"
#include "engines/twin/player.h"
#include "engines/twin/twin.h"
#include "engines/twin/scene.h"
#include "engines/twin/resource.h"
#include "engines/twin/animation.h"

namespace Twin {


ScriptTrack::ScriptTrack(Common::SeekableReadStream *stream) : Script(stream), _label(0) {
	stop();
}

void ScriptTrack::END() {
	stop();
}

void ScriptTrack::NOP() {
}

void ScriptTrack::BODY() {
	byte id = getParamByte();
}

void ScriptTrack::ANIM() {
	uint16 id;
	if (g_twin->getGameType() == GType_LBA2) {
		id = getParamUint16();
	} else {
		id = getParamByte();
	}
	if (_actor) {
		_actor->setAnimation(id);
	}
}

void ScriptTrack::GOTO_POINT() {
	byte id = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	_isWaitingForAction = true;
	_actor->gotoPoint(s->getPoint(id), &_isWaitingForAction);
}

void ScriptTrack::WAIT_ANIM() {
	if (!_actor->_entity) {
		warning("WAIT_ANIM no entity");
		return;
	}
	_isWaitingForAction = true;
	_actor->_entity->_anim->_isWaiting = &_isWaitingForAction;
	_actor->_entity->_anim->_waitLoops = 1;
}

void ScriptTrack::ANGLE() {
	int16 angle = getParamInt16();
	_actor->faceActor(nullptr);
	_actor->turnToAngle(((angle * 360) / 4096.0f) + 90);
}

void ScriptTrack::POS_POINT() {
	byte id = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Point *p = s->getPoint(id);
	_actor->setPos(p->_x, p->_y, p->_z);
}

void ScriptTrack::LABEL() {
	byte id = getParamByte();
	_label = id;
	_labelAddress = getAddress() - 2;
}

void ScriptTrack::GOTO() {
	uint16 id = getParamUint16();
	jumpAddress(id);
}

void ScriptTrack::STOP() {
	stop();
}

void ScriptTrack::GOTO_SYM_POINT() {
	byte id = getParamByte();
}

void ScriptTrack::WAIT_NUM_ANIM() {
	byte numLoops = getParamByte();

	//skip a 0
	jump(1);

	if (!_actor->_entity) {
		warning("WAIT_NUM_ANIM no entity");
		return;
	}

	_isWaitingForAction = true;
	_actor->_entity->_anim->_isWaiting = &_isWaitingForAction;
	_actor->_entity->_anim->_waitLoops = numLoops;
}

void ScriptTrack::SAMPLE() {
	uint16 id = getParamUint16();
}

void ScriptTrack::GOTO_POINT_3D() {
	assert(_actor->_sprite);
	byte id = getParamByte();
	Scene *s = g_twin->getCurrentScene();
	Point *p = s->getPoint(id);
	_isWaitingForAction = true;
	_actor->gotoPoint3D(s->getPoint(id), &_isWaitingForAction);
}

void ScriptTrack::SPEED() {
	int16 speed = getParamInt16();
	_actor->_speed = speed;
}

void ScriptTrack::BACKGROUND() {
	byte id = getParamByte();
}

void ScriptTrack::WAIT_NUM_SECOND() {
	byte numSeconds = getParamByte();
	jump(4);
	_waitTime = numSeconds * 1000;
	_waitedTime = 0;
	_isWaiting = true;
}

void ScriptTrack::NO_BODY() {
	_actor->setInvisible(true);
}

void ScriptTrack::BETA() {
	uint16 angle = getParamUint16();
	_actor->setAngle((360 + 90)-((angle * 360) / 4096.f));
}

void ScriptTrack::OPEN_LEFT() {
	int16 distance = getParamInt16();
	_actor->_speed = 1000;
	_actor->_angle = 180;
	_actor->_speedStart = _actor->_pos;
	_actor->_speedDistance = distance;
	_actor->_doorMode = true;
}

void ScriptTrack::OPEN_RIGHT() {
	int16 distance = getParamInt16();
	_actor->_speed = 1000;
	_actor->_angle = 0;
	_actor->_speedStart = _actor->_pos;
	_actor->_speedDistance = distance;
	_actor->_doorMode = true;
}

void ScriptTrack::OPEN_UP() {
	int16 distance = getParamInt16();
	_actor->_speed = 1000;
	_actor->_angle = 270;
	_actor->_speedStart = _actor->_pos;
	_actor->_speedDistance = distance;
	_actor->_doorMode = true;
}

void ScriptTrack::OPEN_DOWN() {
	int16 distance = getParamInt16();
	_actor->_speed = 1000;
	_actor->_angle = 90;
	_actor->_speedStart = _actor->_pos;
	_actor->_speedDistance = distance;
	_actor->_doorMode = true;
}

void ScriptTrack::CLOSE() {
	_actor->_speed = -1000;
	_actor->_speedStart = _actor->_pos;
	_actor->_doorMode = true;
}

void ScriptTrack::WAIT_DOOR() {
	if (_actor->_doorMode) {
		jump(-1);
		yield();
	}
}

void ScriptTrack::SAMPLE_RND() {
	int16 param = getParamInt16();
}

void ScriptTrack::SAMPLE_ALWAYS() {
	int16 param = getParamInt16();
}

void ScriptTrack::SAMPLE_STOP() {
	int16 param = getParamInt16();
}

void ScriptTrack::PLAY_ACF() {
	error("Read a null terminated string here");
}

void ScriptTrack::REPEAT_SAMPLE() {
	int16 param = getParamInt16();
}

void ScriptTrack::SIMPLE_SAMPLE() {
	int16 param = getParamInt16();
}

void ScriptTrack::FACE_HERO() {
	uint16 param = getParamUint16();
	Player *p = g_twin->getPlayer();
	Math::Angle angle = _actor->getAngleTo(p);
	_actor->turnToAngle(angle);
}

void ScriptTrack::ANGLE_RND() {
	int16 param1 = getParamInt16();
	uint16 param2 = getParamUint16();
}

} // end of namespace Twin
