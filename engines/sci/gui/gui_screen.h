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
 * $URL$
 * $Id$
 *
 */

namespace Sci {

#define SCI_SCREEN_MAXHEIGHT 400

#define SCI_SCREEN_MASK_VISUAL   1
#define SCI_SCREEN_MASK_PRIORITY 2
#define SCI_SCREEN_MASK_CONTROL  4
#define SCI_SCREEN_MASK_ALL      SCI_SCREEN_MASK_VISUAL|SCI_SCREEN_MASK_PRIORITY|SCI_SCREEN_MASK_CONTROL
#define SCI_SCREEN_MASK_DITHERED 128

class SciGUIscreen {
public:
	SciGUIscreen(OSystem *system, EngineState *state);
	~SciGUIscreen();

	void init(void);
	byte *initScreen(uint16 pixelCount);

	void UpdateWhole();

	byte GetDrawingMask(byte color, byte prio, byte control);
	void Put_Pixel(int x, int y, byte drawMask, byte color, byte prio, byte control);
	byte Get_Visual(int x, int y);
	byte Get_Priority(int x, int y);
	byte Get_Control(int x, int y);
	byte IsFillMatch(int16 x, int16 y, byte drawMask, byte t_color, byte t_pri, byte t_con);

	int BitsGetDataSize(Common::Rect rect, byte mask);
	void BitsSave(Common::Rect rect, byte mask, byte *memoryPtr);
	void BitsRestore(byte *memoryPtr);

	sciPalette _sysPalette;

	uint16 _width;
	uint16 _height;
	uint _pixels;
	uint16 _displayWidth;
	uint16 _displayHeight;
	uint _displayPixels;
	byte _bytesPerDisplayPixel;

private:
	void BitsRestoreScreen(Common::Rect rect, byte *&memoryPtr, byte *screen);
	void BitsSaveScreen(Common::Rect rect, byte *screen, byte *&memoryPtr);

	OSystem *_system;
	EngineState *_s;

	uint16 _baseTable[SCI_SCREEN_MAXHEIGHT];
	uint16 _baseDisplayTable[SCI_SCREEN_MAXHEIGHT];

	// these screens have the real resolution of the game engine (320x200 for SCI0/SCI1/SCI11 games, 640x480 for SCI2 games)
	//  SCI0 games will be dithered in here at any time
	byte *_visualScreen;
	byte *_priorityScreen;
	byte *_controlScreen;

	// this screen is the one that is actually displayed to the user. It may be 640x480 for japanese SCI1 games
	//  SCI0 games may be undithered in here. Only read from this buffer for Save/ShowBits usage.
	byte *_displayScreen;
};

} // end of namespace Sci
