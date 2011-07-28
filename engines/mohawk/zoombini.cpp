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

class Zoombini_Title : public Zoombini_Module {
public:
	Zoombini_Title(MohawkEngine_Zoombini *vm) : Zoombini_Module(vm) { }
	void init();
	void shutdown();
	void update();
	void keyDown(uint key);

protected:
	bool _running;
};

void Zoombini_Title::init() {
	_running = false;
}

void Zoombini_Title::shutdown() {
	// TODO: force-stop movie?
	_vm->snoidFadeOut();
	_vm->setCursor(kWatchCursor);

	_vm->getSnoidParts();
	// TODO: _vm->initOptions()
	_vm->_cursor->showCursor();
}

void Zoombini_Title::update() {
	if (!_running) {
		_running = true;
		_vm->_sound->stopSound(99);
		_vm->_wasInTitle = true;
		// TODO: play in background
		_vm->_video->playMovieBlocking("logo025.mov");
		_vm->_cursor->hideCursor();
	} else {
		// movie is done
		_vm->_newLeg = 3; // TODO
		shutdown();
	}
}

void Zoombini_Title::keyDown(uint key) {
	if (key == 0x11) {
		// TODO: control key calls shutdown() then sets shouldQuit?
	} else {
		// TODO: keypress sets global title status to 2, sets movie to done?
	}
}

class Zoombini_PickerScreen : public Zoombini_Module {
public:
	Zoombini_PickerScreen(MohawkEngine_Zoombini *vm);
	void init();
	void shutdown();
	void update();
	void keyDown(uint key);

	void pickerHotspotProc(uint hotspot);
	void moduleHotspotProc(uint hotspot);
	void drawBttnProc(OldFeature *feature);
	void moveBttnProc(OldFeature *feature);

protected:
	void initPicker();
	bool checkModuleToGoto();

	void drawPickerButton(bool redraw, bool pressed, uint button);
	void drawFeatureButton(bool pressed, uint button);
	void drawBigSnoidPart(uint part);
	void drawBigSnoid();

	bool isCurrentSnoidComplete();
	void makeRandomSnoid(bool force);
	void markSnoidAsMade(bool add);

	void togglePickerWaves(bool onlyUpdate);
	void pickerCrowdControl(uint &nextSpareIndex, Common::Point &nextSparePoint);
	void pickerCrowdControl();
	void installBoatAndWaves();
	bool canDoFeature(uint partId, uint partType);
	void pickerPartyOK();

	bool _pickerScreenActive;
	uint16 _currSound;

	uint _neededSnoids;
	SnoidStruct _snoidStruct;
	bool _addedSnoid, _snoidComplete, _partyOK;

	Feature *_boat, *_waves, *_rocks;

	Common::Array<Common::Rect> _featureHotspots, _moduleHotspots;
	Common::Array<Common::Point> _restAreas;
};

Zoombini_PickerScreen::Zoombini_PickerScreen(MohawkEngine_Zoombini *vm) : Zoombini_Module(vm) {
	// hard-coded data tables
	_featureHotspots.resize(20);
	for (uint i = 0; i < 20; i++) {
		_featureHotspots[i].left = 3 + 39 * (i % 5);
		_featureHotspots[i].top = 304 + 44 * (i / 5);
		_featureHotspots[i].setWidth(39);
		_featureHotspots[i].setHeight(42);
	}
	_moduleHotspots.push_back(Common::Rect(205, 304, 260, 342));
	_moduleHotspots.push_back(Common::Rect(205, 347, 273, 420));
	_moduleHotspots.push_back(Common::Rect(201, 419, 287, 437));
	_moduleHotspots.push_back(Common::Rect(205, 440, 260, 478));
	_moduleHotspots.push_back(Common::Rect(600, 403, 639, 440));
	_moduleHotspots.push_back(Common::Rect(600, 441, 639, 478));
	_moduleHotspots.push_back(Common::Rect(600, 365, 639, 402));
	_moduleHotspots.push_back(Common::Rect(0, 0, 640, 480));
	_restAreas.push_back(Common::Point(542, 446));
	_restAreas.push_back(Common::Point(505, 447));
	_restAreas.push_back(Common::Point(466, 451));
	_restAreas.push_back(Common::Point(425, 448));
	_restAreas.push_back(Common::Point(380, 450));
	_restAreas.push_back(Common::Point(342, 451));
	_restAreas.push_back(Common::Point(522, 402));
	_restAreas.push_back(Common::Point(488, 408));
	_restAreas.push_back(Common::Point(444, 416));
	_restAreas.push_back(Common::Point(403, 413));
	_restAreas.push_back(Common::Point(364, 413));
	_restAreas.push_back(Common::Point(498, 360));
	_restAreas.push_back(Common::Point(463, 367));
	_restAreas.push_back(Common::Point(426, 370));
	_restAreas.push_back(Common::Point(389, 373));
	_restAreas.push_back(Common::Point(352, 374));
}

void Zoombini_PickerScreen::initPicker() {
	// TODO: can't work out where this is set in the original. nowhere?!
	_snoidStruct._snoidData.unknown245 = 0;

	_vm->_dropSpotRange = 60;
	_snoidComplete = false;
	_partyOK = false; // Not set in original.
	_vm->_tmpNextLeg = 0;
	_currSound = 0;
	for (uint i = 0; i < 4; i++)
		_snoidStruct._snoidData.part[i] = 0;
	_snoidStruct._snoidData.name = NULL;
	//_vm->randomName(_snoidStruct._snoidData.name);
	for (uint i = 0; i < 16; i++)
		_vm->_sortedSnoids[i] = 0;
	_addedSnoid = false;
	_vm->_numMovingSnoids = 0;
	_vm->_numIdleSnoids = 0;
}

void Zoombini_PickerScreen::init() {
	_pickerScreenActive = false;

	initPicker();

	//_vm->setSoundRangePriority(1, 29999, 20000);
	//_vm->setSoundRangePriority(0, 1007, 1000);

	_vm->snoidDirectionAfterFall(0);

	_neededSnoids = 16;
	_snoidComplete = isCurrentSnoidComplete();

	_vm->loadResourceFile("picker");
	_vm->installNodesAndPaths(1000);
	_vm->installBG(4000);

	// TODO: preload compound shapes 4200, 4300, 4400
	_vm->installFeatureShapes(false, 0, 4100);
	_vm->installGroupOfSCRBs(true, 4100, 11);

	_vm->copyFadeColors(10, 236);

	// TODO: preload sounds 1000, 1004, 1005, 1006

	// twinkle twinkle little stars
	for (uint n = 4101; n <= 4103; n++)
		_vm->installViewFeature(0, 0, NULL, n - 4091, n, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x8000);
	installBoatAndWaves();
	// rocks above drop spot
	_rocks = _vm->installViewFeature(0, 0, NULL, 0, 4100, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x64000000);
	// drop spot
	static const Common::Point dropSpot(172, 226);
	_vm->installViewFeature(0, 0, (void *)&dropSpot, 6, 4110, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x108A000);
	// random scenery objects?
	for (uint n = 4106; n <= 4109; n++)
		_vm->installViewFeature(0, 0, NULL, 0, n, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0);
	// picker buttons
	_vm->installViewFeature(0, 0, NULL, 0, 0, (FeatureProc)&Zoombini_PickerScreen::moveBttnProc, (FeatureProc)&Zoombini_PickerScreen::drawBttnProc, 0x4001000);

	_vm->setViewRestAreas(true, _restAreas);

	// TODO: snoid loading?
	//_vm->installSnoids(false);
	// TODO: snoid setup: sort snoids into the right order based on the rest areas
	_vm->setSnoidsInPartyStatus(false, 1);
	_vm->idleView();
	togglePickerWaves(true);
	pickerPartyOK();

	_vm->addHotspots(&_featureHotspots, (HotspotProc)&Zoombini_PickerScreen::pickerHotspotProc);
	_vm->addHotspots(&_moduleHotspots, (HotspotProc)&Zoombini_PickerScreen::moduleHotspotProc);

	_snoidStruct._snoidData.inPartyStatus = 1;
	// This is calculated in the original by adding 39/31 to the relevant hotspot.
	_snoidStruct._data.currentPos = Common::Point(205 + 39, 347 + 31);

	drawPickerButton(false, false, 0);
	drawFeatureButton(false, 0);
	if (_vm->_wasInTitle) {
		// TODO: language handling?
		_vm->idleView();
	}
	_vm->_needsUpdate = true; // TODO: thought
	_vm->snoidFadeIn();
	_pickerScreenActive = true;

	_vm->queueSound(30001);

	if (_vm->_previousLeg == 1) {
		uint count = _vm->numSnoidsInModule();
		if (625 - (_vm->_state.unknown74 + _vm->_state.unknown76 + _vm->_state.unknown78) > (int)count) {
			if (count < 625) {
				uint rand = _vm->_rnd->getRandomNumberRng(1, 20);
				if (rand == 1)
					_currSound = 20043; // "Quickly, help them begin their journey!"
				else if (rand == 10)
					_currSound = 20044; // "The adventure begins here! Free the Zoombinis!"
			}
		}
	} else {
		//_vm->incrementBeenToModule(snoidut + 40);
		if (_vm->_state.totalSnoidCount < 625) {
			if (_vm->_wasInTitle)
				_currSound = 20042; // "Now perhaps you're wondering what all this has to do with you..."
		}
	}
	if (_currSound)
		_vm->queueSound(_currSound);
	_vm->_wasInTitle = false;
}

void Zoombini_PickerScreen::shutdown() {
	if (!_pickerScreenActive)
		return;
	_pickerScreenActive = false;

	_vm->freeViewFeatures();
	// FIXME: snoid maintenance
	_vm->_sound->stopSound();
	_vm->_sound->stopMidi();
	_vm->unloadResourceFile();
	_vm->snoidFadeOut();
	_vm->setCursor(kWatchCursor);
}

void Zoombini_PickerScreen::update() {
	static bool updateRunning = false;

	if (updateRunning || !_pickerScreenActive)
		return;

	updateRunning = true;
	_vm->idleView();
	if (_vm->_tmpNextLeg) {
		if (_vm->_sound->isPlaying(996)) {
			updateRunning = false;
			return;
		}
		if (_vm->_haveNewGame || !_vm->_numMovingSnoids || _vm->_numIdleSnoids >= 1)
			checkModuleToGoto();
	}
	if (!_vm->_numMovingSnoids)
		_addedSnoid = false;
	updateRunning = false;
}

void Zoombini_PickerScreen::keyDown(uint key) {
	// TODO
}

void Zoombini_PickerScreen::pickerHotspotProc(uint hotspot) {
	if (checkModuleToGoto())
		return;

	if (_currSound) {
		if (_vm->_sound->isPlaying(_currSound)) {
			_vm->_sound->stopSound(_currSound);
		}
		_currSound = 0;
	}

	// Original uses a loop here O.o.
	hotspot--;
	uint partId = hotspot % 5;
	uint partType = hotspot / 5;
	byte part = _snoidStruct._snoidData.part[partType];

	if (part && part == partId + 1) {
		_vm->queueSound(1004);
		drawFeatureButton(false, 5 * partType + partId);
		_vm->_needsUpdate = true; // TODO: thought
		_snoidStruct._snoidData.part[partType] = 0;
	} else if (!canDoFeature(partId, partType)) {
		_vm->queueSound(1008);
	} else {
		if (part) {
			drawFeatureButton(false, 5 * partType + part);
		}
		_vm->queueSound(1000);
		drawFeatureButton(false, 5 * partType + partId);
		_vm->_needsUpdate = true; // TODO: thought
		_snoidStruct._snoidData.part[partType] = partId + 1;
	}
	bool complete = isCurrentSnoidComplete();
	if (complete != _snoidComplete) {
		_snoidComplete = complete;
		drawPickerButton(true, true, 3);
	}
}

bool Zoombini_PickerScreen::checkModuleToGoto() {
	if (!_vm->_tmpNextLeg)
		return false;
	_vm->_newLeg = _vm->_tmpNextLeg;
	_vm->_tmpNextLeg = 0;
	shutdown();
	return true;
}

void Zoombini_PickerScreen::moduleHotspotProc(uint hotspot) {
	if (checkModuleToGoto())
		return;

	if (_currSound) {
		if (_vm->_sound->isPlaying(_currSound)) {
			_vm->_sound->stopSound(_currSound);
		}
		_currSound = 0;
	}

	uint index;
	Common::Point point;
	switch (hotspot) {
	case 1:
		// Add the created snoid to the party.
		if (!_partyOK && _snoidComplete /* && not 625 snoids already */) {
			// TODO: stuff here
			_addedSnoid = true;
			_vm->_numMovingSnoids++;
			_vm->queueSound(1005);
			drawPickerButton(true, true, hotspot);
			// TODO: wait for 2 ticks?
			markSnoidAsMade(true);
			pickerCrowdControl(index, point);
			_vm->_sortedSnoids[index] = _vm->addSnoidToScreen(point, Common::Point(148, 215), 0, &_snoidStruct);
			_vm->_state.totalSnoidCount++;
			drawPickerButton(true, false, hotspot);
			_vm->sortView();
			pickerPartyOK();
			// TODO: random name(10)
			drawPickerButton(true, true, 3);
		}
		_snoidComplete = isCurrentSnoidComplete();
		break;
	case 2:
		// Clicking on the snoid preview.
		uint sound, soundId;
		sound = _vm->_rnd->getRandomNumberRng(0, 12);
		soundId = _vm->getSnoidSoundId(sound, &_snoidStruct._snoidData);
		_vm->queueSound(soundId);
		break;
	case 3:
		// Clicking on the nametag.
		if (_snoidComplete) {
			_vm->queueSound(1000);
			// TODO: random name(10)
			drawPickerButton(true, true, hotspot);
		}
		break;
	case 4:
		// Random snoid.
		if (false /* already have 625 snoids */) {
			_vm->queueSound(1008); // "No."
		} else {
			_vm->queueSound(1006);
			drawPickerButton(true, true, hotspot);
			if (_partyOK || !(_vm->getEventManager()->getModifierState() & Common::KBD_SHIFT)) {
				// TODO: wait for 2 ticks?
				drawPickerButton(true, false, hotspot);
				makeRandomSnoid(_snoidComplete);
				drawFeatureButton(false, 0);
				_vm->_needsUpdate = true; // TODO: thought
				drawPickerButton(true, true, 3);
			} else {
				makeRandomSnoid(true);
				drawFeatureButton(false, 0);
				_vm->_needsUpdate = true; // TODO: thought
				pickerCrowdControl(index, point);
				uint32 lastTime = _vm->getTime();
				while (!_partyOK) {
					markSnoidAsMade(true);
					_vm->_sortedSnoids[index] = _vm->addSnoidToScreen(point, Common::Point(148, 215), lastTime, &_snoidStruct);
					_addedSnoid = true;
					_vm->_numMovingSnoids++;
					_vm->_state.totalSnoidCount++;
					if (_vm->_state.unknown16)
						lastTime += _vm->_rnd->getRandomNumberRng(120, 180);
					else
						lastTime += _vm->_rnd->getRandomNumberRng(60, 120);
					pickerPartyOK();
					makeRandomSnoid(true);
					pickerCrowdControl(index, point);
				}
				drawPickerButton(true, true, 3);
				drawFeatureButton(false, 0);
				_vm->_needsUpdate = true; // TODO: thought
				drawPickerButton(true, false, hotspot);
			}
			_snoidComplete = isCurrentSnoidComplete();
		}
		break;
	case 5:
		// Map screen?
		_vm->queueSound(999);
		drawPickerButton(true, true, hotspot);
		// TODO: wait for 2 ticks?
		drawPickerButton(true, false, hotspot);
		// TODO: move to leg 1
		// TODO: purge picker screen
		break;
	case 6:
		// Go forth!
		if (_partyOK) {
			_vm->queueSound(996);
			drawPickerButton(true, true, hotspot);
			// TODO: wait for 2 ticks?
			drawPickerButton(true, false, hotspot);
			_vm->_numMovingSnoids = 0;
			_vm->_numIdleSnoids = 0;
			for (uint i = 0; i < 4; i++) {
				uint16 snoidId = _vm->_sortedSnoids[i + 11];
				if (!snoidId)
					continue;
				SnoidFeature *snoid = _vm->getSnoidPtrFromId(true, snoidId);
				assert(snoid);
				if (snoid->_snoidData.mode)
					continue;
				// Since _numMovingSnoids is always zero here, this seems a bit silly..
				snoid->_nextTime = _vm->getTime() + 60 * _vm->_numMovingSnoids;
				snoid->_snoidData.currentNode = 0xff;
				snoid->_snoidData.dest = Common::Point(544, 264);
				snoid->setNewSnoidModeAndXY(Common::Point(), 7);
				_vm->_numMovingSnoids++;
				break;
			}
			_vm->_tmpNextLeg = 7;
		} else {
			// TODO: this code is v.similar to that in the init
			uint count = _vm->numSnoidsInModule();
			if (625 - (_vm->_state.unknown74 + _vm->_state.unknown76 + _vm->_state.unknown78) > (int)count) {
				if (count < 625) {
					uint rand = _vm->_rnd->getRandomNumberRng(1, 2);
					if (rand == 1)
						_currSound = 20043; // "Quickly, help them begin their journey!"
					else if (rand == 2)
						_currSound = 20044; // "The adventure begins here! Free the Zoombinis!"
				}
				if (_currSound)
					_vm->queueSound(_currSound);
			}
		}
		break;
	case 7:
		_vm->queueSound(999);
		drawPickerButton(true, true, hotspot);
		// TODO: wait for 2 ticks?
		drawPickerButton(true, false, hotspot);
		// TODO: help stuff which isn't in 1.0?
		break;
	case 8:
		// This rect covers the whole screen, for handling snoid dragging.
		if (_addedSnoid)
			return;

		// See if we clicked on a snoid..
		Common::Point mousePos = _vm->getEventManager()->getMousePos();
		SnoidFeature *feature = (SnoidFeature *)_vm->pointOnFeature(true, 1, mousePos);
		if (!feature)
			break;
		// Can't pick up snoids unless they're idle.
		if (feature->_snoidData.mode != 0 && feature->_snoidData.mode != 4 && feature->_snoidData.mode != 6)
			break;

		// Push the rocks behind everything else.
		_rocks->_flags |= kFeatureSortBackground;

		_vm->dragSnoid(feature, mousePos);
		if (_vm->snoidOnThisDropSpot()) {
			_vm->removeFeature(feature, false);
			for (uint i = 0; i < 16; i++) {
				if (_vm->_sortedSnoids[i] != feature->_id)
					continue;
				_vm->_sortedSnoids[i] = 0;
				break;
			}

			// TODO: change global
			_vm->markNthDropSpot(0, 1);
			_vm->queueSound(1007);
			if (_vm->_state.totalSnoidCount > 0)
				_vm->_state.totalSnoidCount--;
			// FIXME: copy parts back
			// FIXME: copy 10 bytes from name (249)?!
			markSnoidAsMade(false);
			// FIXME: XXX
			pickerPartyOK();
			_snoidComplete = isCurrentSnoidComplete();
			pickerCrowdControl();
			// FIXME: XXX
			delete feature; // TODO: original leaks the snoid?
		}

		// Restore the rocks to their normal z-order.
		_rocks->_flags &= ~kFeatureSortBackground;
		break;
	}
}

void Zoombini_PickerScreen::drawBttnProc(OldFeature *feature) {
	drawPickerButton(false, false, 0);
	drawFeatureButton(false, 0);
}

void Zoombini_PickerScreen::moveBttnProc(OldFeature *feature) {
	// The original updates dirty rectangles here as states change (and
	// checks for shift key presses for the check in drawPickerButton).
}

void Zoombini_PickerScreen::drawPickerButton(bool redraw, bool pressed, uint button) {
	uint start = 0, end = 7; // original uses 8, but #7 isn't used
	if (button) {
		if (button > 7)
			error("drawPickerButton got invalid button %d", button);
		start = button - 1;
		end = button;
	}

	for (uint i = start; i < end; i++) {
		Common::Rect &buttonRect = _moduleHotspots[i];

		uint bitmapId = 0;
		switch (i) {
		case 0:
			bitmapId = 2;
			if (/* already have 625 snoids || */ _partyOK || !_snoidComplete) {
				pressed = false;
				bitmapId = 1;
			}
			break;
		case 1:
			drawBigSnoid();
			break;
		case 2:
			_vm->_gfx->copyAnimSubImageToScreen(4200, 13 - 1, buttonRect.left, buttonRect.top);
			// TODO: draw name text if necessary?
			break;
		case 3:
			bitmapId = 4;
			if (!_partyOK && (_vm->getEventManager()->getModifierState() & Common::KBD_SHIFT) && true /* TODO: if we don't have >=625 snoids in total */)
				bitmapId = 6;
			break;
		case 4:
			bitmapId = 11;
			break;
		case 5:
			bitmapId = 9;
			if (!_partyOK) {
				pressed = false;
				bitmapId = 8;
			}
			break;
		case 6:
			// TODO: The help button isn't in 1.0.
			//bitmapId = 24;
			break;
		}

		if (bitmapId) {
			if (pressed)
				bitmapId++;

			uint16 compoundSHAPId;
			if (bitmapId < 24)
				compoundSHAPId = 4200;
			else
				compoundSHAPId = 1;
			_vm->_gfx->copyAnimSubImageToScreen(compoundSHAPId, bitmapId - 1, buttonRect.left, buttonRect.top);
		}
	}

	if (redraw)
		_vm->_needsUpdate = true; // TODO: thought
}

void Zoombini_PickerScreen::drawBigSnoidPart(uint part) {
	if (!part)
		return;

	Common::Point partPos = _snoidStruct._data.currentPos;
	static const int xOffsets[22] = { 0, 22, 25, 28, 19, 10, 29, 15, 7, 17, 24, 24, 7, 7, 7, 7, 7, 23, 24, 13, 15, 23 };
	static const int yOffsets[22] = { 0, 23, 30, 29, 27, 31, 30, 11, 11, 11, 9, 6, -2, -2, -2, -2, -2, -22, -22, -20, -19, -23 };
	partPos.x -= xOffsets[part];
	partPos.y -= yOffsets[part];
	_vm->_gfx->copyAnimSubImageToScreen(4300, part - 1, partPos.x, partPos.y);
}

void Zoombini_PickerScreen::drawBigSnoid() {
	byte *part = _snoidStruct._snoidData.part;
	if (part[kSnoidPartFeet])
		drawBigSnoidPart(part[kSnoidPartFeet] + 16);
	drawBigSnoidPart(1);
	if (part[kSnoidPartEyes])
		drawBigSnoidPart(part[kSnoidPartEyes] + 6);
	if (part[kSnoidPartNose])
		drawBigSnoidPart(part[kSnoidPartNose] + 11);
	if (part[kSnoidPartHair])
		drawBigSnoidPart(part[kSnoidPartHair] + 1);
}

bool Zoombini_PickerScreen::isCurrentSnoidComplete() {
	for (uint i = 0; i < 4; i++) {
		if (_snoidStruct._snoidData.part[i] > 5)
			_snoidStruct._snoidData.part[i] = 0;
		if (!_snoidStruct._snoidData.part[i])
			return false;
	}

	// TODO: check whether there are less than 2 existing snoids with these parts
	return true;
}

void Zoombini_PickerScreen::makeRandomSnoid(bool force) {
	_snoidComplete = false;
	for (uint i = 0; i < 64; i++) {
		for (uint n = 0; n < 4; n++) {
			if (force || !_snoidStruct._snoidData.part[n])
				_snoidStruct._snoidData.part[n] = _vm->_rnd->getRandomNumberRng(1, 5);
		}
		_snoidComplete = isCurrentSnoidComplete();
		if (_snoidComplete)
			break;
		force = true;
	}
	if (!_snoidComplete) {
		// TODO: try all combinations in order, pick whichever is valid
	}
	if (force)
		; // TODO: randomName(10);
}

void Zoombini_PickerScreen::markSnoidAsMade(bool add) {
	if (add) {
		_snoidComplete = isCurrentSnoidComplete();
	} else {
		// We don't want to check whether we have too many.
		_snoidComplete = true;
		for (uint n = 0; n < 4; n++)
			if (!_snoidStruct._snoidData.part[n])
				_snoidComplete = false;
	}

	if (!_snoidComplete)
		return;

	if (add) {
		// The original does another check for whether there are less than 2 existing
		// snoids here, which is a bit silly given isCurrentSnoidComplete() does it.
		// TODO: increase count
	} else {
		// TODO: decrease count
	}
}

void Zoombini_PickerScreen::drawFeatureButton(bool pressed, uint button) {
	uint start = 0, end = 20;
	if (button) {
		if (button > 20)
			error("drawFeatureButton got invalid button %d", button);
		start = button - 1;
		end = button;
	}

	for (uint i = start; i < end; i++) {
		uint bitmapId = (2 * i) + 1;
		uint part = i % 5; // column
		uint partType = i / 5; // row

		// If this is the selected part, show the selected button.
		if (part + 1 == _snoidStruct._snoidData.part[partType])
			bitmapId++;

		_vm->_gfx->copyAnimSubImageToScreen(4400, bitmapId - 1, _featureHotspots[i].left, _featureHotspots[i].top);
	}
}

void Zoombini_PickerScreen::togglePickerWaves(bool onlyUpdate) {
	// TODO: this is some global state? and onlyUpdate is for debugging?
	uint16 mode = 0;
	if (!onlyUpdate)
		mode++;
	if (mode > 3)
		mode = 0;

	uint16 enableWaves, enableBoat;
	switch (mode) {
	case 0:
		enableWaves = 1;
		enableBoat = 1;
		break;
	case 1:
		enableWaves = 0;
		enableBoat = 0;
		break;
	case 2:
		enableWaves = 1;
		enableBoat = 0;
		break;
	case 3:
		enableWaves = 0;
		enableBoat = 1;
		break;
	}

	_waves->_data.enabled = enableWaves;
	_waves->_data.syncChannel = 0;
	_boat->_data.enabled = enableBoat;
	_boat->_data.syncChannel = 0;
}

void Zoombini_PickerScreen::pickerCrowdControl(uint &nextSpareIndex, Common::Point &nextSparePoint) {
	uint i = 0;
	while (_vm->_sortedSnoids[i]) {
		i++;
		if (i == 16) {
			nextSpareIndex = 0xffffffff;
			return;
		}
	}
	nextSpareIndex = i;
	nextSparePoint = _restAreas[i];
}

void Zoombini_PickerScreen::pickerCrowdControl() {
	_vm->_snoidPathUpdatesDisabled = true;

	for (uint i = 0; i < 15; i++) {
		if (_vm->_sortedSnoids[i])
			continue;

		uint var0 = 0, var1 = 0, var2 = 0, var3 = 0, var4 = 0;
		switch (i) {
		case 0:
			var4 = 5;
		case 1:
		case 6:
		case 11:
			var3 = 4;
		case 2:
		case 7:
		case 12:
			var2 = 3;
		case 3:
		case 8:
		case 13:
			var1 = 2;
		case 4:
		case 9:
		case 14:
			var0 = 1;
			break;
		}

		while (var0) {
			uint snoidId = i + var0;
			if (_vm->_sortedSnoids[snoidId]) {
				SnoidFeature *snoid = _vm->getSnoidPtrFromId(true, _vm->_sortedSnoids[snoidId]);
				if (snoid) {
					snoid->_snoidData.currentNode = 0xff;
					if (snoidId >= 17) {
						snoid->_snoidData.dest = Common::Point(326, 390);
						snoid->setNewSnoidModeAndXY(Common::Point(), 7);
						snoidMoveProc(snoid);
						snoid->_snoidData.dest = _restAreas[i];
					} else {
						snoid->_snoidData.dest = _restAreas[i];
						snoid->setNewSnoidModeAndXY(Common::Point(), 7);
					}
					_vm->_sortedSnoids[i] = _vm->_sortedSnoids[snoidId];
					_vm->_sortedSnoids[snoidId] = 0;
					break;
				}
			}
			var0 = var1;
			var1 = var2;
			var2 = var3;
			var3 = var4;
			var4 = 0;
		}
	}
	_vm->idleView();
	_vm->_snoidPathUpdatesDisabled = false;
}

void Zoombini_PickerScreen::installBoatAndWaves() {
	_waves = _vm->installViewFeature(0, 0, NULL, 7, 4104, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x800c000);
	_boat = _vm->installViewFeature(0, 0, NULL, 9, 4105, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x8008000);
}

bool Zoombini_PickerScreen::canDoFeature(uint partId, uint partType) {
	return true;
}

void Zoombini_PickerScreen::pickerPartyOK() {
	uint snoidsInParty = _vm->numSnoidsInParty();
	_partyOK = (snoidsInParty && (snoidsInParty >= _neededSnoids /* || we already have >= 625 snoids total */));
}

class Zoombini_Bridge : public Zoombini_Module {
public:
	Zoombini_Bridge(MohawkEngine_Zoombini *vm);
	void init();
	void shutdown();
	void update();
	void keyDown(uint key);

	void hotspotProc(uint hotspot);
	void drawBttnProc(OldFeature *feature);
	void moveBttnProc(OldFeature *feature);

protected:
	void drawButton(bool redraw, bool pressed, uint button);

	uint _snoidsOver;
	uint _pegsGone;

	Common::Array<Common::Rect> _hotspots;
	Common::Array<Common::Point> _restAreas;
};

Zoombini_Bridge::Zoombini_Bridge(MohawkEngine_Zoombini *vm) : Zoombini_Module(vm) {
	_hotspots.push_back(Common::Rect(600, 403, 639, 440));
	_hotspots.push_back(Common::Rect(600, 441, 639, 478));
	_hotspots.push_back(Common::Rect(600, 365, 639, 402));
	_hotspots.push_back(Common::Rect(0, 0, 640, 480));
	_restAreas.push_back(Common::Point(176, 304));
	_restAreas.push_back(Common::Point(169, 327));
	_restAreas.push_back(Common::Point(144, 283));
	_restAreas.push_back(Common::Point(147, 355));
	_restAreas.push_back(Common::Point(124, 318));
	_restAreas.push_back(Common::Point(119, 379));
	_restAreas.push_back(Common::Point(108, 284));
	_restAreas.push_back(Common::Point(99, 345));
	_restAreas.push_back(Common::Point(88, 414));
	_restAreas.push_back(Common::Point(69, 262));
	_restAreas.push_back(Common::Point(79, 303));
	_restAreas.push_back(Common::Point(78, 370));
	_restAreas.push_back(Common::Point(61, 346));
	_restAreas.push_back(Common::Point(45, 301));
	_restAreas.push_back(Common::Point(36, 359));
	_restAreas.push_back(Common::Point(30, 404));
}

void Zoombini_Bridge::init() {
	// initBridge
	// FIXME
	_vm->_dropSpotRange = 55;
	// FIXME
	_vm->_tmpNextLeg = 0;
	// FIXME
	_vm->_numIdleSnoids = 0;
	_vm->_numMovingSnoids = 0;
	// FIXME

	_snoidsOver = 0;
	// FIXME
	_vm->loadResourceFile("bridge");
	_vm->installTerrain(1600);
	_vm->installBG(1000);
	_vm->installFeatureShapes(false, 0, 1100);
	_vm->installFeatureShapes(false, 1, 1200);
	_vm->installFeatureShapes(false, 2, 1300);
	_vm->installGroupOfSCRBs(true, 1100, 7);
	_vm->installGroupOfSCRBs(false, 1200, 10, 49);
	_vm->installGroupOfSCRBs(false, 1200, 0, 2);
	_vm->initRejectScripts(20, 20, 1000);
	_vm->initNormalScripts(5, 25, 2000);

	// Install the two bridge hotspots.
	static const Common::Point dropSpot1(104, 116);
	static const Common::Point dropSpot2(203, 128);
	_vm->installViewFeature(0, 0, (void *)&dropSpot1, 7, 1300, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x108A000);
	_vm->installViewFeature(0, 0, (void *)&dropSpot2, 7, 1301, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x108A000);
	_vm->installViewFeature(0, 0, NULL, 6, 1105, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x91C8000);

	_pegsGone = 0;
	// Pegs.
	_vm->installViewFeature(0, 0, NULL, 6, 1202 + _pegsGone, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x8108000);
	// North bridge.
	_vm->installViewFeature(0, 0, NULL, 6, 1201, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x8108000);
	// South bridge.
	_vm->installViewFeature(0, 0, NULL, 6, 1200, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x8108000);

	// Scenery.
	for (uint i = 1100; i < 1103; i++)
		_vm->installViewFeature(0, 0, NULL, 0, i, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0);
	_vm->installViewFeature(0, 0, NULL, 0, 1103, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x100000);
	for (uint i = 1104; i < 1106; i++)
		_vm->installViewFeature(0, 0, NULL, 0, i, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0);
	_vm->installViewFeature(0, 0, NULL, 0, 1106, (FeatureProc)&Zoombini_Module::defaultMoveProc, (FeatureProc)&Zoombini_Module::defaultDrawProc, 0x8000);

	// UI buttons.
	_vm->installViewFeature(0, 0, NULL, 0, 0, (FeatureProc)&Zoombini_Bridge::moveBttnProc, (FeatureProc)&Zoombini_Bridge::drawBttnProc, 0x1000);

	_vm->setViewRestAreas(true, _restAreas);
	// _vm->installSnoids(false);
	// _vm->prepSnoidsToWalkToRestPts(0);
	_vm->idleView();
	// _vm->adjustSnoidWalkTimes(0, 45);
	// setupBridgePuzzle();
	_vm->addHotspots(&_hotspots, (HotspotProc)&Zoombini_Bridge::hotspotProc);
	// FIXME
	_vm->snoidFadeIn();
	_vm->setSnoidsInPartyStatus(false, 0);
	_vm->setTimeOfLastUserAction();
	// FIXME
	_vm->queueSound(997);
	// FIXME
}

void Zoombini_Bridge::shutdown() {
	// FIXME
}

void Zoombini_Bridge::update() {
	// FIXME
	_vm->idleView();
}

void Zoombini_Bridge::keyDown(uint key) {
	// FIXME
}

void Zoombini_Bridge::hotspotProc(uint hotspot) {
	// FIXME
}

void Zoombini_Bridge::drawBttnProc(OldFeature *feature) {
	drawButton(false, false, 1);
	drawButton(false, false, 2);
	//drawButton(false, false, 3); FIXME: help button is 1.1+
}

void Zoombini_Bridge::moveBttnProc(OldFeature *feature) {
	// The original updates dirty rectangles here as states change.
}

void Zoombini_Bridge::drawButton(bool redraw, bool pressed, uint button) {
	if (--button > 2)
		error("Zoombini_Bridge::drawButton can't draw button %d", button + 1);
	uint16 compoundSHAPId = 1400;
	uint16 bitmapId;
	switch (button) {
	case 0:
		bitmapId = 5;
		break;
	case 1:
		bitmapId = 2;
		if (!_snoidsOver) {
			pressed = false;
			bitmapId = 1;
		}
		break;
	case 2:
		bitmapId = 24;
		compoundSHAPId = 1;
		break;
	}

	if (pressed)
		bitmapId++;

	_vm->_gfx->copyAnimSubImageToScreen(compoundSHAPId, bitmapId - 1, _hotspots[button].left, _hotspots[button].top);

	if (redraw)
		_vm->_needsUpdate = true; // TODO: thought
}

MohawkEngine_Zoombini::MohawkEngine_Zoombini(OSystem *syst, const MohawkGameDescription *gamedesc) : MohawkEngine(syst, gamedesc), View(this) {
	_rnd = new Common::RandomSource("zoombini");
	g_eventRec.registerRandomSource(*_rnd, "zoombini");

	_tmpNextLeg = 0;
	_previousLeg = 0xffff;
	_currentLeg = 0xffff;
	_newLeg = 0;

	_inDialog = false; // TODO: where is this set?
	_dragOneTime = 0; // TODO: where is this set?

	_idleWaitTime = 64;

	_currentCursor = 0;
}

MohawkEngine_Zoombini::~MohawkEngine_Zoombini() {
	delete _console;
	delete _gfx;
	delete _rnd;
}

Common::Error MohawkEngine_Zoombini::run() {
	MohawkEngine::run();

	_console = new ZoombiniConsole(this);
	_gfx = new ZoombiniGraphics(this);
	setGraphicsManager(_gfx); // TODO
	_cursor = new DefaultCursorManager(this, ID_CURS);

	loadResourceFile("zoombini");

	// TODO: should be in reset func
	memset(&_state, 0, sizeof(_state));
	_hideCursor = true;
	_stickyMouse = true;
	_mouseLeewayTime = 30;

	setupView();
	// this is GetSnoidParts(true);
	loadResourceFile("midimpc");

	queueSound(99);

	Common::Event event;
	while (!shouldQuit()) {
		if (_newLeg != -1)
			newModule();

		while (_eventMan->pollEvent(event)) {
			switch (event.type) {
			case Common::EVENT_MOUSEMOVE:
				_needsUpdate = true;
				break;

			case Common::EVENT_LBUTTONUP:
				break;

			case Common::EVENT_LBUTTONDOWN:
				handleMouseDown(event.mouse);
				break;

			case Common::EVENT_KEYDOWN:
				switch (event.kbd.keycode) {
				case Common::KEYCODE_d:
					if (event.kbd.flags & Common::KBD_CTRL) {
						_console->attach();
						_console->onFrame();
					}
					break;

				case Common::KEYCODE_SPACE:
					pauseGame();
					break;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}

		if (_video->updateMovies())
			_needsUpdate = true;

		/*if (_needsUpdate) {
			_system->updateScreen();
			_needsUpdate = false;
			if (_backgroundId != 0xffff)
				_gfx->copyAnimImageToScreen(_backgroundId);
		}*/

		// Cut down on CPU usage
		_system->delayMillis(10);

		update();
	}

	return Common::kNoError;
}

uint32 MohawkEngine_Zoombini::getTime() {
	return _system->getMillis() / 17;
}

void MohawkEngine_Zoombini::setCursor(uint16 id) {
	if (_currentCursor == id)
		return;

	_currentCursor = id;
	if (_currentCursor == kWatchCursor) {
		_waitCursorId = 0;
		_nextCursorTime = getTime();
	}
	if (_currentCursor)
		_cursor->setCursor(_currentCursor);
	else
		_cursor->setDefaultCursor();
	_needsUpdate = true;
}

void MohawkEngine_Zoombini::e2FlushEvent(uint types) {
	Common::Event event;
	while (_eventMan->pollEvent(event)) {
		// TODO: queue messages which don't match types (1 = keys, 2 = mouse) somewhere
	}		
}

bool MohawkEngine_Zoombini::pdStillDown(uint /*which*/) {
	return getEventManager()->getButtonState() & 1;
}

void MohawkEngine_Zoombini::queueSound(uint16 id, bool streaming) {
	// TODO: music restarting
	// TODO: actually queue sound
	if (id < 30000)
		_sound->playSound(id);
	else
		_sound->playMidi(id);
}

void MohawkEngine_Zoombini::setTimeOfLastUserAction() {
	_lastTimeout = getTime();
	_lastUserAction = getTime();
	if (_idleWaitTime)
		_idleWaitTime = 64;
}

void MohawkEngine_Zoombini::loadResourceFile(Common::String name) {
	MohawkArchive *archive = new MohawkArchive();
	if (!archive->openFile(name + ".mhk"))
		error("failed to open %s.mhk", name.c_str());
	_mhk.push_back(archive);
}

void MohawkEngine_Zoombini::unloadResourceFile() {
	if (_mhk.size() < 3)
		error("tried unloadResourceFile() with no extra files loaded");
	delete _mhk.back();
	_mhk.pop_back();
}

void MohawkEngine_Zoombini::addHotspots(Common::Array<Common::Rect> *rects, Zoombini_Module::HotspotProc proc) {
	_hotspotRects.push_back(rects);
	_hotspotProcs.push_back(proc);
}

void MohawkEngine_Zoombini::snoidFadeIn() {
	// TODO

	setCursor(0);
}

void MohawkEngine_Zoombini::snoidFadeOut() {
	// TODO
}

uint32 MohawkEngine_Zoombini::offsetToFrame(bool snoid, uint index, uint16 &frame) {
	Common::SeekableReadStream *ourSCRB;
	if (snoid)
		ourSCRB = getResource(ID_SCRS, index + 100);
	else
 		ourSCRB = getSCRB(index);
	int16 count = ourSCRB->readSint16BE();
	if (snoid)
		ourSCRB->skip(2);
	// TODO: wtf is all this madness? :)
	if (count <= (int16)frame)
		frame = count - 1;
	if ((int16)frame < -count)
		frame = -count;
	if (frame > 0x7fff)
		frame += count;
	uint16 remaining = frame;
	while (remaining) {
		uint16 data = ourSCRB->readUint16BE();
		if (data > 0x7fff) {
			if (data < 0xff00)
				ourSCRB->skip(2);
			remaining--; 
		} else {
			ourSCRB->skip(4);
		}
	}
	uint32 pos = ourSCRB->pos() / 2;
	delete ourSCRB;
	return pos;
}

void MohawkEngine_Zoombini::freeScripts() {
	View::freeScripts();
	_dropSpots.clear();
	_restAreas.clear();
}

void MohawkEngine_Zoombini::installTerrain(uint16 resourceId) {
	// FIXME
}

void MohawkEngine_Zoombini::freeTerrain() {
	// FIXME
}

void MohawkEngine_Zoombini::setupView() {
	_haveNewGame = false;

	// TODO: sound priorities

	_lastNodeId = 0;

	// TODO: SetActiveSortModeOff(1);

	_rootNode = new OldFeature(this);
	_cursorNode = new OldFeature(this);
	//_rootNode->setNodeDefaults(1, _cursorNode, NULL);
	_rootNode->setNodeDefaults(NULL, _cursorNode);
	_rootNode->_id = 1;
	_rootNode->_data.enabled = 0;
	_rootNode->_flags = 0x1008000;
	//_cursorNode->setNodeDefaults(0xffff, NULL, _rootNode);
	_cursorNode->setNodeDefaults(_rootNode, NULL);
	_cursorNode->_id = 0xffff;
	_cursorNode->_data.enabled = 0;
	_cursorNode->_flags = 0x1000;
	// TODO: set draw/move callbacks

	for (uint i = 0; i < NUM_SYNC_CHANNELS; i++) {
		_syncChannels[i].masterId = 0;
		_syncChannels[i].state = 0;
		_syncChannels[i].alternate = false;
	}

	// TODO: InitSnoidUt(); with below
	_snoidPathUpdatesDisabled = false;
	_draggingSnoid = false;
	_dropSpotRange = 15;

	// TODO: reset sound queues
}

void MohawkEngine_Zoombini::idleView() {
	View::idleView();

	// TODO: play queued sound/music
}

Feature *MohawkEngine_Zoombini::installViewFeature(uint16 unknown1, uint16 unknown2, void *data, uint tempo, uint16 scrbId, Module::FeatureProc moveProc, Module::FeatureProc drawProc, uint32 flags) {
	uint16 nextId = getNewFeatureId();

	Feature *node = _rootNode;
	while (node) {
		if (node->_next && (node->_next->_id == 0xffff)) {
			break;
		} else if (unknown1 != 0xfffd || node->_id != _lastNodeId) {
			if (unknown1 && unknown1 == node->_id) {
				if (unknown2) {
					break;
				} else if (node->_id != 1) {
					node = node->_prev;
					break;
				}
			}
		} else
			break;

		node = node->_next;
	}
	if (!node)
		error("failed to install view feature");

	if (flags & kFeatureSortBackground)
		_lastNodeId = nextId;

	Feature *feature;
	if (flags & 1) {
		feature = new SnoidFeature(this);
		SnoidStruct *source = (SnoidStruct *)data;
		memcpy(&feature->_data, &source->_data, sizeof(FeatureData));
		memcpy(&((SnoidFeature *)feature)->_snoidData, &source->_snoidData, sizeof(SnoidData));
	} else if (flags & 2) {
		// TODO: copy data;
		warning("ignoring flag 2");
	} else
		feature = new OldFeature(this);
	// feature->setNodeDefaults(nextId, node->_next, node);
	feature->setNodeDefaults(node, node->_next);
	feature->_id = nextId;
	node->_next = feature;
	feature->_next->_prev = feature;
	if (flags & kFeatureOldAdjustByPos) {
		// TODO: set 214 and 218 (currentPos, nextPos) to dereferenced data?
		warning("ignoring point deref");
	}
	feature->_moveProc = moveProc;
	feature->_drawProc = drawProc;
	feature->_scrbId = scrbId;
	feature->_flags = flags;
	feature->_delayTime = tempo;
	if (flags & kFeatureOldDropSpot) {
		ZoombiniDropSpot dropSpot;
		dropSpot.pos = *(const Common::Point *)data;
		dropSpot.id = feature->_id;
		dropSpot.snoidId = 0;
		// The original maxes out at 125, but we don't have to care.
		_dropSpots.push_back(dropSpot);
	}
	return feature;
}

void MohawkEngine_Zoombini::freeViewFeatures() {
	// TODO: sound priorities

	// TODO: snoid party management

	for (Feature *node = _rootNode; node;) {
		Feature *currNode = node;
		node = currNode->_next;

		if (currNode->_next && currNode->_prev) {
			// this is not an end node
			assert(currNode != _rootNode && currNode != _cursorNode);

			// remove from list, delete
			currNode->_prev->_next = currNode->_next;
			currNode->_next->_prev = currNode->_prev;
			delete currNode;
		}
	}

	freeNodesAndPaths();
	freeScripts();
	freeTerrain();
	freeRejectScripts();

	for (uint i = 0; i < NUM_SYNC_CHANNELS; i++) {
		_syncChannels[i].masterId = 0;
		_syncChannels[i].state = 0;
		_syncChannels[i].alternate = false;
	}
	_dragOneTime = 0;
	_lastNodeId = 0;
	_dropSpotRange = 15;
	// TODO: SetActiveSortModeOff(1);
	// TODO: reset sound queues
	_inDialog = false;
	// TODO: SetSnoidWalkCallback(NULL);
}

void MohawkEngine_Zoombini::setViewRestAreas(bool assign, const Common::Array<Common::Point> &restAreas) {
	for (uint i = 0; i < 125; i++)
		_snoidOnRestArea[i] = 0;
	_restAreas = restAreas;
	if (!assign)
		return;
	uint count = numSnoidsInParty();
	_prevSnoid = NULL;
	for (uint i = 0; i < count; i++) {
		SnoidFeature *snoid = findNextSnoid(false);
		if (!snoid)
			continue;
		snoid->_data.currentPos = _restAreas[i];
		snoid->_snoidData.mode = 0;
	}
}

void MohawkEngine_Zoombini::getSnoidParts() {
	// TODO: preload compound shapes 3000, 3001, 3100
	initSnoidScripts();
	loadRegistrationData(regs100, 100);
	loadRegistrationData(regs101, 101);
	loadRegistrationData(regs102, 102);
	loadRegistrationData(regs103, 103);
}

void MohawkEngine_Zoombini::freeSnoidParts() {
	// TODO
	// setSnoidSize(1);
	freeSnoidScripts();
	freeRegistrationData();
}

void MohawkEngine_Zoombini::loadRegistrationData(Common::Array<uint16> &regs, uint16 resourceId) {
	regs.clear();

	Common::SeekableReadStream *regsStream = getResource(ID_REGS, resourceId);
	while (regsStream->pos() != regsStream->size())
		regs.push_back(regsStream->readUint16BE());
	delete regsStream;
}

void MohawkEngine_Zoombini::freeRegistrationData() {
	regs100.clear();
	regs101.clear();
	regs102.clear();
	regs103.clear();
}

uint MohawkEngine_Zoombini::numSnoidsInParty() {
	uint count = 0;
	for (Feature *node = _rootNode; node; node = node->_next) {
		if ((node->_flags & 1) && node->_data.enabled)
			if (((SnoidFeature *)node)->_snoidData.inPartyStatus)
				count++;
	}
	return count;
}

uint MohawkEngine_Zoombini::numSnoidsInModule() {
	uint count = 0;
	for (Feature *node = _rootNode; node; node = node->_next) {
		if (node->_flags & 1)
			count++;
	}
	return count;
}

void MohawkEngine_Zoombini::initSnoidScripts() {
	// We don't have to do anything here, since we just (inefficiently) load the scripts (100 to 150) as we go.
	// TODO: Read/cache the scripts somehow.
}

void MohawkEngine_Zoombini::freeSnoidScripts() {
}

void MohawkEngine_Zoombini::initRejectScripts(uint count, uint max, uint16 resourceBase) {
	// FIXME
}

void MohawkEngine_Zoombini::initNormalScripts(uint count, uint max, uint16 resourceBase) {
	// FIXME
}

void MohawkEngine_Zoombini::freeRejectScripts() {
	// FIXME
	_snoidPathUpdatesDisabled = false;
}

uint16 MohawkEngine_Zoombini::addSnoidToScreen(Common::Point dest, Common::Point pos, uint32 nextTime, SnoidStruct *data) {
	Common::Point tmp = data->_data.currentPos;
	data->_snoidData.unknown241 = 1;
	data->_snoidData.unknown242 = 0;
	data->_data.currentPos = pos;
	data->_snoidData.dest = dest;
	data->_data.bitmapIds[0] = 0;
	data->_data.unknown192 = 0;
	data->_snoidData.unknown248 = _rnd->getRandomNumberRng(0, 64);
	uint16 id = installOneSnoid(true, data);
	Feature *snoid = getFeaturePtr(id);
	snoid->_nextTime = nextTime;
	data->_snoidData.unknown241 = 0;
	data->_data.currentPos = tmp;
	return id;
}

uint16 MohawkEngine_Zoombini::installOneSnoid(bool unknown, SnoidStruct *data) {
	if (!data->_snoidData.unknown241)
		return 0;

	for (uint i = 0; i < 16; i++)
		data->_snoidData.unknown194[i] = 0;

	SnoidFeature *snoid = (SnoidFeature *)installViewFeature(0, 0, data, 6, 0, (Module::FeatureProc)&Zoombini_Module::snoidMoveProc, (Module::FeatureProc)&Zoombini_Module::snoidDrawProc, 1);
	snoid->setNewSnoidModeAndXY(Common::Point(), (unknown ? 7 : 0));
	snoid->_nextTime = 0;
	return snoid->_id;
}

uint16 MohawkEngine_Zoombini::snoidOnThisDropSpot() {
	if (_lastSnoidId)
		return _lastDropSpotId + 1;
	else
		return 0;
}

void MohawkEngine_Zoombini::markNthDropSpot(uint16 snoidId, uint16 dropspotId) {
	if (!dropspotId)
		return;
	dropspotId--;
	if (dropspotId >= _dropSpots.size())
		error("markNthDropSpot tried to mark dropspot %d of %d", dropspotId + 1, _dropSpots.size());
	_dropSpots[dropspotId].snoidId = snoidId;
}

void MohawkEngine_Zoombini::installNodesAndPaths(uint16 id) {
	if (_nodes.size() || _paths.size())
		error("installNodesAndPaths called with nodes/paths already present");

	Common::SeekableReadStream *nodeStream = getResource(ID_NODE, id);
	uint16 nodeCount = nodeStream->readUint16BE();
	_nodes.reserve(nodeCount);
	for (uint i = 0; i < nodeCount; i++) {
		Common::Point pos;
		pos.x = nodeStream->readUint16BE();
		pos.y = nodeStream->readUint16BE();
		_nodes.push_back(pos);
	}
	delete nodeStream;

	Common::SeekableReadStream *pathStream = getResource(ID_PATH, id);
	uint16 pathCount = pathStream->readUint16BE();
	_paths.reserve(pathCount);
	for (uint i = 0; i < pathCount; i++) {
		Common::Array<byte> nodes;
		for (uint n = 0; n < 24; n++)
			nodes.push_back(pathStream->readByte());
		_paths.push_back(nodes);
	}
	delete pathStream;
}

void MohawkEngine_Zoombini::freeNodesAndPaths() {
	_nodes.clear();
	_paths.clear();
}

void MohawkEngine_Zoombini::setSnoidsInPartyStatus(bool enable, byte status) {
	uint count = 0;
	for (Feature *node = _rootNode; node; node = node->_next) {
		if (!(node->_flags & 1))
			continue;

		SnoidFeature *snoid = (SnoidFeature *)node;
		if (enable)
			snoid->_data.enabled = 1;

		if (count >= 20) {
			snoid->_snoidData.inPartyStatus = 0;
			continue;
		}
		if (snoid->_data.enabled) {
			snoid->_snoidData.inPartyStatus = status;
			if (status)
				count++;
		} else {
			snoid->_snoidData.inPartyStatus = 0;
		}
	}
}

void MohawkEngine_Zoombini::snoidDirectionAfterFall(int direction) {
	if (direction == -1)
		_snoidDirectionAfterFall = 1;
	else if (direction == 1)
		_snoidDirectionAfterFall = 2;
	else
		_snoidDirectionAfterFall = 0;
}

uint16 MohawkEngine_Zoombini::getSnoidSoundId(uint type, SnoidData *data) {
	if (type == 16) {
		return _rnd->getRandomNumberRng(1800, 1814);
	} else if (type == 17) {
		return 99;
	} else if (type > 17) {
		error("getSnoidSoundId doesn't understand type %d", type);
	}

	// snoid sounds are in groups of 5*5 depending on hair and nose style
	uint16 soundId = 100 + (type * 25);

	// adjust for hair style
	switch (data->part[kSnoidPartHair]) {
	case 2:
		soundId += 5;
		break;
	case 3:
		soundId += 20;
		break;
	case 4:
		soundId += 15;
		break;
	case 5:
		soundId += 10;
		break;
	}

	// adjust for nose style
	soundId += (data->part[kSnoidPartNose] - 1);

	return soundId;
}

SnoidFeature *MohawkEngine_Zoombini::getSnoidPtrFromId(bool reset, uint16 id) {
	Feature *feature = getFeaturePtr(id);
	if (feature && (feature->_flags & 1)) {
		SnoidFeature *snoid = (SnoidFeature *)feature;
		if (reset)
			snoid->_nextTime = 0;
		return snoid;
	}

	return NULL;
}

SnoidFeature *MohawkEngine_Zoombini::findNextSnoid(bool restart) {
	Feature *next;

	if (!restart && _prevSnoid) {
		next = _prevSnoid->_next;
		_prevSnoid = NULL;
	} else {
		next = _rootNode->_next;
	}
	while (next && !(next->_flags & 1)) {
		next = next->_next;
	}
	if (next)
		_prevSnoid = (SnoidFeature *)next;
	return (SnoidFeature *)next;
}

uint16 MohawkEngine_Zoombini::dragSnoid(SnoidFeature *snoid, Common::Point mousePos, Common::Rect rect) {
	if (rect.isEmpty())
		rect = Common::Rect(0, 0, 640, 480);

	uint oldIdleWaitTime = _idleWaitTime;
	_lastSnoidId = 0;
	_lastDropSpotId = 0;
	_idleWaitTime = 0;

	uint16 foundDropSpotId = 0;

	uint16 snoidId = snoid->_id;
	// TODO: weird previous feature stuff (see continueToDrag)
	removeFeature(snoid, false);
	_draggingSnoid = true;
	snoid->_id = 0xfffd;
	uint32 oldSnoidFlags = snoid->_flags;
	snoid->_flags |= 0x4001000;
	insertUnderCursor(snoid);
	// FIXME: setupNamePlate()
	uint32 oldSnoidDelayTime = snoid->_delayTime;
	snoid->_nextTime = 0;
	snoid->_delayTime = 3;
	snoid->_snoidData.unknown248 = 0;
	Common::Point snoidPos = snoid->_data.currentPos;
	// TODO: or bounds into dirty feature rect
	snoid->setNewSnoidModeAndXY(Common::Point(), 5);
	// TODO: or bounds into dirty feature rect

	Common::Point oldMousePos = mousePos;

	Common::Rect firstCheckRect(snoid->_data.currentPos.x - _dropSpotRange,
		snoid->_data.currentPos.y - _dropSpotRange,
		snoid->_data.currentPos.x + _dropSpotRange,
		snoid->_data.currentPos.y + _dropSpotRange);
	for (uint i = 0; i < _dropSpots.size(); i++) {
		if (!firstCheckRect.contains(_dropSpots[i].pos))
			continue;
		foundDropSpotId = i + 1;
		if (_dropSpots[i].snoidId == snoidId)
			_dropSpots[i].snoidId = 0;
		break;
	}
	for (uint i = 0; i < _restAreas.size(); i++) {
		if (!firstCheckRect.contains(_restAreas[i]))
			continue;
		// FIXME: remove snoid from rest area if needed
		break;
	}

	bool moved = true;
	uint16 dropSpotFeatureId = 0;
	uint16 dropSpotId;
	while (continueToDrag()) {
		mousePos = getEventManager()->getMousePos();

		// Make snoid look left/right if moving.
		if (oldMousePos.x < mousePos.x && snoid->_snoidData.unknown242)
			snoid->_snoidData.unknown242 = 0;
		else if (oldMousePos.x > mousePos.x && !snoid->_snoidData.unknown242)
			snoid->_snoidData.unknown242 = 1;

		if (false /* TODO: weird previous feature stuff */) {
			moved = true;
		} else {
			// Move the snoid.
			Common::Point newPos = mousePos;
			if (newPos.x < rect.left)
				newPos.x = rect.left;
			else if (newPos.x > rect.right)
				newPos.x = rect.right;
			if (newPos.y < rect.top)
				newPos.y = rect.top;
			else if (newPos.y > rect.bottom)
				newPos.y = rect.bottom;
			snoid->_data.currentPos = newPos;
		}

		// Update the mouse position.
		if (oldMousePos != mousePos) {
			moved = true;
		}
		oldMousePos = mousePos;

		if (!_dropSpots.empty() && (moved || _dragOneTime)) {
			Common::Rect checkRect(snoid->_data.currentPos.x - _dropSpotRange,
				snoid->_data.currentPos.y - _dropSpotRange,
				snoid->_data.currentPos.x + _dropSpotRange,
				snoid->_data.currentPos.y + _dropSpotRange);
			bool foundDropSpot = false;
			if (!_dragOneTime) {
				for (uint i = 0; i < _dropSpots.size(); i++) {
					// TODO: check some global (for bridge rejection?), break if set
					if (_dropSpots[i].snoidId)
						continue;
					if (!checkRect.contains(_dropSpots[i].pos))
						continue;
					Feature *dropSpot = getFeaturePtr(_dropSpots[i].id);
					if (!dropSpot)
						continue;
					foundDropSpot = true;
					if (dropSpotFeatureId == dropSpot->_id)
						break;
					if (dropSpotFeatureId) {
						// Disable the previous drop spot.
						Feature *oldDropSpot = getFeaturePtr(dropSpotFeatureId);
						if (oldDropSpot->_data.enabled) {
							oldDropSpot->_flags |= kFeatureOldReset;
							oldDropSpot->_nextTime = 0;
							update();
						}
					}
					dropSpotId = i;
					dropSpotFeatureId = dropSpot->_id;
					dropSpot->_data.enabled = 1;
					dropSpot->_data.currFrame = 0;
					dropSpot->_data.currOffset = 1;
					break;
				}
			}
			if (!foundDropSpot && dropSpotFeatureId) {
				// Disable the previous drop spot.
				Feature *oldDropSpot = getFeaturePtr(dropSpotFeatureId);
				if (oldDropSpot->_data.enabled) {
					oldDropSpot->_flags |= kFeatureOldReset;
					oldDropSpot->_nextTime = 0;
					update();
				}
				dropSpotFeatureId = 0;
			}
		}

		update();
		setTimeOfLastUserAction();
	}

	if (dropSpotFeatureId && !false /* TODO: weird flag */) {
		_dropSpots[dropSpotId].snoidId = snoidId;
		_lastDropSpotId = dropSpotId;
		_lastSnoidId = snoidId;
		Feature *dropSpot = getFeaturePtr(dropSpotFeatureId);
		if (dropSpot->_data.enabled) {
			dropSpot->_flags |= kFeatureOldReset;
			dropSpot->_nextTime = 0;
			update();
		}
	}
	// FIXME: deactivateNamePlate()
	// TODO: or bounds into dirty feature rect
	if (true /* TODO: weird flag */) {
		if (dropSpotFeatureId)
			snoid->_snoidData.dest = _dropSpots[dropSpotId].pos;
		else if (_currentLeg == 6) // TODO: constant (kZoombiniLegTown?)
			snoid->_snoidData.dest = snoidPos;
		/* FIXME else if (!snoidOnValidTerrain(snoid))
			snoid->_snoidData.dest = snoidPos; */
	}
	snoid->_snoidData.unknown248 = 1;
	// TODO: or bounds into dirty feature rect
	if (false /* TODO: weird flag */) {
		snoid->_snoidData.unknown242 = 0;
		snoid->setNewSnoidModeAndXY(Common::Point(), 0);
	} else {
		snoid->setNewSnoidModeAndXY(Common::Point(), 4);
	}
	snoid->_id = snoidId;
	snoid->_flags = oldSnoidFlags;
	snoid->_delayTime = oldSnoidDelayTime;
	_idleWaitTime = oldIdleWaitTime;
	// TODO: weird previous feature stuff
	_dragOneTime = 0;
	_draggingSnoid = true;

	return foundDropSpotId;
}

bool MohawkEngine_Zoombini::continueToDrag() {
	static bool currentlyDragging = false;
	static bool wasDown, beSticky;

	e2FlushEvent(0); // FIXME: hack because pdStillDown doesn't report correct values otherwise

	if (currentlyDragging) {
		if (_dragOneTime == 1) {
			_dragOneTime = 2;
			currentlyDragging = false;
			if (_hideCursor)
				_cursor->showCursor();
			e2FlushEvent(3);
			return false;
		}
	} else
		_dragOneTime = 0;

	if (!currentlyDragging) {
		// Start dragging.
		currentlyDragging = true;
		wasDown = pdStillDown(_lastMouseDown);
		beSticky = _stickyMouse;
		if (_hideCursor)
			_cursor->hideCursor();
		return true;
	}

	bool mouseDown = pdStillDown(_lastMouseDown);
	if (false /* TODO: weird flag */ && !mouseDown) {
		currentlyDragging = false;
	} else if (beSticky) {
		if (wasDown && !mouseDown) {
			// User has released mouse, but we're sticky.
			wasDown = false;
		} else if (!wasDown && mouseDown) {
			// User has clicked mouse again: stop dragging.
			currentlyDragging = false;
		}
	} else if (!mouseDown) {
		if (!_stickyMouse || getTime() - _lastMouseTime >= _mouseLeewayTime) {
			// User has released mouse, and we're not sticky: stop dragging.
			currentlyDragging = false;
		} else {
			// We should be sticky, become sticky.
			beSticky = true;
			wasDown = false;
		}
	}

	if (!currentlyDragging && _hideCursor)
		_cursor->showCursor();

	if (!currentlyDragging)
		e2FlushEvent(3);

	return currentlyDragging;
}

void MohawkEngine_Zoombini::handleMouseDown(Common::Point pos) {
	for (uint i = 0; i < _hotspotRects.size(); i++) {
		Common::Array<Common::Rect> &rects = *_hotspotRects[i];
		for (uint n = 0; n < rects.size(); n++) {
			if (rects[n].contains(pos)) {
				(_currentModule->*(_hotspotProcs[i]))(n + 1);
				return;
			}
		}
	}
}

void MohawkEngine_Zoombini::update() {
	if (_currentModule)
		_currentModule->update();

	static const uint16 cursorList[12] = { 2, 4, 2, 3, 2, 5, 2, 5, 2, 4, 3, 5 };
	if (_currentCursor >= kWatchCursor && getTime() >= _nextCursorTime) {
		if (_waitCursorId >= 12)
			_waitCursorId = 0;
		setCursor(cursorList[_waitCursorId++]);
		_nextCursorTime = getTime() + 12;
	}
}

void MohawkEngine_Zoombini::newModule() {
	_hotspotRects.clear();
	_hotspotProcs.clear();

	delete _currentModule;
	_currentModule = NULL;

	_currentLeg = _newLeg;
	switch (_newLeg) {
	case 0:
		_currentModule = new Zoombini_Title(this);
		break;
	case 3:
		_currentModule = new Zoombini_PickerScreen(this);
		break;
	case 7:
		_currentModule = new Zoombini_Bridge(this);
		break;
	default:
		error("unknown leg %d", _newLeg);
	}

	_newLeg = -1;
	_nextAmbientTime = _system->getMillis() + 900;
	_currentModule->init();
}

} // End of namespace Mohawk
