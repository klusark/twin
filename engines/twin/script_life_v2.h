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

#ifndef TWIN_SCRIPT_LIFE_V2_H
#define TWIN_SCRIPT_LIFE_V2_H

#include "common/stack.h"

#include "engines/twin/script.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class ScriptTrackV2;

class State {
public:
	State() : _isInSwitch(false) {}
	bool _isInSwitch;
	byte _switchCond;
	byte _switchParam;
};

class ScriptLifeV2 : public Script {
public:
	ScriptLifeV2(Common::SeekableReadStream *stream, ScriptTrackV2 *track);
	
private:
	bool ScriptLifeV2::testCond(uint16 a, uint16 b, byte oper);

	ScriptTrackV2 *_track;

	uint16 _comportementAddress;

	void execute(byte opcode) override;

	bool checkCondition();
	bool checkCondition(byte cond);

	Common::Stack<State> _states;
	State _currentState;

	//Condition
	bool COL();
	bool COL_OBJ();
	bool DISTANCE();
	bool ZONE();
	bool ZONE_OBJ();

	bool CURRENT_TRACK();
	bool CURRENT_TRACK_OBJ();
	bool VAR_CUBE();
	bool CONE_VIEW();
	bool HIT_BY();
	bool ACTION();
	bool VAR_GAME();
	bool LIFE_POINT();
	bool LIFE_POINT_OBJ();
	bool NUM_LITTLE_KEYS();
	bool NUM_GOLD_PIECES();
	bool BEHAVIOUR();
	bool CHAPTER();

	bool USE_INVERNTORY();

	bool RND();

	bool BETA_COND();

	bool CARRIED_OBJ_BY();
	bool ANGLE();
	bool DISTANCE_MESSAGE();

	bool PROCESSOR();

	//Opcodes
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
	void SET_CAMERA();
	void CAMERA_CENTER();
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

	void SET_DOOR_LEFT();
	void SET_DOOR_RIGHT();
	void SET_DOOR_UP();
	void SET_DOOR_DOWN();
	void GIVE_BONUS();

	void OR_IF();
	void INVISIBLE();

	void POS_POINT();

	void SET_HOLO_POS();

	void BETA();

	void SET_FRAME();

	void NO_SHOCK();

	void CINEMA_MODE();
	void SAVE_HERO();
	void RESTORE_HERO();
	void ANIM_SET();

	void AND_IF();
	void SWITCH();
	void OR_CASE();
	void CASE();
	void DEFAULT();
	void BREAK();
	void END_SWITCH();
	void SET_HIT_ZONE();
	void SAVE_COMPORTEMENT();
	void RESTORE_COMPORTEMENT();
	void SAMPLE();

	void ADD_VAR_GAME();
	void SUB_VAR_GAME();

	void NO_BODY();

	void SET_ANIM_DIAL();
};

} // end of namespace Twin

#endif
