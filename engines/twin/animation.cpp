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

#include "engines/twin/animation.h"
#include "engines/twin/twin.h"
#include "engines/twin/model.h"

namespace Twin {

Animation::Animation(Common::SeekableReadStream *stream, Model *m, uint16 id) :
		_id(id), _isWaiting(nullptr), _stopped(false), _stopOnDone(false) {
	_model = m;
	//if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	//}
}

Animation::~Animation() {
	for (int i = 0; i < _numKeys; ++i) {
		delete[] _keyframes[i]._bones;
	}
	delete[] _keyframes;
}

void Animation::loadLBA2(Common::SeekableReadStream *stream) {
	_numKeys = stream->readUint16LE();
	_numBones = stream->readUint16LE();
	_startFrame = stream->readUint16LE();
	uint16 unknown1 = stream->readUint16LE();

	float angleMult = 0;
	if (g_twin->getGameType() == GType_LBA2) {
		angleMult = 360.0f/4096;
	} else {
		angleMult = 1.0f/180;
	}

	_keyframes = new Keyframe[_numKeys];
	for (int i = 0; i < _numKeys; ++i) {
		Keyframe *k = &_keyframes[i];
		k->_length = stream->readUint16LE();
		if (g_twin->getGameType() == GType_LBA) {
			k->_length *= 15;
		}
		k->_x = stream->readSint16LE();
		k->_y = stream->readSint16LE();
		k->_z = stream->readSint16LE();
		k->_noFalling = stream->readSint32LE();
		stream->readSint32LE();
		stream->seek(-8, SEEK_CUR);
		k->_bones = new Boneframe[_numBones];
		for (int j = 0; j < _numBones; ++j) {
			Boneframe *b = &k->_bones[j];
			b->_type = stream->readUint16LE();
			float x = stream->readSint16LE();
			float y = stream->readSint16LE();
			float z = stream->readSint16LE();
			if (b->_type == 0) {
				b->_pitch = x * angleMult;
				b->_roll  = y * angleMult;
				b->_yaw   = z * angleMult;
			} else {
				b->_pos.set(x / 16384.0f, y / 16384.0f, z / 16384.0f);
			}
		}
	}
	_currentFrame = 0;
	_time = 0;
}

void Animation::update(uint32 time) {
	if (_stopped) {
		return;
	}
	_time += time;
	Keyframe *k = &_keyframes[_currentFrame];
	if (_time > k->_length) {
		_time = 0;
		++_currentFrame;
		if (_currentFrame >= _numKeys) {
			if (_stopOnDone) {
				_stopped = true;
				--_currentFrame;
				_time = k->_length;
			} else {
				_currentFrame = _startFrame;
			}
			if (_isWaiting) {
				--_waitLoops;
				if (_waitLoops == 0) {
					*_isWaiting = false;
					_isWaiting = nullptr;
				}
			}
		}
		k = &_keyframes[_currentFrame];
	}
	Keyframe *next;
	if (_currentFrame + 1 != _numKeys) {
		next = &_keyframes[_currentFrame + 1];
	} else {
		next = &_keyframes[_startFrame];
	}
	float interp = (float)_time / k->_length;
	int numBones = _model->_numBones;
	if (numBones > _numBones) {
		numBones = _numBones;
	}

	for (int i = 0; i < numBones; ++i) {
		Boneframe *b = &k->_bones[i];
		Boneframe *bn = &next->_bones[i];
		Hierarchy *h = &_model->_heirs[i];
		if (b->_type == 0) {
			h->_isTransltion = false;
			h->_pitch = b->_pitch + (((((int)(bn->_pitch - b->_pitch).getDegrees() % 360) + 540) % 360) - 180) * interp;
			h->_yaw = b->_yaw + (((((int)(bn->_yaw - b->_yaw).getDegrees() % 360) + 540) % 360) - 180) * interp;
			h->_roll = b->_roll + (((((int)(bn->_roll - b->_roll).getDegrees() % 360) + 540) % 360) - 180) * interp;
		} else {
			h->_isTransltion = true;
			h->_translation = b->_pos + (bn->_pos - b->_pos) * interp;
		}
	}
	_model->recalculateHierarchy();
}


} // end of namespace Twin
