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

#include "common/scummsys.h"
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/error.h"
#include "common/EventRecorder.h"
#include "common/file.h"
#include "common/fs.h"
#include "engines/util.h"
#include "cge/cge.h"
#include "cge/vga13h.h"
#include "cge/cge_main.h"
#include "cge/talk.h"
#include "cge/text.h"
#include "cge/vol.h"
#include "cge/walk.h"

namespace CGE {

CGEEngine::CGEEngine(OSystem *syst, const ADGameDescription *gameDescription)
	: Engine(syst), _gameDescription(gameDescription), _randomSource("cge") {

	// Debug/console setup
	DebugMan.addDebugChannel(kCGEDebugBitmap, "bitmap", "CGE Bitmap debug channel");
	DebugMan.addDebugChannel(kCGEDebugFile, "file", "CGE IO debug channel");
	DebugMan.addDebugChannel(kCGEDebugEngine, "engine", "CGE Engine debug channel");

	_isDemo      = _gameDescription->flags & ADGF_DEMO;
	_startupMode = 1;
	_demoText    = kDemo;
	_oldLev      = 0;
	_pocPtr      = 0;

}

void CGEEngine::initCaveValues() {
	if (_isDemo) {
		_caveDx = 23;
		_caveDy = 29;
		_caveNx = 3;
		_caveNy = 1;
	} else {
		_caveDx = 9;
		_caveDy = 10;
		_caveNx = 8;
		_caveNy = 3;
	}
	_caveMax = _caveNx * _caveNy;

	if (_isDemo) {
		_maxCaveArr[0] = _caveMax;
		_maxCaveArr[1] = -1;
		_maxCaveArr[2] = -1;
		_maxCaveArr[3] = -1;
		_maxCaveArr[4] = -1;
	} else {
		_maxCaveArr[0] = 1;
		_maxCaveArr[1] = 8;
		_maxCaveArr[2] = 16;
		_maxCaveArr[3] = 23;
		_maxCaveArr[4] = 24;
	};

	_heroXY = (Hxy *) malloc (sizeof(Hxy) * _caveMax);
	for (int i = 0; i < _caveMax; i++) {
		_heroXY[i]._x = 0;
		_heroXY[i]._y = 0;
	}

	_barriers = (Bar *) malloc (sizeof(Bar) * (1 + _caveMax));
	for (int i = 0; i < _caveMax + 1; i++) {
		_barriers[i]._horz = 0xFF;
		_barriers[i]._vert = 0xFF;
	}
}

void CGEEngine::freeCaveValues() {
	free(_heroXY);
	free(_barriers);
}

void CGEEngine::setup() {
	debugC(1, kCGEDebugEngine, "CGEEngine::setup()");

	// Initialise fields
	_lastFrame = 0;
	_lastTick = 0;
	_hero = NULL;
	_shadow = NULL;
	_miniCave = NULL;
	_miniShp = NULL;
	_miniShpList = NULL;
	_sprite = NULL;

	// Create debugger console
	_console = new CGEConsole(this);

	// Initialise classes that have static members
	Vga::init();
	VFile::init();
	Bitmap::init();
	Talk::init();
	Cluster::init(this);

	// Initialise engine objects
	_text = new Text(this, progName(), 128);
	_vga = new Vga(M13H);
	_sys = new System(this);
	_pocLight = new PocLight(this);
	for (int i = 0; i < kPocketNX; i++)
		_pocket[i] = NULL;
	_horzLine = isDemo() ? NULL : new HorizLine(this);
	_infoLine = new InfoLine(this, kInfoW);
	_cavLight = new CavLight(this);
	_debugLine = new InfoLine(this, kScrWidth);
	_snail = new Snail(this, false);
	_snail_ = new Snail(this, true);

	_mouse = new Mouse(this);
	_keyboard = new Keyboard();
	_eventManager = new EventManager();
	_fx = new Fx(16);   // must precede SOUND!!
	_sound = new Sound(this);

	_offUseCount = atoi(_text->getText(kOffUseCount));
	_music = true;

	for (int i = 0; i < kPocketNX; i++)
		_pocref[i] = -1;
	_volume[0] = 0;
	_volume[1] = 0;

	initCaveValues();

	_maxCave    =  0;
	_dark       = false;
	_game       = false;
	_now        =  1;
	_lev        = -1;
	_recentStep = -2;

	for (int i = 0; i < 4; i++)
		_flag[i] = false;

	_mode = 0;
	_soundOk = 1;
	_sprTv = NULL;
	_gameCase2Cpt = 0;

	_startGameSlot = ConfMan.hasKey("save_slot") ? ConfMan.getInt("save_slot") : -1;
}

CGEEngine::~CGEEngine() {
	debugC(1, kCGEDebugEngine, "CGEEngine::~CGEEngine()");

	// Call classes with static members to clear them up
	Talk::deinit();
	Bitmap::deinit();
	VFile::deinit();
	Vga::deinit();
	Cluster::init(this);

	// Remove all of our debug levels here
	DebugMan.clearAllDebugChannels();

	delete _console;
	_midiPlayer.killMidi();

	// Delete engine objects
	delete _vga;
	delete _sys;
	delete _sprite;
	delete _miniCave;
	delete _shadow;
	delete _horzLine;
	delete _infoLine;
	delete _cavLight;
	delete _debugLine;
	delete _text;
	delete _pocLight;
	delete _keyboard;
	delete _mouse;
	delete _eventManager;
	delete _fx;
	delete _sound;
	delete _snail;
	delete _snail_;
	delete _hero;

	if (_miniShpList) {
		for (int i = 0; _miniShpList[i]; ++i)
			delete _miniShpList[i];
		delete[] _miniShpList;
	}

	freeCaveValues();
}

Common::Error CGEEngine::run() {
	debugC(1, kCGEDebugEngine, "CGEEngine::run()");

	// Initialize graphics using following:
	initGraphics(320, 200, false);

	// Setup necessary game objects
	setup();
	// Run the game
	cge_main();

	return Common::kNoError;
}

bool CGEEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL) ||
		(f == kSupportsLoadingDuringRuntime) ||
		(f == kSupportsSavingDuringRuntime);
}

bool CGEEngine::canLoadGameStateCurrently() {
	return (_startupMode == 0) && _mouse->_active;
}

bool CGEEngine::canSaveGameStateCurrently() {
	return (_startupMode == 0) && _mouse->_active;
}

bool CGEEngine::isDemo() const {
	return _gameDescription->flags & ADGF_DEMO;
}

} // End of namespace CGE