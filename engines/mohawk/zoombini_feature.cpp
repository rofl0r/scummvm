/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
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

#include "mohawk/zoombini.h"
#include "mohawk/resource.h"
#include "mohawk/cursors.h"
#include "mohawk/sound.h"
#include "mohawk/video.h"

#include "common/error.h"
#include "common/events.h"
#include "common/EventRecorder.h"

#include "engines/util.h"

namespace Mohawk {

SnoidFeature::SnoidFeature(MohawkEngine_Zoombini *vm) : OldFeature(vm) {
}

bool SnoidFeature::nextPointOnPath() {
	MohawkEngine_Zoombini *_vm = (MohawkEngine_Zoombini *)_view; // XXX: hack

	if (!_vm->_nodes.empty() && !_vm->_paths.empty() && !_vm->_snoidPathUpdatesDisabled) {
		Common::Point dest = _snoidData.dest;
		if (_snoidData.currentNode != 0xff) { // TODO: original checks for < 0x7f
			uint node = _vm->_paths[_snoidData.currentPath][_snoidData.currentNode];
			_snoidData.currentNode += _snoidData.pathStep;
			if (node) {
				Common::Point pos = _vm->_nodes[node - 1];
				if (_data.currentPos.sqrDist(_snoidData.dest) > _data.currentPos.sqrDist(pos))
					dest = pos;
			}
		}
		_data.nextPos = dest;
	}

	if (_data.currentPos == _data.nextPos)
		return false;

	int xDiff = _data.nextPos.x - _data.currentPos.x;
	int yDiff = _data.currentPos.y - _data.nextPos.y;

	uint16 old245 = _snoidData.unknown245;

	int val;
	if (xDiff) {
		val = (yDiff * 1024) / abs(xDiff);
	} else {
		if (yDiff >= 0)
			val = 1410;
		else
			val = -1410;
	}

	int16 x, y;
	if (val <= -1409) {
		_snoidData.unknown245 = 0;
		x = 5;
		y = -15;
	} else if (val <= -332) {
		_snoidData.unknown245 = 1;
		x = 13;
		y = -10;
	} else if (val < 332) {
		_snoidData.unknown245 = 2;
		x = 16;
		y = 8;
	} else if (val < 1409) {
		_snoidData.unknown245 = 3;
		x = 13;
		y = 10;
	} else {
		_snoidData.unknown245 = 4;
		x = 5;
		y = 15;
	}

	if (abs(x) < abs(y)) {
		_snoidData.stepSize.y = y;
		int div = yDiff / abs(y);
		if (div)
			_snoidData.stepSize.x = xDiff / div;
		else
			_snoidData.stepSize.x = xDiff;
		if (!_snoidData.stepSize.x && xDiff)
			_snoidData.stepSize.x = xDiff / abs(xDiff);
	} else {
		_snoidData.stepSize.x = x;
		int div = xDiff / abs(x);
		if (div)
			_snoidData.stepSize.y = yDiff / div;
		else
			_snoidData.stepSize.y = yDiff;
		if (!_snoidData.stepSize.y && yDiff)
			_snoidData.stepSize.y = yDiff / abs(yDiff);
	}

	if (_snoidData.unknown245 != old245) {
		_data.scrbIndex = _snoidData.unknown245 + 5 * _snoidData.part[3];
		_data.currOffset = _vm->offsetToFrame(true, _data.scrbIndex, _data.currFrame);
		Common::SeekableReadStream *ourSCRS = _vm->getResource(ID_SCRS, _data.scrbIndex + 100);
		ourSCRS->skip(2);
		setSnoidDrawOrder(ourSCRS->readUint16BE());
		delete ourSCRS;
	}

	return true;
}

void SnoidFeature::walkSnoidToPoint(Common::Point pos) {
	MohawkEngine_Zoombini *_vm = (MohawkEngine_Zoombini *)_view; // XXX: hack

	if (_vm->_nodes.empty() || _vm->_paths.empty() || _vm->_snoidPathUpdatesDisabled) {
		_data.nextPos = _snoidData.dest;
		return;
	}

	// Find the node which is closest to our destination.
	uint nearestNode = 0;
	uint nearestNodeDist = 999999;
	for (uint i = 0; i < _vm->_nodes.size(); i++) {
		uint thisNodeDist = _vm->_nodes[i].sqrDist(pos);
		if (thisNodeDist <= nearestNodeDist) {
			nearestNodeDist = thisNodeDist;
			nearestNode = i + 1;
		}
	}
	if (!nearestNode)
		error("walkSnoidToPoint didn't find a nearest node");

	// Then, find a path with that node in it, and find the node
	// in that path which is closest to our current position.
	nearestNodeDist = 999999;
	for (uint i = 0; i < _vm->_paths.size(); i++) {
		for (uint j = 0; j < _vm->_paths[i].size(); j++) {
			if (_vm->_paths[i][j] != nearestNode)
				continue;

			for (uint k = 0; k < _vm->_paths[i].size(); k++) {
				if (!_vm->_paths[i][k])
					continue;

				byte thisNode = _vm->_paths[i][k];
				uint thisNodeDist = _vm->_nodes[thisNode - 1].sqrDist(_data.currentPos);
				if (thisNodeDist <= nearestNodeDist) {
					nearestNodeDist = thisNodeDist;
					_snoidData.currentNode = k + 1;
					_snoidData.currentPath = i;
					if (j && k >= j)
						_snoidData.pathStep = -1;
					else
						_snoidData.pathStep = 1;
				}
			}

			break;
		}
	}
}

void SnoidFeature::setNewSnoidModeAndXY(Common::Point pos, uint mode) {
	if (mode > 10)
		mode = 0;

	MohawkEngine_Zoombini *_vm = (MohawkEngine_Zoombini *)_view; // XXX: hack
	_vm->_syncChannels[_data.syncChannel].masterId = 0;
	_data.syncChannel = 0;

	if (mode == 3 && !_snoidData.mode) {
		error("meep"); // FIXME
		if (_snoidData.unknown241 != 1) // TODO: ...
			_snoidData.unknown241 = 1;

		for (uint i = 0; i < 5; i++) {
			uint id = 0;
			switch (i) {
			case 0:
				if (_snoidData.unknown245 & 8)
					id = _snoidData.part[3] + 435;
				break;
			case 2:
				if (_snoidData.unknown245 & 4)
					id = _snoidData.part[2] + 440;
				break;
			case 3:
				if (_snoidData.unknown245 & 2)
					id = _snoidData.part[1] + 430;
				break;
			case 4:
				if (_snoidData.unknown245 & 1)
					id = _snoidData.part[0] + 425;
				break;
			}
			if (id) {
				id = (2 * id) - 1;
				if (_snoidData.unknown242)
					id++;
				_data.bitmapIds[i + 10] = id;
			} else {
				_data.bitmapIds[i + 10] = _data.bitmapIds[i];
			}
		}

		_snoidData.unknown245 = 0;
		_snoidData.mode = 3;
		return;
	}

	uint16 frame = 0;
	uint16 scrbIndex = _snoidData.unknown241;

	switch (mode) {
	case 0:
	case 1:
	case 2:
		// < 3: do nothing
		break;
	case 3:
		// == 3: do nothing
		break;
	case 4:
		_snoidData.pathStep = 0;
		break;
	case 5:
		if (false /* TODO: small snoids */)
			break;
		scrbIndex = _snoidData.part[3] + 45;
		if (_snoidData.unknown241 == 1) {
			frame = 1;
		} else if (_snoidData.unknown241 == 2) {
			frame = 2;
		}
		break;
	case 6:
		if (_snoidData.unknown241 > 2)
			break;
		if (_snoidData.unknown241 == 0)
			_snoidData.unknown241 = 1;
		switch (_snoidData.unknown241) {
		case 1:
			scrbIndex = _snoidData.unknown245 + 30;
			break;
		case 2:
			scrbIndex = _snoidData.unknown245 + 38;
			break;
		}
		if (_vm->_idleWaitTime && true /* TODO: idle sound option? */) {
			static uint counter = 0;
			counter++;
			counter %= 32;
			if (counter == 0) {
				// TODO: kill all sounds from 100 to 424 inclusive?
			}
			uint16 soundId = 4;
			if (_vm->_rnd->getRandomNumberRng(1, 100) > 50)
				soundId = 5;
			_vm->queueSound(_vm->getSnoidSoundId(soundId, &_snoidData));
		}
		break;
	case 7:
		scrbIndex = _snoidData.unknown245 + 5 * _snoidData.part[3];
		break;
	case 8:
	case 9:
		mode = 0;
		break;
	case 10:
		// == 10: do nothing
		break;
	}

	_snoidData.drawOrder = 0xffff;
	_snoidData.mode = mode;
	if (mode)
		_snoidData.unknown245 = 1;
	else
		_snoidData.unknown245 = 0;
	_data.enabled = 1;

	if (pos.x && pos.y) {
		// TODO: original checks for NULL pointer
		_data.currentPos = pos;
	}

	_data.currFrame = 0;
	_data.currOffset = 2;
	_data.scrbIndex = scrbIndex;
	if (frame) {
		_data.currFrame = frame;
		_data.currOffset = _vm->offsetToFrame(true, scrbIndex, frame);
	}
	Common::SeekableReadStream *ourSCRS = _vm->getResource(ID_SCRS, 100 + scrbIndex);
	_data.endFrame = ourSCRS->readUint16BE();

	// TODO: or bounds onto view rgn
	setSnoidDrawOrder(ourSCRS->readUint16BE());
	setSnoidBounds(NULL);
	// TODO: or bounds onto view rgn again

	delete ourSCRS;
}

uint16 SnoidFeature::setSnoidBounds(uint16 *something) {
	MohawkEngine_Zoombini *_vm = (MohawkEngine_Zoombini *)_view; // XXX: hack

	if (something)
		*something = 0;

	_data.bounds = Common::Rect();

	Common::SeekableReadStream *ourScript;

	Common::Array<uint16> *regsX, *regsY;
	uint16 *next194 = _snoidData.unknown194;
	uint16 someCount;
	int xOffset, yOffset;
	if (_snoidData.mode == 8) {
		// TODO: XXX
		error("meh");
	} else if (_snoidData.mode == 9) {
		// TODO: XXX
		error("meh");
	} else {
		someCount = 5;
		ourScript = _vm->getResource(ID_SCRS, _data.scrbIndex + 100);
		ourScript->seek(2 * _data.currOffset);
		next194++;
		xOffset = _data.currentPos.x;
		yOffset = _data.currentPos.y;
		regsX = &_vm->regs100;
		regsY = &_vm->regs101;
	}

	uint16 lastSound = 0;
	// Original code has two copies of this loop just for the unknown242 check..
	uint bitmapId = 0;
	for (uint i = 0; i <= someCount; i++) {
		uint16 thisVal = ourScript->readUint16BE();
		if (thisVal == 0) {
			_data.bitmapIds[bitmapId] = 0;
			_data.bitmapPos[bitmapId] = Common::Point();
			bitmapId++;
			ourScript->skip(4);
		} else if (thisVal <= 0x7fff) {
			thisVal = 2 * (thisVal + *(next194 + i));
			if (!_snoidData.unknown242)
				thisVal--;
			_data.bitmapIds[bitmapId] = thisVal;
			_data.bitmapPos[bitmapId].x = ourScript->readUint16BE() + xOffset - (*regsX)[thisVal];
			_data.bitmapPos[bitmapId].y = ourScript->readUint16BE() + yOffset - (*regsY)[thisVal];
			bitmapId++;
		} else {
			if (thisVal < 0xff00) {
				lastSound = ourScript->readUint16BE();
			}
			if (something)
				*something = (byte)thisVal;
			if (i)
				_data.bitmapIds[bitmapId] = 0;
			break;
		}
	}

	// update bounds
	uint16 compoundSHAPId = 3000;
	if (_snoidData.mode == 9)
		compoundSHAPId = 3100;
	for (uint i = 0; i < 24; i++) {
		bitmapId = _data.bitmapIds[i];
		if (!bitmapId) // || bitmapId > compoundSHAP.size()
			break;
		uint16 width, height;
		_vm->_gfx->getSubImageSize(compoundSHAPId, bitmapId - 1, width, height);
		Common::Rect bitmapRect(width, height);
		bitmapRect.moveTo(_data.bitmapPos[i]);
		if (_data.bounds.isEmpty())
			_data.bounds = bitmapRect;
		else
			_data.bounds.extend(bitmapRect);
	}

	if (something) {
		_data.currFrame++;
		_data.currOffset = ourScript->pos() / 2;
	}

	delete ourScript;

	return lastSound;
}

void SnoidFeature::setSnoidDrawOrder(uint16 index) {
	if (index > 2)
		error("bad index %d for setSnoidDrawOrder", index);

	if (index == _snoidData.drawOrder)
		return;
	_snoidData.drawOrder = index;

	// TODO: de-duplicate all this code
	if (_snoidData.mode == 9) {
		static const uint16 orders0[6] = { 0, 18, 72, 36, 54, 90 };
		static const uint16 orders1[6] = { 0, 108, 126, 144, 162, 180 };
		static const uint16 orders2[6] = { 0, 198, 216, 234, 252, 270 };
		static const uint16 orders3[6] = { 0, 288, 306, 324, 342, 360 };
		switch (index) {
		case 0:
			_snoidData.unknown194[1] = orders3[_snoidData.part[3]];
			_snoidData.unknown194[2] = 0;
			_snoidData.unknown194[3] = orders2[_snoidData.part[2]];
			_snoidData.unknown194[4] = orders1[_snoidData.part[1]];
			break;
		case 1:
			_snoidData.unknown194[1] = orders3[_snoidData.part[3]];
			_snoidData.unknown194[2] = orders2[_snoidData.part[2]];
			_snoidData.unknown194[3] = 0;
			_snoidData.unknown194[4] = orders1[_snoidData.part[1]];
			break;
		case 2:
			_snoidData.unknown194[1] = 0;
			_snoidData.unknown194[2] = orders1[_snoidData.part[1]];
			_snoidData.unknown194[3] = orders2[_snoidData.part[2]];
			_snoidData.unknown194[4] = orders3[_snoidData.part[3]];
			break;
		}
		_snoidData.unknown194[5] = orders0[_snoidData.part[0]];
	} else {
		static const uint16 orders0[6] = { 0, 11, 27, 43, 59, 75 };
		static const uint16 orders1[6] = { 0, 91, 107, 123, 139, 155 };
		static const uint16 orders2[6] = { 0, 171, 175, 179, 183, 187 };
		static const uint16 orders3[6] = { 0, 191, 246, 335, 360, 411 };
		switch (index) {
		case 0:
			_snoidData.unknown194[1] = orders3[_snoidData.part[3]];
			_snoidData.unknown194[2] = 0;
			_snoidData.unknown194[3] = orders2[_snoidData.part[2]];
			_snoidData.unknown194[4] = orders1[_snoidData.part[1]];
			break;
		case 1:
			_snoidData.unknown194[1] = orders3[_snoidData.part[3]];
			_snoidData.unknown194[2] = orders2[_snoidData.part[2]];
			_snoidData.unknown194[3] = 0;
			_snoidData.unknown194[4] = orders1[_snoidData.part[1]];
			break;
		case 2:
			_snoidData.unknown194[1] = 0;
			_snoidData.unknown194[2] = orders1[_snoidData.part[1]];
			_snoidData.unknown194[3] = orders2[_snoidData.part[2]];
			_snoidData.unknown194[4] = orders3[_snoidData.part[3]];
			break;
		}
		_snoidData.unknown194[5] = orders0[_snoidData.part[0]];
	}
}

void Zoombini_Module::defaultMoveProc(OldFeature *feature) {
	if (_vm->_inDialog)
		return;

	const uint16 syncChannel = feature->_data.syncChannel;

	if (!feature->_data.enabled) {
		if (syncChannel) {
			_vm->_syncChannels[syncChannel].masterId = 0;
			feature->_data.syncChannel = 0;
		}
		return;
	}

	bool live = feature->_nextTime <= _vm->_lastIdleTime;
	if (syncChannel) {
		if (!_vm->_syncChannels[syncChannel].masterId)
			_vm->_syncChannels[syncChannel].masterId = feature->_id;

		if (_vm->_syncChannels[syncChannel].masterId == feature->_id) {
			if (_vm->_syncChannels[syncChannel].alternate && _vm->_syncChannels[syncChannel].state) {
				live = false;
			} else {
				if (live)
					_vm->_syncChannels[syncChannel].state = 1;
				else
					_vm->_syncChannels[syncChannel].state = 0;
			}
		} else {
			if (_vm->_syncChannels[syncChannel].alternate) {
				if (_vm->_syncChannels[syncChannel].state == 1) {
					_vm->_syncChannels[syncChannel].state = 2;
					live = false;
				} else if (_vm->_syncChannels[syncChannel].state == 2) {
					_vm->_syncChannels[syncChannel].state = 0;
					live = true;
				}
			} else {
				live = _vm->_syncChannels[syncChannel].state != 0;
			}
		}
	}

	if (!live)
		return;

	bool someFlag = false;
	bool wasDisabledByReset = feature->_needsReset;

	if (feature->_needsReset) {
		feature->resetFeatureScript(1, feature->_scrbId);
		feature->_justReset = 0;
		if (feature->_data.endFrame < 1 || feature->_flags & kFeatureOldDisableOnReset || feature->_flags & kFeatureOldDisable)
			feature->_data.enabled = 0;
		if (feature->_flags & kFeatureOldDisabled) {
			feature->_data.enabled = 0;
		} else {
			feature->_dirty = 1;
			wasDisabledByReset = false;
		}
		if (feature->_flags & kFeatureInternalRegion) {
			// TODO: create region
		}
	} else {
		if (!(feature->_flags & kFeatureOldNoClip)) {
			if (feature->_data.useClipRect) {
				// TODO: or clip with _unknown228
			} else if (feature->_region) {
				// TODO: or clip with region
			} else {
				// TODO: or clip with bounds
			}
		}

		feature->_dirty = 1;

		if (feature->_data.currFrame < feature->_data.endFrame) {
			if (feature->_flags & kFeatureOldRandom) {
				feature->_data.currFrame = _vm->_rnd->getRandomNumberRng(0, feature->_data.endFrame);
				feature->_data.currOffset = _vm->offsetToFrame(false, feature->_data.scrbIndex, feature->_data.currFrame);
			} else {
				if (feature->_justReset)
					feature->_justReset = 0;
				else
					feature->_data.currFrame++;
			}
		} else {
			bool noLoop = true;
			if (feature->_flags & kFeatureOldAlternateScripts) {
				someFlag = true;
				if (feature->_storedScrbId) {
					int16 tmp = feature->_storedScrbId;
					feature->_storedScrbId = 0;
					if (tmp >= 0) {
						feature->resetFeatureScript(1, tmp);
					} else {
						feature->resetFeatureScript(1, -tmp);
						feature->_flags &= kFeatureOldRandom;
					}
					feature->_storedScrbId = 0;
					if (feature->_flags & kFeatureOldDropSpot)
						feature->_data.enabled = 0;
				}
				if (feature->_data.endFrame < 2)
					feature->_data.enabled = 0;
				feature->_data.currFrame = 0;
				feature->_data.currOffset = 1;
				noLoop = false;
			}
			if (feature->_flags & kFeatureDisableOnEnd) {
				someFlag = true;
				if (feature->_data.syncChannel) {
					_vm->_syncChannels[feature->_data.syncChannel].masterId = 0;
					_vm->_syncChannels[feature->_data.syncChannel].alternate = false;
					feature->_data.syncChannel = 0;
				}
				feature->_data.enabled = 0;
				if (noLoop) {
					if (feature->_notifyDone) {
						if (feature->_doneProc)
							(this->*(feature->_doneProc))(feature); // TODO: -1
					}
					feature->_doneProc = NULL;
					return;
				}
			} else {
				feature->_data.currFrame = 0;
				feature->_data.currOffset = 1;
			}
		}

		if (feature->_flags & kFeatureOldDisable) {
			feature->_data.enabled = 0;
		}

		if (feature->_flags & kFeatureOldReset) {
			feature->_flags &= ~kFeatureOldReset;
			feature->_data.currFrame = 0;
			feature->_data.currOffset = 1;
			feature->_data.enabled = 0;
		}
	}

	feature->_nextTime = _vm->_lastIdleTime + feature->_delayTime;
	Common::SeekableReadStream *ourSCRB = _vm->getSCRB(feature->_data.scrbIndex);
	ourSCRB->seek(feature->_data.currOffset * 2);

	int16 xOffset = 0, yOffset = 0;
	if (feature->_flags & kFeatureOldAdjustByPos) {
		xOffset = feature->_data.currentPos.x - feature->_data.nextPos.x;
		yOffset = feature->_data.currentPos.y - feature->_data.nextPos.y;
	}

	uint16 lastVal = ourSCRB->readUint16BE();
	bool validVal = (lastVal <= 0x7fff); // TODO: consider
	for (uint i = 0; i < 24; i++) {
		if (i > 0)
			lastVal = ourSCRB->readUint16BE();
		if (lastVal) {
			if (lastVal > 0x7fff) {
				if (lastVal < 0xff00) {
					uint16 resourceId = ourSCRB->readUint16BE();
					if (!wasDisabledByReset)
						_vm->queueSound(resourceId);
				}
				// TODO: if we were passed a region and the first byte(?) is set
				//   then call that weird +16 proc with (that byte)-1
				if (i != 23) {
					if (i == 0) {
						feature->_data.currOffset = ourSCRB->pos() / 2;
						if (someFlag) {
							if (feature->_notifyDone) {
								if (feature->_doneProc)
									(this->*(feature->_doneProc))(feature);
							}
							feature->_doneProc = NULL;
						}
						delete ourSCRB;
						return;
					}
					feature->_data.bitmapIds[i] = 0;
					i = 23;
				}
			} else {
				feature->_data.bitmapIds[i] = lastVal;
				feature->_data.bitmapPos[i].x = ourSCRB->readUint16BE() + xOffset;
				feature->_data.bitmapPos[i].y = ourSCRB->readUint16BE() + yOffset;
			}
		} else {
			feature->_data.bitmapIds[i] = 0;
			feature->_data.bitmapPos[i] = Common::Point();
			ourSCRB->skip(4);
		}
	}

	feature->_data.currOffset = ourSCRB->pos() / 2;
	if (someFlag) {
		if (feature->_notifyDone) {
			if (feature->_doneProc)
				(this->*(feature->_doneProc))(feature);
		}
		feature->_doneProc = NULL;
	}
	if (validVal) {
		if (feature->_frameProc)
			(this->*(feature->_frameProc))(feature);
		// FIXME: adjust by registration (using _compoundSHAPIndex) if necessary
		if (feature->_data.useClipRect) {
			// TODO: rects/regions
		} else if (feature->_flags & kFeatureInternalRegion) {
			// TODO: rects/regions
		} else {
			// TODO: rects/regions
			feature->_data.bounds = Common::Rect();
			uint16 compoundSHAPId = _vm->getCompoundSHAPId(feature->_data.compoundSHAPIndex);
			for (uint i = 0; i < 24; i++) {
				uint16 bitmapId = feature->_data.bitmapIds[i];
				if (!bitmapId) // || bitmapId > compoundSHAP.size()
					break;
				uint16 width, height;
				_vm->_gfx->getSubImageSize(compoundSHAPId, bitmapId - 1, width, height);
				Common::Rect bitmapRect(width, height);
				bitmapRect.moveTo(feature->_data.bitmapPos[i]);
				if (feature->_data.bounds.isEmpty())
					feature->_data.bounds = bitmapRect;
				else
					feature->_data.bounds.extend(bitmapRect);
			}
		}
	}
	delete ourSCRB;
}

void Zoombini_Module::defaultDrawProc(OldFeature *feature) {
	if (!feature->_data.enabled && (feature->_flags & kFeatureOldDisabled))
		return;

	if (feature->_data.useClipRect) {
		// TODO: mark _unknown228 as clip
	}
	uint16 compoundSHAPId = _vm->getCompoundSHAPId(feature->_data.compoundSHAPIndex);
	for (uint i = 0; i < 24; i++) {
		uint16 bitmapId = feature->_data.bitmapIds[i];
		if (!bitmapId) // || bitmapId > compoundSHAP.size()
			break;
		_vm->_gfx->copyAnimSubImageToScreen(compoundSHAPId, bitmapId - 1, feature->_data.bitmapPos[i].x, feature->_data.bitmapPos[i].y);
	}
	if (feature->_data.useClipRect) {
		// TODO: restore clip rgn
	}
}

void Zoombini_Module::snoidMoveProc(SnoidFeature *feature) {
	if (!feature->_data.enabled)
		return;

	if (_vm->_inDialog)
		return;

	bool live = feature->_nextTime <= _vm->_lastIdleTime;
	if (feature->_data.syncChannel) {
		const uint16 syncChannel = feature->_data.syncChannel;
		if (!_vm->_syncChannels[syncChannel].masterId)
			_vm->_syncChannels[syncChannel].masterId = feature->_id;

		if (_vm->_syncChannels[syncChannel].masterId == feature->_id) {
			if (live)
				_vm->_syncChannels[syncChannel].state = 1;
			else
				_vm->_syncChannels[syncChannel].state = 0;
		} else {
			live = _vm->_syncChannels[syncChannel].state != 0;
		}
	}

	if (!live)
		return;

	feature->_nextTime = _vm->_lastIdleTime + feature->_delayTime;

	bool someFlag = false;
	switch (feature->_snoidData.mode) {
	case 1:
	case 2:
		feature->_snoidData.unknown248 = 0;
		if (feature->_snoidData.mode == 1) {
			if (feature->_snoidData.unknown242) {
				feature->_snoidData.unknown241 = 1;
				feature->_snoidData.mode = 0;
			} else {
				if (feature->_snoidData.unknown241 != 2) {
					feature->_snoidData.unknown241 = 0;
					feature->_snoidData.unknown242 = 1;
				} else {
					feature->_snoidData.unknown241 = 1;
				}
			}
		} else {
			if (feature->_snoidData.unknown242) {
				if (feature->_snoidData.unknown241 != 2) {
					feature->_snoidData.unknown241 = 0;
					feature->_snoidData.unknown242 = 0;
				} else {
					feature->_snoidData.unknown241 = 1;
				}
			} else {
				feature->_snoidData.unknown241 = 1;
				feature->_snoidData.mode = 0;
			}
		}
		someFlag = true;
		// fall-through
	case 0:
		if (feature->_snoidData.unknown245) {
			feature->_snoidData.unknown245 = 0;
			someFlag = true;
		}
		if (_vm->_idleWaitTime && !_vm->_inDialog) {
			byte idleTime = feature->_snoidData.unknown248++;
			if (idleTime > _vm->_idleWaitTime) {
				feature->_snoidData.unknown248 = 0;
				if (true /* FIXME: not small snoids */) {
					if (_vm->_rnd->getRandomNumberRng(0, 100) < 10) {
						feature->_snoidData.unknown245 = _vm->_rnd->getRandomNumberRng(0, 7);
						feature->setNewSnoidModeAndXY(Common::Point(), 6);
					}
				}
			}
		}
		break;

	case 3:
		if (feature->_snoidData.unknown245 >= 6) {
			feature->setNewSnoidModeAndXY(Common::Point(), 0);
			feature->_snoidData.unknown245 = 0;
		} else {
			for (uint i = 0; i < 5; i++) {
				SWAP(feature->_data.bitmapIds[i], feature->_data.bitmapIds[i + 10]);
			}
			feature->_snoidData.unknown245++;
		}
		feature->_dirty = 1;
		return;

	case 4:
		someFlag = true;
		if (feature->_snoidData.dest == feature->_data.currentPos) {
			feature->_delayTime = 6;
			feature->_snoidData.unknown248 = 0;
			feature->setNewSnoidModeAndXY(Common::Point(), _vm->_snoidDirectionAfterFall);
		} else {
			feature->_snoidData.unknown241 = 1;
			feature->_snoidData.unknown242 = 0;
			feature->_data.currentPos = feature->_snoidData.dest;
		}
		break;

	case 5:
		someFlag = true;
		break;

	case 6:
		someFlag = true;
		break;

	default:
		if (feature->_snoidData.mode == 10) {
			feature->setNewSnoidModeAndXY(Common::Point(), 7);
			_vm->_numMovingSnoids++;
			return;
		} else if (feature->_snoidData.mode >= 10) {
			if (feature->_snoidData.mode != 112)
				break;
		} else if (feature->_snoidData.mode == 7) {
			feature->walkSnoidToPoint(feature->_snoidData.dest);
			feature->nextPointOnPath();
			feature->_snoidData.mode = 112;
		} else {
			someFlag = true;
			break;
		}

		int x = feature->_data.nextPos.x - feature->_data.currentPos.x;
		int y = feature->_data.currentPos.y - feature->_data.nextPos.y;
		if (!x && !y && !feature->nextPointOnPath()) {
			// TODO: or bounds into global view rgn(?!)
			feature->_snoidData.unknown248 = 0;
			feature->setNewSnoidModeAndXY(Common::Point(), _vm->_snoidDirectionAfterFall);
			if (_vm->_numMovingSnoids) {
				_vm->_numMovingSnoids--;
				_vm->_numIdleSnoids++;
			}
			if (_vm->_bridgeStaticSnoidHack) {
				if (feature->_snoidData.inPartyStatus == 2)
					feature->_flags |= kFeatureSortStatic;
			}
			if (_vm->_checkSnoidDropSpotsDuringMove) {
				_vm->checkDropSpotsFor(feature);
			}
			_vm->checkRestAreasFor(feature);
			// FIXME: snoid walk callback
		} else {
			int x2 = abs(feature->_snoidData.stepSize.x);
			int y2 = abs(feature->_snoidData.stepSize.y);
			if (x > 0) {
				feature->_data.currentPos.x += MIN(abs(x), x2);
				feature->_snoidData.unknown242 = 0;
			} else if (x < 0) {
				feature->_data.currentPos.x -= MIN(abs(x), x2);
				feature->_snoidData.unknown242 = 1;
			}
			if (y > 0) {
				feature->_data.currentPos.y -= MIN(abs(y), y2);
			} else if (y < 0) {
				feature->_data.currentPos.y += MIN(abs(y), y2);
			}
		}
		someFlag = true;
		break;
	}

	if (!someFlag)
		return;

	// TODO: or bounds

	if (feature->_snoidData.mode == 4 || feature->_data.endFrame <= 1) {
		feature->setSnoidBounds(NULL);
		feature->_dirty = 1;
		return;
	}

	// The flow control here in the original (in fact, in the whole function) is a bit weird.
	if (feature->_data.currFrame >= feature->_data.endFrame) {
		if (feature->_snoidData.mode == 5) {
			feature->_doneProc = NULL;
			feature->_data.currFrame = 2;
			// FIXME: if small snoids is set, set _currFrame to 0
			feature->_data.currOffset = _vm->offsetToFrame(true, feature->_data.scrbIndex, feature->_data.currFrame);
		} else {
			if (feature->_snoidData.mode >= 10) {
				if (feature->_snoidData.mode != 112) {
					// TODO: or bounds into global view rgn(?!)
					feature->setNewSnoidModeAndXY(Common::Point(), 0);
					feature->_doneProc = NULL;
					feature->_dirty = 1;
					return;
				}
			} else if (feature->_snoidData.mode != 7) {
				if (feature->_snoidData.unknown248 == 1) {
					_vm->_syncChannels[feature->_data.syncChannel].masterId = 0;
					feature->_data.syncChannel = 0;
					feature->_data.enabled = 0;
					feature->_data.currFrame = 0;
					feature->_data.currOffset = 2;
				} else {
					// TODO: or bounds into global view rgn(?!)
					feature->setNewSnoidModeAndXY(Common::Point(), 0);
				}
				if (feature->_notifyDone) {
					if (feature->_doneProc)
						(this->*(feature->_doneProc))(feature);
				}
				feature->_doneProc = NULL;
				feature->_dirty = 1;
				return;
			}
			feature->_data.currFrame = 1;
			feature->_data.currOffset = _vm->offsetToFrame(true, feature->_data.scrbIndex, feature->_data.currFrame);
		}
	}

	uint16 something;
	uint16 soundId = feature->setSnoidBounds(&something);
	if (soundId)
		_vm->queueSound(soundId);
	if (something) {
		something--;
		if (something < 200 || something > 239) {
			// TODO: call +16 if it is set
		} else if (something < 218) {
			static uint16 soundIds[18] = {8,6,7,10,2,12,1,9,0,4,5,3,11,13,14,15,16,17};
			soundId = _vm->getSnoidSoundId(soundIds[something - 200], &feature->_snoidData);
			_vm->queueSound(soundId);
		}
	}
	feature->_dirty = 1;
}

void Zoombini_Module::snoidDrawProc(SnoidFeature *feature) {
	if (!feature->_data.enabled)
		return;
	if (feature->_data.useClipRect) {
		// TODO: mark _unknown228 as clip
	}
	uint16 compoundSHAPId = 3000;
	if (feature->_snoidData.mode == 9)
		compoundSHAPId = 3100;
	for (uint i = 0; i < 24; i++) {
		uint16 bitmapId = feature->_data.bitmapIds[i];
		if (!bitmapId) // || bitmapId > compoundSHAP.size()
			break;
		_vm->_gfx->copyAnimSubImageToScreen(compoundSHAPId, bitmapId - 1, feature->_data.bitmapPos[i].x, feature->_data.bitmapPos[i].y);
	}
	if (feature->_data.useClipRect) {
		// TODO: restore clip rgn
	}
}

} // End of namespace Mohawk
