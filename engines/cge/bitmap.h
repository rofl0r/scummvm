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

#ifndef __CGE_BITMAP__
#define __CGE_BITMAP__

#include "cge/general.h"

namespace CGE {

#define kBmpEOI      0x0000
#define kBmpSKP      0x4000
#define kBmpREP      0x8000
#define kBmpCPY      0xC000

#define kMaxPath  128

#include "common/pack-start.h"

struct Bgr4 {
	uint16 _b : 2;
	uint16 _B : 6;
	uint16 _g : 2;
	uint16 _G : 6;
	uint16 _r : 2;
	uint16 _R : 6;
	uint16 _Z : 8;
};


struct HideDesc {
	uint16 _skip;
	uint16 _hide;
};

#include "common/pack-end.h"

class Bitmap {
	bool loadBMP(XFile *f);
	bool loadVBM(XFile *f);
public:
	static Dac *_pal;
	uint16 _w;
	uint16 _h;
	uint8 *_m;
	uint8 *_v;
	int32 _map;
	HideDesc *_b;

	Bitmap(const char *fname, bool rem);
	Bitmap(uint16 w, uint16 h, uint8 *map);
	Bitmap(uint16 w, uint16 h, uint8 fill);
	Bitmap(const Bitmap &bmp);
	~Bitmap();

	static void init();
	static void deinit();
	Bitmap *flipH();
	Bitmap *code();
	Bitmap &operator = (const Bitmap &bmp);
	void hide(int16 x, int16 y);
	void show(int16 x, int16 y);
	void xShow(int16 x, int16 y);
	bool solidAt(int16 x, int16 y);
	bool saveVBM(XFile *f);
	uint16 moveVmap(uint8 *buf);
};


typedef Bitmap *BitmapPtr;

} // End of namespace CGE

#endif