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

#include "engines/twin/player.h"
#include "engines/twin/resource.h"

namespace Twin {

Player::Player() {
	_entityID = 0;
	_controlMode = kManual;
}

void Player::handleKeyDown(Common::KeyCode key) {
	if (key == Common::KEYCODE_UP) {
		delete _entity;
		_entity = g_resource->getEntity(0, 0, kForward);
	}
}

void Player::handleKeyUp(Common::KeyCode key) {

}

} // end of namespace Twin
