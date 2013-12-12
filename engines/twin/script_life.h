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

#ifndef TWIN_SCRIPT_LIFE_H
#define TWIN_SCRIPT_LIFE_H

#include "common/stack.h"

#include "engines/twin/script.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {


class ScriptTrack;

class State {
public:
	State() : _isInSwitch(false), _orCase(false) {}
	bool _isInSwitch;
	byte _switchCond;
	byte _param;
	bool _orCase;
};

class ScriptLife : public Script {
public:
	ScriptLife(Common::SeekableReadStream *stream, ScriptTrack *track);

protected:
	bool testCond(uint16 a, uint16 b, byte oper);

	ScriptTrack *_track;

	uint16 _comportementAddress;

	bool checkCondition();
	void loadConditionParam();
	virtual bool checkFuncCondition(byte cond) = 0;
	virtual void loadFuncConditionParam(byte cond) = 0;

	int16 _savedTrack;

	Common::Stack<State> _states;
	State _currentState;

	bool COL(byte);
	bool COL_OBJ(byte);
	bool DISTANCE(byte);
	bool ZONE(byte);
	bool ZONE_OBJ(byte);
	bool BODY_COND(byte);
	bool BODY_OBJ_COND(byte);
	bool ANIM_COND(byte);
	bool ANIM_OBJ_COND(byte);
	bool CURRENT_TRACK(byte);
	bool CURRENT_TRACK_OBJ(byte);
	bool VAR_CUBE(byte);
	bool CONE_VIEW(byte);
	bool HIT_BY(byte);
	bool ACTION(byte);
	bool VAR_GAME(byte);
	bool LIFE_POINT(byte);
	bool LIFE_POINT_OBJ(byte);
	bool NUM_LITTLE_KEYS(byte);
	bool NUM_GOLD_PIECES(byte);
	bool BEHAVIOUR(byte);
	bool CHAPTER(byte);
	bool DISTANCE_3D(byte);

	bool USE_INVERNTORY(byte);

	bool CARRIED_BY(byte);

	// Shared OPCODES
	void END();

	void SNIF();

	void NEVERIF();

	void RETURN();
	void IF();
	void SWIF();
	void ONEIF();
	void ELSE();

	void BODY();
	void BODY_OBJ();
	void ANIM();
	void ANIM_OBJ();

	void SET_TRACK();
	void SET_TRACK_OBJ();
	void MESSAGE();
	void CAN_FALL();
	void SET_DIRMODE();
	void SET_DIRMODE_OBJ();
	void CAM_FOLLOW();
	void SET_BEHAVIOUR();
	void SET_VAR_CUBE();

	void SET_COMPORTEMENT();
	void SET_COMPORTEMENT_OBJ();
	void END_COMPORTEMENT();
	void SET_VAR_GAME();
	void KILL_OBJ();
	void SUICIDE();
	void USE_ONE_LITTLE_KEY();
	void GIVE_GOLD_PIECES();
	void END_LIFE();
	void STOP_CURRENT_TRACK ();
	void RESTORE_LAST_TRACK ();
	void MESSAGE_OBJ();
	void INC_CHAPTER();
	void FOUND_OBJECT();
	void SET_DOOR_LEFT();
	void SET_DOOR_RIGHT();
	void SET_DOOR_UP();
	void SET_DOOR_DOWN();
	void GIVE_BONUS();

};

} // end of namespace Twin

#endif
