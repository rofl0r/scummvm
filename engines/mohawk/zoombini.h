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
	uint16 previousLeg; // +202
	uint16 currentLeg; // +204
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

	bool _haveNewGame, _wasInTitle;
	int _tmpNextLeg, _previousLeg, _newLeg, _currentLeg;

	bool _inDialog;

	uint32 getTime();
	void setCursor(uint16 id);

	void e2FlushEvent(uint types);
	bool pdStillDown(uint which);

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
	void markNthDropSpot(uint16 snoidId, uint16 dropspotId);
	void installNodesAndPaths(uint16 id);
	void freeNodesAndPaths();
	void setSnoidsInPartyStatus(bool unknown, byte status);
	void snoidDirectionAfterFall(int direction);
	uint16 getSnoidSoundId(uint type, struct SnoidData *data);
	SnoidFeature *getSnoidPtrFromId(bool reset, uint16 id);

	uint16 dragSnoid(SnoidFeature *snoid, Common::Point mousePos, Common::Rect rect = Common::Rect());
	bool continueToDrag();

	Common::Array<ZoombiniDropSpot> _dropSpots;
	uint _dropSpotRange;

	ZoombiniGameState _state;

	uint _idleWaitTime;
	uint _snoidDirectionAfterFall;
	uint16 _sortedSnoids[21];
	uint _numMovingSnoids, _numIdleSnoids;

	Common::Array<Common::Point> _restAreas;

	Common::Array<Common::Point> _nodes;
	Common::Array<Common::Array<byte> > _paths;
	bool _snoidPathUpdatesDisabled;

	Common::Array<uint16> regs100;
	Common::Array<uint16> regs101;
	Common::Array<uint16> regs102;
	Common::Array<uint16> regs103;

	uint _dragOneTime;

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
