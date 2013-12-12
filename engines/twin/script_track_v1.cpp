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

#include "engines/twin/script_track_v1.h"
#include "engines/twin/actor.h"
#include "engines/twin/player.h"
#include "engines/twin/twin.h"
#include "engines/twin/scene.h"
#include "engines/twin/resource.h"
#include "engines/twin/animation.h"

namespace Twin {


ScriptTrackV1::ScriptTrackV1(Common::SeekableReadStream *stream) : ScriptTrack(stream) {
}

void ScriptTrackV1::execute(byte opcode) {
	warning("%x", opcode);
	switch (opcode) {
		#define OPCODE(op, func) case op: func(); break
		#define OPCODEI(op, func) OPCODE(op, func)
			TRACK_OPCODES_V1
		#undef OPCODE
		#undef OPCODEI
	default:
		warning("asdf");
	};
}

} // end of namespace Twin
