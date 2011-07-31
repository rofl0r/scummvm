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

#ifndef MOHAWK_ZOOMBINI_H
#define MOHAWK_ZOOMBINI_H

#include "mohawk/mohawk.h"
#include "mohawk/console.h"
#include "mohawk/graphics.h"
#include "mohawk/view.h"

#include "common/random.h"

#include "audio/mixer.h"

namespace Mohawk {

class MohawkEngine_Zoombini;
class SnoidFeature;

class Zoombini_Module : public Module {
public:
	Zoombini_Module(MohawkEngine_Zoombini *vm) : _vm(vm) { }
	virtual ~Zoombini_Module() { }

	virtual void init() = 0;
	virtual void shutdown() = 0; // Purge
	virtual void update() = 0; // DelayProc
	virtual void keyDown(uint key) = 0; // DoKey

	void defaultMoveProc(OldFeature *feature);
	void defaultDrawProc(OldFeature *feature);
	void snoidMoveProc(SnoidFeature *feature);
	void snoidDrawProc(SnoidFeature *feature);

protected:
	MohawkEngine_Zoombini *_vm;
};

enum {
	kArrowCursor = 0,
	kWatchCursor = 1
};

enum {
	kSnoidPartHair = 0,
	kSnoidPartEyes = 1,
	kSnoidPartNose = 2,
	kSnoidPartFeet = 3
};

enum {
	kZoombiniModuleTitle = 0,
	kZoombiniModuleMap = 1,
	kZoombiniModuleXfer = 2,
	kZoombiniModulePicker = 3,
	kZoombiniModuleBaseCamp1 = 4,
	kZoombiniModuleBaseCamp2 = 5,
	kZoombiniModuleTown = 6,
	kZoombiniModuleBridge = 7,
	kZoombiniModuleTunnels = 8,
	kZoombiniModulePizza = 9,
	kZoombiniModuleFerry = 10,
	kZoombiniModuleLilly = 11,
	kZoombiniModuleRise = 12,
	kZoombiniModuleFleens = 13,
	kZoombiniModuleHotel = 14,
	kZoombiniModuleMudball = 15,
	kZoombiniModuleCaves = 16,
	kZoombiniModuleMirror = 17,
	kZoombiniModuleMaze = 18
};

// In the original, this is appended to the end of the OldFeature struct
// for snoids; a plain snoid struct contains FeatureData and SnoidData, and is
// then copied into the SnoidFeature on creation.
struct SnoidData {
	// starts at +236 inside feature, or +188 inside SnoidStruct

	byte part[4]; // +188
	uint16 drawOrder;
	uint16 unknown194[16];
	Common::Point unknown226;

	Common::Point dest; // +230/232
	byte currentNode; // +234
	byte currentPath; // +235
	Common::Point stepSize; // +236
	signed char pathStep; // +240

	byte unknown241;
	uint16 unknown242;
	byte mode; // +244 (or +292 inside parent)
	uint16 unknown245; // +245 (occasionally referenced as a byte, doesn't matter)
	byte inPartyStatus; // +247
	byte unknown248;
	char name[10]; // yes, at +249
};

struct SnoidStruct {
	FeatureData _data;
	SnoidData _snoidData;
};

struct SnoidFeature : public OldFeature {
	SnoidFeature(MohawkEngine_Zoombini *vm);

	bool nextPointOnPath();
	void walkSnoidToPoint(Common::Point pos);
	void setNewSnoidModeAndXY(Common::Point pos, uint mode);
	uint16 setSnoidBounds(uint16 *something);
	void setSnoidDrawOrder(uint16 index);

	SnoidData _snoidData;
};

// 44559 bytes (in v1.1)
struct ZoombiniGameState {
	uint16 unknown0;
	uint16 unknown2;
	byte unknown4;
	byte unknown5;
	byte unknown6;
	byte unknown7;
	byte debugMode; // +8
	byte unknown9;
	uint16 unknown10;
	uint16 unknown16;
	// +18..
	uint16 unknown38;
	// +40..
	uint16 totalSnoidCount; // +72
	uint16 unknown74;
	uint16 unknown76;
	uint16 unknown78;
	// +80..
	uint16 previousModule; // +202
	uint16 currentModule; // +204
	// +206..
	// data for snoids (starting with count?):
	// at +41468: 612 bytes plus word
	// (at +42082: 614*2)?
	// at +43310: 612 bytes plus word
	byte snoidCount[5][5][5][5]; // 625 bytes at +43924
	// ** end of saved data **
	uint16 unknown44549[5];
};

struct ZoombiniDropSpot {
	Common::Point pos;
	uint16 id;
	uint16 snoidId;
};

class MohawkEngine_Zoombini : public MohawkEngine, public View {
protected:
	Common::Error run();

public:
	MohawkEngine_Zoombini(OSystem *syst, const MohawkGameDescription *gamedesc);
	virtual ~MohawkEngine_Zoombini();

	Common::RandomSource *_rnd;

	ZoombiniGraphics *_gfx;
	//bool _needsUpdate;

	GUI::Debugger *getDebugger() { return _console; }

	bool _haveNewGame, _wasInTitle, _practiceMode;
	int _tmpNextModuleId, _previousModuleId, _newModuleId, _currentModuleId;
	bool _puzzleLevelJustUpdated, _forceDisableXfer;
	uint currentLegOfGame(bool &isLastModule);

	bool _inDialog;

	uint32 getTime();
	void setCursor(uint16 id);

	void e2FlushEvent(uint types);
	bool pdStillDown(uint which);

	void queueSound(uint16 id, bool streaming = false);

	void setTimeOfLastUserAction();

	void loadResourceFile(Common::String name);
	void unloadResourceFile();

	void addHotspots(Common::Array<Common::Rect> *rects, Zoombini_Module::HotspotProc proc);

	void snoidFadeIn();
	void snoidFadeOut();

	uint32 offsetToFrame(bool snoid, uint scrbIndex, uint16 &frame);

	void freeScripts();
	void installTerrain(uint16 resourceId);
	void freeTerrain();

	void setupView();
	void idleView();
	Feature *installViewFeature(uint16 unknown1, uint16 unknown2, void *data, uint tempo, uint16 scrbId, Module::FeatureProc moveProc, Module::FeatureProc drawProc, uint32 flags);
	void freeViewFeatures();
	void setViewRestAreas(bool assign, const Common::Array<Common::Point> &restAreas);

	uint32 _lastTimeout, _lastUserAction;

	// Snoidut
	void initSnoidUt();
	void getSnoidParts();
	void freeSnoidParts();
	void loadRegistrationData(Common::Array<uint16> &regs, uint16 resourceId);
	void freeRegistrationData();
	uint numSnoidsInParty();
	uint numSnoidsInModule();
	void initSnoidScripts();
	void freeSnoidScripts();
	void initRejectScripts(uint count, uint max, uint16 resourceBase);
	void initNormalScripts(uint count, uint max, uint16 resourceBase);
	void freeRejectScripts();
	uint16 addSnoidToScreen(Common::Point dest, Common::Point pos, uint32 nextTime, struct SnoidStruct *data);
	uint16 installOneSnoid(bool enable, struct SnoidStruct *data);
	uint16 snoidOnThisDropSpot();
	Common::Point getNthRestPoint(uint id);
	void markNthDropSpot(uint16 snoidId, uint16 dropspotId);
	void installNodesAndPaths(uint16 id);
	void freeNodesAndPaths();
	void setSnoidsInPartyStatus(bool unknown, byte status);
	void snoidDirectionAfterFall(int direction);
	uint16 getSnoidSoundId(uint type, struct SnoidData *data);
	SnoidFeature *getSnoidPtrFromId(bool reset, uint16 id);

	uint16 dragSnoid(SnoidFeature *snoid, Common::Point mousePos, Common::Rect rect = Common::Rect());
	bool continueToDrag();

	void checkDropSpotsFor(SnoidFeature *snoid);
	void checkRestAreasFor(SnoidFeature *snoid);

	Common::Array<ZoombiniDropSpot> _dropSpots;
	uint _dropSpotRange;
	bool _checkSnoidDropSpotsDuringMove;
	bool _dragShouldCheckDropSpots;

	ZoombiniGameState _state;

	uint _idleWaitTime;
	uint _snoidDirectionAfterFall;
	uint16 _sortedSnoids[21];
	uint _numMovingSnoids, _numIdleSnoids;
	bool _bridgeStaticSnoidHack;

	Common::Array<Common::Point> _restAreas;

	Common::Array<Common::Point> _nodes;
	Common::Array<Common::Array<byte> > _paths;
	bool _snoidPathUpdatesDisabled;

	Common::Array<uint16> regs100;
	Common::Array<uint16> regs101;
	Common::Array<uint16> regs102;
	Common::Array<uint16> regs103;

	uint _dragOneTime;
	bool _dragDisallowMovement;

	uint _lastMouseDown;

private:
	ZoombiniConsole *_console;

	void handleMouseDown(Common::Point pos);
	void update();

	void newModule();

	uint16 _currentCursor, _waitCursorId;
	uint32 _nextCursorTime;

	bool _hideCursor, _stickyMouse;
	uint32 _lastMouseTime;
	uint32 _mouseLeewayTime;

	uint32 _nextAmbientTime;

	uint16 _lastNodeId;

	bool _draggingSnoid;
	uint16 _lastSnoidId, _lastDropSpotId;

	uint16 _snoidOnRestArea[125];

	SnoidFeature *_prevSnoid;
	SnoidFeature *findNextSnoid(bool restart);

	Common::Array<Common::Array<Common::Rect>*> _hotspotRects;
	Common::Array<Zoombini_Module::HotspotProc> _hotspotProcs;
};

} // End of namespace Mohawk

#endif
