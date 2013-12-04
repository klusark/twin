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

#ifndef TWIN_SCRIPT_TRACK_V1_H
#define TWIN_SCRIPT_TRACK_V1_H

#include "engines/twin/script.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {


#define TRACK_OPCODES					\



class ScriptTrackV1 : public Script {
public:
	ScriptTrackV1(Common::SeekableReadStream *stream);
	byte getLabel() { return _label; }
	uint16 getLabelAddress() { return _labelAddress; }
private:
	void execute(byte opcode) override;

	byte _label;
	uint16 _labelAddress;

	//Opcodes
	#define OPCODE(op, func) void func()
		TRACK_OPCODES
	#undef OPCODE
};

} // end of namespace Twin

#endif
