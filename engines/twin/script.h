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

#ifndef TWIN_SCRIPT_H
#define TWIN_SCRIPT_H


namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Actor;

class Script {
public:
	Script(Common::SeekableReadStream *stream);
	virtual ~Script();
	void run();
	void setActor(Actor *a) { _actor = a; };
//protected:
	byte getParamByte();
	uint16 getParamUint16();
	int16 getParamInt16();
	void stop();
	void start();
	void yield();
	void jump(uint16 size);
	void jumpAddress(uint16 address);
	uint16 getGameVar(byte id);
	void setGameVar(byte id, uint16 val);

	Actor *_actor;

private:
	void loadLBA2(Common::SeekableReadStream *stream);

	bool _isExecuting;
	bool _isYielded;
	uint16 _length;
	byte *_data;
	
	byte *_ptr;

	static uint16 _gameVars[256];

	virtual void execute(byte opcode) = 0; 
};

} // end of namespace Twin

#endif
