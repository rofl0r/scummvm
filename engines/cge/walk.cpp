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

/*
 * This code is based on original Soltys source code
 * Copyright (c) 1994-1995 Janus B. Wisniewski and L.K. Avalon
 */

#include "cge/walk.h"
#include "cge/cge_main.h"

namespace CGE {

Walk *_hero;

uint8 Cluster::_map[kMapZCnt][kMapXCnt];
CGEEngine *Cluster::_vm;

void Cluster::init(CGEEngine *vm) {
	_vm = vm;
}

uint8 &Cluster::cell() {
	return _map[_b][_a];
}

bool Cluster::isValid() const {
	return (_a >= 0) && (_a < kMapXCnt) && (_b >= 0) && (_b < kMapZCnt);
}

bool Cluster::chkBar() const {
	assert(_vm->_now <= _vm->_caveMax);
	return (_a == _vm->_barriers[_vm->_now]._horz) && (_b == _vm->_barriers[_vm->_now]._vert);
}

Cluster XZ(int x, int y) {
	if (y < kMapTop)
		y = kMapTop;

	if (y > kMapTop + kMapHig - kMapGridZ)
		y = kMapTop + kMapHig - kMapGridZ;

	return Cluster(x / kMapGridX, (y - kMapTop) / kMapGridZ);
}


Cluster XZ(Couple xy) {
	signed char x, y;
	xy.split(x, y);
	return XZ(x, y);
}

Walk::Walk(CGEEngine *vm, BitmapPtr *shpl)
	: Sprite(vm, shpl), _dir(kDirNone), _tracePtr(-1), _level(0), _target(-1, -1), _vm(vm) {
}


void Walk::tick() {
	if (_flags._hide)
		return;

	_here = XZ(_x + _w / 2, _y + _h);

	if (_dir != kDirNone) {
		Sprite *spr;
		_sys->funTouch();
		for (spr = _vga->_showQ->first(); spr; spr = spr->_next) {
			if (distance(spr) < 2) {
				if (!spr->_flags._near) {
					_vm->feedSnail(spr, kNear);
					spr->_flags._near = true;
				}
			} else {
				spr->_flags._near = false;
			}
		}
	}

	if (_flags._hold || _tracePtr < 0)
		park();
	else {
		if (_here == _trace[_tracePtr]) {
			if (--_tracePtr < 0)
				park();
		} else {
			signed char dx, dz;
			(_trace[_tracePtr] - _here).split(dx, dz);
			Dir d = (dx) ? ((dx > 0) ? kDirEast : kDirWest) : ((dz > 0) ? kDirSouth : kDirNorth);
			turn(d);
		}
	}
	step();
	if ((_dir == kDirWest  && _x      <= 0)         ||
	    (_dir == kDirEast  && _x + _w >= kScrWidth) ||
	    (_dir == kDirSouth && _y + _w >= kWorldHeight - 2))
		park();
	else {
		signed char x;            // dummy var
		_here.split(x, _z);         // take current Z position
		_snail_->addCom(kSnZTrim, -1, 0, this);    // update Hero's pos in show queue
	}
}


int Walk::distance(Sprite *spr) {
	int dx, dz;
	dx = spr->_x - (_x + _w - kWalkSide);
	if (dx < 0)
		dx = (_x + kWalkSide) - (spr->_x + spr->_w);

	if (dx < 0)
		dx = 0;

	dx /= kMapGridX;
	dz = spr->_z - _z;
	if (dz < 0)
		dz = - dz;

	dx = dx * dx + dz * dz;
	for (dz = 1; dz * dz < dx; dz++)
		;

	return dz - 1;
}


void Walk::turn(Dir d) {
	Dir dir = (_dir == kDirNone) ? kDirSouth : _dir;
	if (d != _dir) {
		step((d == dir) ? (1 + dir + dir) : (9 + 4 * dir + d));
		_dir = d;
	}
}


void Walk::park() {
	if (_time == 0)
		_time++;

	if (_dir != kDirNone) {
		step(9 + 4 * _dir + _dir);
		_dir = kDirNone;
		_tracePtr = -1;
	}
}


void Walk::findWay(Cluster c) {
	if (c != _here) {
		for (_findLevel = 1; _findLevel <= kMaxFindLevel; _findLevel++) {
			signed char x, z;
			_here.split(x, z);
			_target = Couple(x, z);
			c.split(x, z);

			if (find1Way(Cluster(x, z)))
				break;
		}
		_tracePtr = (_findLevel > kMaxFindLevel) ? -1 : (_findLevel - 1);
		if (_tracePtr < 0)
			noWay();
		_time = 1;
	}
}


void Walk::findWay(Sprite *spr) {
	if (spr && spr != this) {
		int x = spr->_x;
		int z = spr->_z;
		if (spr->_flags._east)
			x += spr->_w + _w / 2 - kWalkSide;
		else
			x -= _w / 2 - kWalkSide;
		findWay(Cluster((x / kMapGridX),
		                ((z < kMapZCnt - kDistMax) ? (z + 1)
		                 : (z - 1))));
	}
}


bool Walk::lower(Sprite *spr) {
	return (spr->_y > _y + (_h * 3) / 5);
}


void Walk::reach(Sprite *spr, int mode) {
	if (spr) {
		_hero->findWay(spr);
		if (mode < 0) {
			mode = spr->_flags._east;
			if (lower(spr))
				mode += 2;
		}
	}
	// note: insert SNAIL commands in reverse order
	_snail->insCom(kSnPause, -1, 64, NULL);
	_snail->insCom(kSnSeq, -1, kTSeq + mode, this);
	if (spr) {
		_snail->insCom(kSnWait,  -1, -1, _hero); /////--------$$$$$$$
		//SNINSERT(SNWALK, -1, -1, spr);
	}
	// sequence is not finished,
	// now it is just at sprite appear (disappear) point
}

void Walk::noWay() {
	_vm->trouble(kSeqNoWay, kNoWay);
}

bool Walk::find1Way(Cluster c) {
	Cluster start = c;
	const Cluster tab[4] = { Cluster(-1, 0), Cluster(1, 0), Cluster(0, -1), Cluster(0, 1)};
	const int tabLen = 4;

	if (c == _target)
		// Found destination
		return true;

	if (_level >= _findLevel)
		// Nesting limit
		return false;

	// Look for barriers
	if (c.chkBar())
		return false;

	if (c.cell())
		// Location is occupied
		return false;


	// Loop through each direction
	for (int i = 0; i < tabLen; i++) {
		// Reset to starting position
		c = start;

		do {
			c += tab[i];
			if (!c.isValid())
				// Break to check next direction
				break;

			// Recursively check for further paths
			++_level;
			++start.cell();
			bool foundPath = find1Way(c);
			--start.cell();
			--_level;

			if (foundPath) {
				// Set route point
				_trace[_level] = start;
				return true;
			}
		} while (!c.chkBar() && !c.cell());
	}

	return false;
}

} // End of namespace CGE