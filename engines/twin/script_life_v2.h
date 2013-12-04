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

#define LIFE_OPCODES_V2					\
	OPCODE(0x00, END);					\
										\
	OPCODE(0x02, SNIF);					\
										\
	OPCODE(0x04, NEVERIF);				\
										\
	OPCODE(0x0B, RETURN);				\
	OPCODE(0x0C, IF);					\
	OPCODE(0x0D, SWIF);					\
	OPCODE(0x0E, ONEIF);				\
	OPCODE(0x0F, ELSE);					\
										\
	OPCODE(0x11, BODY);					\
	OPCODE(0x12, BODY_OBJ);				\
	OPCODE(0x13, ANIM);					\
	OPCODE(0x14, ANIM_OBJ);				\
	OPCODE(0x15, SET_CAMERA);			\
	OPCODE(0x16, CAMERA_CENTER);		\
	OPCODE(0x17, SET_TRACK);			\
	OPCODE(0x18, SET_TRACK_OBJ);		\
	OPCODE(0x19, MESSAGE);				\
	OPCODE(0x1A, CAN_FALL);				\
	OPCODE(0x1B, SET_DIRMODE);			\
	OPCODE(0x1C, SET_DIRMODE_OBJ);		\
	OPCODE(0x1D, CAM_FOLLOW);			\
	OPCODE(0x1E, SET_BEHAVIOUR);		\
	OPCODE(0x1F, SET_VAR_CUBE);			\
										\
	OPCODE(0x21, SET_COMPORTEMENT);		\
	OPCODE(0x22, SET_COMPORTEMENT_OBJ);	\
	OPCODE(0x23, END_COMPORTEMENT);		\
	OPCODE(0x24, SET_VAR_GAME);			\
	OPCODE(0x25, KILL_OBJ);				\
	OPCODE(0x26, SUICIDE);				\
	OPCODE(0x27, USE_ONE_LITTLE_KEY);	\
	OPCODE(0x28, GIVE_GOLD_PIECES);		\
	OPCODE(0x29, END_LIFE);				\
	OPCODE(0x2A, STOP_CURRENT_TRACK );	\
	OPCODE(0x2B, RESTORE_LAST_TRACK );	\
	OPCODE(0x2C, MESSAGE_OBJ);			\
	OPCODE(0x2D, INC_CHAPTER);			\
	OPCODE(0x2E, FOUND_OBJECT);			\
	OPCODE(0x2F, SET_DOOR_LEFT);		\
	OPCODE(0x30, SET_DOOR_RIGHT);		\
	OPCODE(0x31, SET_DOOR_UP);			\
	OPCODE(0x32, SET_DOOR_DOWN);		\
	OPCODE(0x33, GIVE_BONUS);			\
										\
	OPCODE(0x35, OBJ_COL);				\
	OPCODE(0x36, BRICK_COL);			\
	OPCODE(0x37, OR_IF);				\
	OPCODE(0x38, INVISIBLE);			\
	OPCODE(0x39, SHADOW_OBJ);			\
	OPCODE(0x3A, POS_POINT);			\
										\
	OPCODE(0x3F, HIT_OBJ);				\
	OPCODE(0x40, PLAY_ACF);				\
										\
	OPCODE(0x48, SET_HOLO_POS);			\
										\
	OPCODE(0x4C, SET_GRM );				\
	OPCODE(0x4D, SET_CHANGE_CUBE);		\
										\
	OPCODE(0x50, BETA);					\
										\
	OPCODE(0x53, SET_FRAME);			\
	OPCODE(0x54, SET_SPRITE);			\
										\
	OPCODE(0x5A, NO_SHOCK);				\
										\
	OPCODE(0x5C, CINEMA_MODE);			\
	OPCODE(0x5D, SAVE_HERO);			\
	OPCODE(0x5E, RESTORE_HERO);			\
	OPCODE(0x5F, ANIM_SET);				\
										\
	OPCODE(0x65, TRACK_TO_VAR_GAME);	\
	OPCODE(0x66, VAR_GAME_TO_TRACK);	\
	OPCODE(0x67, ANIM_TEXTURE);			\
										\
	OPCODE(0x70, AND_IF);				\
	OPCODE(0x71, SWITCH);				\
	OPCODE(0x72, OR_CASE);				\
	OPCODE(0x73, CASE);					\
	OPCODE(0x74, DEFAULT);				\
	OPCODE(0x75, BREAK);				\
	OPCODE(0x76, END_SWITCH);			\
	OPCODE(0x77, SET_HIT_ZONE);			\
	OPCODE(0x78, SAVE_COMPORTEMENT);	\
	OPCODE(0x79, RESTORE_COMPORTEMENT);	\
	OPCODE(0x7A, SAMPLE);				\
	OPCODE(0x7B, SAMPLE_RND);			\
	OPCODE(0x7C, SAMPLE_ALWAYS);		\
	OPCODE(0x7D, SAMPLE_STOP);			\
	OPCODE(0x7E, REPEAT_SAMPLE);		\
	OPCODE(0x7F, BACKGROUND);			\
	OPCODE(0x80, ADD_VAR_GAME);			\
	OPCODE(0x81, SUB_VAR_GAME);			\
										\
	OPCODE(0x85, SET_RAIL);				\
										\
	OPCODE(0x87, NO_BODY);				\
										\
	OPCODE(0x91, FLOW_POINT);			\
	OPCODE(0x92, FLOW_OBJ);				\
	OPCODE(0x93, SET_ANIM_DIAL);		\
	OPCODE(0x98, NEW_SAMPLE);			\


#define LIFE_COND_OPCODES_V2				\
	COND_OPCODE(0x00, COL, 0);				\
	COND_OPCODE(0x01, COL_OBJ, 1);			\
	COND_OPCODE(0x02, DISTANCE, 1);			\
	COND_OPCODE(0x03, ZONE, 0);				\
	COND_OPCODE(0x04, ZONE_OBJ, 1);			\
	COND_OPCODE(0x05, BODY, 0);				\
	COND_OPCODE(0x06, BODY_OBJ, 1);			\
	COND_OPCODE(0x07, ANIM, 0);				\
	COND_OPCODE(0x08, ANIM_OBJ, 1);			\
	COND_OPCODE(0x09, CURRENT_TRACK, 0);	\
	COND_OPCODE(0x0A, CURRENT_TRACK_OBJ, 1);\
	COND_OPCODE(0x0B, VAR_CUBE, 1);			\
	COND_OPCODE(0x0C, CONE_VIEW, 1);		\
	COND_OPCODE(0x0D, HIT_BY, 0);			\
	COND_OPCODE(0x0E, ACTION, 0);			\
	COND_OPCODE(0x0F, VAR_GAME, 1);			\
	COND_OPCODE(0x10, LIFE_POINT, 0);		\
	COND_OPCODE(0x11, LIFE_POINT_OBJ, 1);	\
	COND_OPCODE(0x12, NUM_LITTLE_KEYS, 0);	\
	COND_OPCODE(0x13, NUM_GOLD_PIECES, 0);	\
	COND_OPCODE(0x14, BEHAVIOUR, 0);		\
	COND_OPCODE(0x15, CHAPTER, 0);			\
	COND_OPCODE(0x16, DISTANCE_3D, 1);		\
											\
	COND_OPCODE(0x19, USE_INVERNTORY, 1);	\
											\
	COND_OPCODE(0x1C, CARRIED_BY, 0);		\
											\
	COND_OPCODE(0x1F, RND, 1);				\
	COND_OPCODE(0x20, RAIL, 1);				\
	COND_OPCODE(0x21, BETA_COND, 0);		\
	COND_OPCODE(0x22, BETA_OBJ, 1);			\
	COND_OPCODE(0x23, CARRIED_OBJ_BY, 1);	\
	COND_OPCODE(0x24, ANGLE, 1);			\
	COND_OPCODE(0x25, DISTANCE_MESSAGE, 1);	\
	COND_OPCODE(0x26, HIT_OBJ_BY, 1);		\
											\
	COND_OPCODE(0x29, COL_DECORS, 0);		\
	COND_OPCODE(0x2A, COL_DECORS_OBJ, 1);	\
	COND_OPCODE(0x2B, PROCESSOR, 0);		\
	COND_OPCODE(0x2C, OBJECT_DISPLAYED, 0);	\
	COND_OPCODE(0x2D, ANGLE_OBJ, 1);		\

class ScriptTrackV2;

class State {
public:
	State() : _isInSwitch(false), _orCase(false) {}
	bool _isInSwitch;
	byte _switchCond;
	byte _param;
	bool _orCase;
};

class ScriptLifeV2 : public Script {
public:
	ScriptLifeV2(Common::SeekableReadStream *stream, ScriptTrackV2 *track);

private:
	bool testCond(uint16 a, uint16 b, byte oper);

	ScriptTrackV2 *_track;

	uint16 _comportementAddress;

	void execute(byte opcode) override;

	bool checkCondition();
	bool checkCondition(byte cond);
	void loadConditionParam();
	void loadConditionParam(byte cond);

	Common::Stack<State> _states;
	State _currentState;

	int16 _savedTrack;

	//Condition
	#define COND_OPCODE(op, func, param) bool func(byte oper)
		LIFE_COND_OPCODES_V2
	#undef COND_OPCODE

	//Opcodes
	#define OPCODE(op, func) void func()
		LIFE_OPCODES_V2
	#undef OPCODE
};

} // end of namespace Twin

#endif
