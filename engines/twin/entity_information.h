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

#ifndef TWIN_ENTITY_INFORMATION_H
#define TWIN_ENTITY_INFORMATION_H

#include "common/array.h"

#include "engines/twin/resource.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

class Hqr;

class EntityBody {
public:
	uint16 _bodyIndex;
	byte _index;
	bool _hasBox;

	BoundingBox _box;
};

class EntityAnim {
public:
	byte _index;
	uint16 _animIndex;
};

class EntityEntry {
public:
	Common::Array<EntityBody> _bodies;
	Common::Array<EntityAnim> _anims;
};

class EntityInformation {
public:
	EntityInformation(Common::SeekableReadStream *stream);
	EntityInformation(Hqr *file);
	~EntityInformation();
//private:

	void loadEntity(Common::SeekableReadStream *stream, EntityEntry *e);

	EntityEntry *_entities;
	uint32 _numEntities;
};

} // end of namespace Twin

#endif
