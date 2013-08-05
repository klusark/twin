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

#include "engines/twin/script.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class ScriptLifeV2 : public Script {
public:
	ScriptLifeV2(Common::SeekableReadStream *stream);
	
private:
	bool ScriptLifeV2::testCond(uint16 a, uint16 b, byte oper);

	void execute(byte opcode) override;

	bool checkCondition();

	//Condition
	bool COL();
	bool COL_OBJ();
	bool DISTANCE();
	bool ZONE();
	bool ZONE_OBJ();

	bool CURRENT_TRACK();

	bool HIT_BY();

	bool VAR_GAME();

	bool NUM_LITTLE_KEYS();

	bool CHAPTER();

	//Opcodes
	void END();

	void IF();
	void SWIF();

	void ELSE();

	void SET_TRACK();

	void MESSAGE();

	void SET_VAR_CUBE();

	void SET_COMPORTEMENT();
	void SET_COMPORTEMENT_OBJ();
	void END_COMPORTEMENT();

	void SUICIDE();

	void END_LIFE();

	void GIVE_BONUS();

	void OR_IF();

	void POS_POINT();

	void SET_HOLO_POS();

	void BETA();

	void AND_IF();

	void SAMPLE();

	void ADD_VAR_GAME();
	void SUB_VAR_GAME();
};

} // end of namespace Twin

#endif
