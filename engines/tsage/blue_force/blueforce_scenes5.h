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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TSAGE_BLUEFORCE_SCENES5_H
#define TSAGE_BLUEFORCE_SCENES5_H

#include "common/scummsys.h"
#include "tsage/blue_force/blueforce_logic.h"
#include "tsage/blue_force/blueforce_speakers.h"
#include "tsage/converse.h"
#include "tsage/events.h"
#include "tsage/core.h"
#include "tsage/scenes.h"
#include "tsage/globals.h"
#include "tsage/sound.h"

namespace TsAGE {

namespace BlueForce {

using namespace TsAGE;

class Scene550: public SceneExt {
	/* Objects */
	class Object1: public NamedObject {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class CaravanDoor: public NamedObject {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class Vechile: public NamedObject {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};

	/* Actions */
	class Action1: public ActionExt {
	public:
		virtual void signal();
	};
public:
	SequenceManager _sequenceManager;
	Object1 _object1;
	CaravanDoor _caravanDoor;
	Vechile _vechile;
	NamedHotspot _item1, _item2, _item3;
	SpeakerGameText _gameTextSpeaker;
	SpeakerLyleHat _lyleHatSpeaker;
	SpeakerJakeJacket _jakeJacketSpeaker;
	Action1 _action1;

	virtual void postInit(SceneObjectList *OwnerList = NULL);
	virtual void signal();
};

class Scene551: public Scene550 {
	/* Objects */
	class Vechile: public NamedObject {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class DrunkStanding: public NamedObjectExt {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class Drunk: public NamedObjectExt {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class PatrolCarTrunk: public NamedObject {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class TrunkInset: public FocusObject {
	public:
		virtual void remove();
		virtual bool startAction(CursorType action, Event &event);
	};
	class TrunkKits: public NamedObject {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class Harrison: public NamedObjectExt {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};

	/* Items */
	class Item4: public SceneHotspot {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};

	/* Actions */
	class Action2: public Action {
	public:
		virtual void signal();
	};
public:
	SpeakerDrunk _drunkSpeaker;
	SpeakerJakeUniform _jakeUniformSpeaker;
	SpeakerGiggles _gigglesSpeaker;
	Vechile _vechile2;
	DrunkStanding _drunkStanding;
	Drunk _drunk;
	PatrolCarTrunk _patrolCarTrunk;
	TrunkInset _trunkInset;
	TrunkKits _trunkKits;
	Harrison _harrison;
	NamedObject _object11, _object12, _object13;
	SceneObject _object14, _object15;
	Item4 _item4;
	Action2 _action2;
	int _field1CD0, _field1CD2;

	Scene551();
	virtual void synchronize(Serializer &s);
	void postInit(SceneObjectList *OwnerList = NULL);
	virtual void signal();
	virtual void dispatch();
};

class Scene560: public SceneExt {
	/* Objects */
	class DeskChair: public NamedObject {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class Object2: public NamedObjectExt {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class SafeInset: public FocusObject {
		/* Items */
		class Item: public NamedHotspotExt {
		public:
			virtual bool startAction(CursorType action, Event &event);
		};
	public:
		NamedObject _digit0, _digit1, _digit2;
		Item _item1, _item2, _item3, _item4, _item5, _item6;
		Visage _cursorVisage;

		virtual Common::String getClassName() { return "Scene560_SafeInset"; }
		virtual void postInit(SceneObjectList *OwnerList = NULL);
		virtual void remove();
		virtual void signal();
		virtual void process(Event &event);
	};
	class Nickel: public NamedObject {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};
	class Object5: public FocusObject {
		/* Items */
		class Item1: public NamedHotspot {
		public:
			virtual bool startAction(CursorType action, Event &event);
		};
	public:
		Item1 _item1;

		virtual Common::String getClassName() { return "Scene560_Object5"; }
		virtual void postInit(SceneObjectList *OwnerList = NULL);
		virtual void remove();
	};

	/* Item groups */
	class PicturePart: public NamedHotspotExt {
	public:
		virtual Common::String getClassName() { return "Scene560_Group1"; }
		virtual bool startAction(CursorType action, Event &event);
	};

	/* Items */
	class Computer: public NamedHotspot {
	public:
		virtual bool startAction(CursorType action, Event &event);
	};

	/* Actions */
	class Action1: public Action {
	public:
		virtual void signal();
	};
	class Action2: public Action {
	public:
		virtual void signal();
	};
	class Action3: public Action {
	public:
		virtual void signal();
	};
public:
	Action1 _action1;
	Action2 _action2;
	Action3 _action3;
	SpeakerGameText _gameTextSpeaker;
	DeskChair _deskChair;
	Object2 _object2;
	SafeInset _safeInset;
	Nickel _nickel;
	Object5 _object5;
	NamedObject _object6;
	PicturePart _picture1, _picture2, _picture3, _picture4;
	Computer _computer;
	NamedHotspot _chair, _lamp, _item4, _trophy, _watercolours, _fileCabinets;
	NamedHotspot _certificate, _bookcase, _desk, _carpet, _item12, _office;
	ASound _sound1;
	int _field380, _field11EA;
	Common::Point _destPosition;

	Scene560();
	virtual void postInit(SceneObjectList *OwnerList = NULL);
	virtual void signal();
	virtual void process(Event &event);
	virtual void dispatch();
};

} // End of namespace BlueForce

} // End of namespace TsAGE

#endif