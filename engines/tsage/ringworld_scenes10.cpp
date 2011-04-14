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
 * $URL$
 * $Id$
 *
 */

#include "graphics/cursorman.h"
#include "tsage/ringworld_scenes10.h"
#include "tsage/scenes.h"
#include "tsage/tsage.h"
#include "tsage/staticres.h"

namespace tSage {

Scene2::Scene2() : Scene() {
	_sceneState = 0;
}

/*--------------------------------------------------------------------------*/

void Object9350::postInit(SceneObjectList *OwnerList) {
	_globals->_sceneManager.postInit(&_globals->_sceneManager._altSceneObjects);
}

void Object9350::draw() {
	reposition();
	Rect destRect = _bounds;
	destRect.translate(-_globals->_sceneOffset.x, -_globals->_sceneOffset.y);
	Region *priorityRegion = _globals->_sceneManager._scene->_priorities.find(_globals->_sceneManager._scene->_stripManager._stripNum);
	GfxSurface frame = getFrame();
	_globals->gfxManager().copyFrom(frame, destRect, priorityRegion);
}

/*--------------------------------------------------------------------------
 * Scene 9100
 *
 *--------------------------------------------------------------------------*/
void Scene9100::SceneHotspot1::doAction(int action) {
	Scene9100 *scene = (Scene9100 *)_globals->_sceneManager._scene;

	if (action == CURSOR_TALK) {
		if (_globals->getFlag(23)) {
			_globals->_player.disableControl();
			scene->_sceneMode = 9104;
		} else {
			_globals->setFlag(23);
			_globals->_player.disableControl();
			scene->_sceneMode = 9105;
		}
		scene->setAction(&scene->_sequenceManager, scene, scene->_sceneMode, &_globals->_player, &scene->_object5, &scene->_object6, 0);
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9100::dispatch() {
	Scene9100 *scene = (Scene9100 *)_globals->_sceneManager._scene;

	if (!_action) {
		if (_globals->_player._position.x < 25) {
			if (!_globals->getFlag(11)) {
				scene->_sceneMode = 9106;
			} else {
				scene->_sceneMode = 9108;
				_globals->setFlag(11);
			}
		} else {
			scene->_sceneMode = 9106;
		}
		scene->setAction(&scene->_sequenceManager, scene, scene->_sceneMode, &_globals->_player, 0);
	} else {
		Scene::dispatch();
	}
}

void Scene9100::signal() {
	Scene9100 *scene = (Scene9100 *)_globals->_sceneManager._scene;

	switch (scene->_sceneMode) {
	case 9102:
	case 9106:
	case 9108:
		_globals->_sceneManager.changeScene(9150);
		break;
	case 9105:
		_sceneHotspot1.remove();
	// No break on purpose
	case 9103:
	case 9104:
	case 9107:
	case 9109:
	default:
		_globals->_player.enableControl();
		break;
	}
}

void Scene9100::postInit(SceneObjectList *OwnerList) {
	Scene9100 *scene = (Scene9100 *)_globals->_sceneManager._scene;

	Scene::postInit();
	setZoomPercents(0, 100, 200, 100);
	_object1.postInit();
	_object1.setVisage(9100);
	_object1._strip = 1;
	_object1._numFrames = 6;
	_object1.setPosition(Common::Point(297, 132), 0);
	_object1.animate(ANIM_MODE_2, 0);
	_object1.setPriority2(10);

	_globals->_player.postInit();

	_object2.postInit();
	_object2.hide();

	_object3.postInit();
	_object3.hide();

	_object4.postInit();
	_object4.hide();

	_object5.postInit();
	_object5.hide();

	if (!_globals->getFlag(23)) {
		_object6.postInit();
		_object6.setVisage(9111);
		_object6.setStrip(6);
		_object6.setFrame(1);
		_object6.setPosition(Common::Point(138, 166), 0);
		_sceneHotspot3.setup(145, 125, 166, 156, 9100, 40, 43);
	}
	_sceneHotspot1.setup(140, 176, 185, 215, 9100, 36, 37);
	_sceneHotspot2.setup(161, 138, 182, 175, 9100, 38, 39);
	_sceneHotspot4.setup(37, 196, 47, 320, 9100, 44, -1);
	_sceneHotspot5.setup(69, 36, 121, 272, 9100, 45, 46);
	_sceneHotspot6.setup(127, 0, 200, 52, 9100, 47, 48);

	_globals->_soundHandler.startSound(251, 0, 127);
	if (_globals->_sceneManager._previousScene == 9150) {
		if (_globals->getFlag(20)) {
			_globals->_player.disableControl();
			if (_globals->getFlag(11))
				_sceneMode = 9107;
			else
				_sceneMode = 9109;
			setAction(&scene->_sequenceManager, scene, _sceneMode, &_globals->_player, &_object5, 0);
		} else {
			_sceneMode = 9103;
			_globals->_player.disableControl();
			setAction(&scene->_sequenceManager, scene, _sceneMode, &_globals->_player, &_object2, &_object3, &_object4, &_object5, 0);
			_globals->setFlag(20);
		}
	} else {
		_sceneMode = 9102;
		_globals->_player.disableControl();
		setAction(&scene->_sequenceManager, scene, _sceneMode, &_globals->_player, &_object2, &_object3, &_object4, &_object5, 0);
	}
}

/*--------------------------------------------------------------------------
 * Scene 9150
 *
 *--------------------------------------------------------------------------*/
void Scene9150::Object3::signal() {
	switch (_signalFlag++) {
	case 0:
		_timer = 10 + _globals->_randomSource.getRandomNumber(90);
		break;
	default:
		animate(ANIM_MODE_5, this);
		_signalFlag = 0;
		break;
	}
}

void Scene9150::Object3::dispatch() {
	SceneObject::dispatch();
	if ((_timer != 0) && (--_timer == 0))
		signal();
}

void Scene9150::signal() {
	switch (_sceneMode) {
	case 9151:
	case 9157:
		_globals->_sceneManager.changeScene(9100);
		break;
	case 9153:
		_globals->_sceneManager.changeScene(9300);
		break;
	case 9152:
	case 9155:
	case 9156:
		_globals->_player.enableControl();
		break;
	case 9154:
	default:
		break;
	}
}

void Scene9150::dispatch() {

	if ((_sceneState != 0) && (_sceneBounds.left == 0)) {
		_object3._timer = 0;
		_sceneState = 0;
		_sceneHotspot3.setAction(&_sequenceManager2, 0, 9154, &_object3, 0);
		_sceneHotspot10.remove();
	}

	if (_action) {
		_action->dispatch();
	} else {
		if (_globals->_player._position.x >= 160) {
			if (_globals->_player._position.x > 630) {
				_globals->_player.disableControl();
				_sceneMode = 9157;
				setAction(&_sequenceManager1, this, _sceneMode, &_globals->_player, 0);
			}
		} else {
			_globals->_player.disableControl();
			if (_globals->getFlag(11)) {
				_globals->_soundHandler.startSound(286, 0, 127);
				_sceneMode = 9153;
			} else {
				_sceneMode = 9156;
			}
			setAction(&_sequenceManager1, this, _sceneMode, &_globals->_player, 0);
		}
	}
}

void Scene9150::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	setZoomPercents(0, 100, 200, 100);
	_globals->_player.postInit();

	_object3.postInit();
	_sceneState = 1;
	_object3.setVisage(9151);
	_object3._strip = 1;
	_object3._frame = 1;
	_object3.setPosition(Common::Point(312, 95), 0);
	_object3.signal();

	_sceneHotspot1.setup(0, 0, 200, 94, 9150, 46, -1);
	_sceneHotspot2.setup(51, 90, 118, 230, 9150, 47, -1);
	_sceneHotspot3.setup(182, 104, 200, 320, 9150, 48, 49);
	_sceneHotspot4.setup(103, 292, 152, 314, 9150, 50, 51);
	_sceneHotspot5.setup(115, 350, 160, 374, 9150, 52, 53);
	_sceneHotspot6.setup(0, 471, 200, 531, 9150, 54, 55);
	_sceneHotspot7.setup(170, 320, 185, 640, 9150, 56, -1);
	_sceneHotspot9.setup(157, 107, 186, 320, 9150, 56, -1);
	_sceneHotspot8.setup(133, 584, 142, 640, 9150, 57, -1);
	_sceneHotspot10.setup(83, 304, 103, 323, 9150, 58, 59);

	_globals->_soundHandler.startSound(285, 0, 127);
	_globals->_player.disableControl();

	if (_globals->getFlag(20)) {
		// Walking alone
		_globals->_scrollFollower = &_globals->_player;
		if (_globals->getFlag(11))
			// Hero wearing peasan suit
			_sceneMode = 9155;
		else
			// Hero wearing Purple suit
			_sceneMode = 9152;
		setAction(&_sequenceManager1, this, _sceneMode, &_globals->_player, 0);
	} else {
		// Walking with the tiger
		_sceneMode = 9151;
		_object2.postInit();
		_object2.hide();
		_object1.postInit();
		setAction(&_sequenceManager1, this, _sceneMode, &_globals->_player, &_object1, &_object2, 0);
	}
}

/*--------------------------------------------------------------------------
 * Scene 9200
 *
 *--------------------------------------------------------------------------*/
void Scene9200::SceneHotspot1::doAction(int action) {
	Scene9200 *scene = (Scene9200 *)_globals->_sceneManager._scene;

	if (action == OBJECT_TUNIC) {
		_globals->_player.disableControl();
		if (_globals->getFlag(93)) {
			scene->_sceneState = 9214;
			setAction(&scene->_sequenceManager, scene, 9214, &_globals->_player, &scene->_object2, 0);
		} else {
			_globals->setFlag(93);
			scene->_sceneState = 9213;
			setAction(&scene->_sequenceManager, scene, 9213, &_globals->_player, &scene->_object2, 0);
		}
	} else if (action <= 100) {
		_globals->_player.disableControl();
		scene->_sceneState = 9214;
		setAction(&scene->_sequenceManager, scene, 9214, &_globals->_player, &scene->_object2, 0);
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9200::signal() {
	switch (_sceneState++) {
	case 9207:
		_globals->_sceneManager.changeScene(9700);
		break;
	case 9208:
	case 9211:
	case 9212:
		_globals->_sceneManager.changeScene(9500);
		break;
	case 9209:
		_globals->_sceneManager.changeScene(9360);
		break;
	case 9210:
		_hotspot1.remove();
	// No break on purpose
	case 9201:
	case 9202:
	case 9203:
	case 9204:
	case 9205:
	case 9206:
	default:
		_globals->_player.enableControl();
		break;
	}
}

void Scene9200::process(Event &event) {
	Scene::process(event);
}

void Scene9200::dispatch() {
//	Rect rect9200 = Rect(320, 175, 250, 154);
	Rect rect9200 = Rect(250, 154, 320, 175);

	if (_action) {
		_action->dispatch();
	} else {
		if ( (_globals->_player._position.x <= 0) || ((_globals->_player._position.x < 100) && (_globals->_player._position.y > 199))) {
				_globals->_player.disableControl();
				_sceneState = 9209;
				setAction(&_sequenceManager, this, 9209, &_globals->_player, &_object2, &_object3, 0);
		} else {
			if (rect9200.contains(_globals->_player._position)) {
				if (_globals->getFlag(93)) {
					if (_globals->getFlag(86)) {
						_sceneState = 9215;
						setAction(&_sequenceManager, this, 9215, &_globals->_player, &_object2, &_object3, 0);
					} else {
						_sceneState = 9208;
						setAction(&_sequenceManager, this, 9208, &_globals->_player, &_object2, &_object3, 0);
					}
				} else {
					_globals->_player.disableControl();
					_sceneState = 9204;
					setAction(&_sequenceManager, this, 9204, &_globals->_player, &_object2, &_object3, 0);
				}
			} else {
				if (_globals->_player._position.y < 140) {
					_globals->_player.disableControl();
					_sceneState = 9207;
					setAction(&_sequenceManager, this, 9207, &_globals->_player, &_object2, &_object3, 0);
				}
			}
		}
	}
}

void Scene9200::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	setZoomPercents(130, 50, 200, 150);

	_globals->_player.postInit();
	_object3.postInit();
	_object3.hide();
	_object1.postInit();
	// Water animation
	_object1.setVisage(9200);
	_object1._strip = 3;
	_object1.animate(ANIM_MODE_2, 0);
	_object1.setPosition(Common::Point(132, 114), 0);
	_object1.setPriority2(140);
	_soundHandler.startSound(297, 0, 127);
	_stripManager.addSpeaker(&_speakerQText);
	_stripManager.addSpeaker(&_speakerGR);
	_stripManager.addSpeaker(&_speakerGText);

	if (!_globals->getFlag(86)) {
		_object2.postInit();
		_hotspot1.setup(96, 194, 160, 234, 9200, 29, 31);
	}
	_hotspot2.setup(164, 0, 200, 282, 9200, 0, 1);
	_hotspot3.setup(140, 39, 165, 153, 9200, 2, 3);
	_hotspot4.setup(92, 122, 139, 152, 9200, 4, 5);
	_hotspot5.setup(33, 20, 142, 115, 9200, 6, 7);
	_hotspot6.setup(104, 235, 153, 265, 9200, 8, 9);
	_hotspot7.setup(107, 262, 153, 286, 9200, 10, 11);
	_hotspot8.setup(69, 276, 164, 320, 9200, 12, 13);

	_globals->_events.setCursor(CURSOR_WALK);
	_globals->_player.disableControl();

	switch (_globals->_sceneManager._previousScene) {
	case 9500:
		if (_globals->getFlag(85)) {
			if (_globals->_inventory._helmet._sceneNumber == 1) {
				_globals->setFlag(86);
				_sceneState = 9210;
				setAction(&_sequenceManager, this, 9210, &_globals->_player, &_object2, &_object3, 0);
			} else {
				_sceneState = 9212;
				setAction(&_sequenceManager, this, 9212, &_globals->_player, &_object2, &_object3, 0);
			}
		} else {
			if (_globals->_inventory._helmet._sceneNumber == 1) {
				_sceneState = 9211;
				setAction(&_sequenceManager, this, 9211, &_globals->_player, &_object2, &_object3, 0);
			} else {
				_sceneState = 9202;
				setAction(&_sequenceManager, this, 9202, &_globals->_player, &_object2, &_object3, 0);
			}
		}
		break;
	case 9700:
		if (_globals->getFlag(86)) {
			_sceneState = 9206;
			setAction(&_sequenceManager, this, 9206, &_globals->_player, &_object2, &_object3, 0);
		} else {
			_sceneState = 9203;
			setAction(&_sequenceManager, this, 9203, &_globals->_player, &_object2, &_object3, 0);
		}
		break;
	case 9360:
	default:
		if (_globals->getFlag(86)) {
			_sceneState = 9205;
			setAction(&_sequenceManager, this, 9205, &_globals->_player, &_object2, &_object3, 0);
		} else {
			_sceneState = 9201;
			setAction(&_sequenceManager, this, 9201, &_globals->_player, &_object2, &_object3, 0);
		}
		break;
	}
}

/*--------------------------------------------------------------------------
 * Scene 9300
 *
 *--------------------------------------------------------------------------*/
void Scene9300::signal() {
	switch (_sceneMode++) {
	case 9301:
		_globals->setFlag(84);
		// No break on purpose
	case 9303:
		_globals->_soundHandler.startSound(295, 0, 127);
		_globals->_sceneManager.changeScene(9350);
		break;
	case 9302:
		_globals->_player.enableControl();
		break;
	default:
		break;
	}
}

void Scene9300::dispatch() {
	if (_action) {
		_action->dispatch();
	} else if (_globals->_player._position.y < 145) {
		_globals->_player.disableControl();
		_sceneMode = 9303;
		setAction(&_sequenceManager, this, 9303, &_globals->_player, &_object1, &_object2, 0);
	}
}

void Scene9300::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	setZoomPercents(130, 75, 230, 150);

	_sceneMode = 0;
	_globals->_player.postInit();
	_globals->_player.changeZoom(-1);
	_object1.postInit();
	_object2.postInit();
	_globals->_soundHandler.startSound(289, 0, 127);

	_hotspot1.setup(35, 142, 76, 212, 9300, 0, 1);
	_hotspot2.setup(28, 90, 81, 143, 9300, 2, 3);
	_hotspot3.setup(78, 142, 146, 216, 9300, 4, 5);
	_hotspot4.setup(3, 43, 91, 74, 9300, 6, 7);
	_hotspot5.setup(82, 19, 157, 65, 9300, 8, 9);
	_hotspot6.setup(5, 218, 84, 274, 9300, 10, 11);
	_hotspot7.setup(86, 233, 168, 293, 9300, 12, 13);
	_hotspot8.setup(157, 0, 200, 230, 9300, 14, 15);
	_hotspot9.setup(169, 227, 200, 320, 9300, 16, 17);
	_hotspot10.setup(145, 97, 166, 225, 9300, 18, 19);
	_hotspot11.setup(81, 75, 145, 145, 9300, 20, 21);
	_hotspot12.setup(0, 0, 94, 35, 9300, 22, 23);
	_hotspot13.setup(12, 268, 149, 320, 9300, 24, 25);

	if (_globals->_sceneManager._previousScene == 9350) {
		_globals->_player.disableControl();
		_sceneMode = 9302;
		setAction(&_sequenceManager, this, 9302, &_globals->_player, &_object1, &_object2, 0);
	} else {
		_globals->_player.disableControl();
		_sceneMode = 9301;
		setAction(&_sequenceManager, this, 9301, &_globals->_player, &_object1, &_object2, 0);
	}
}

/*--------------------------------------------------------------------------
 * Scene 9350
 *
 *--------------------------------------------------------------------------*/

void Scene9350::signal() {
	switch (_sceneState ++) {
	case 0:
	case 9352:
	case 9353:
	case 9354:
		_globals->_player.enableControl();
		break;
	case 9355:
		_globals->_sceneManager.changeScene(9300);
		break;
	case 9356:
		_globals->_sceneManager.changeScene(9360);
		break;
	case 9357:
	case 9359:
		_globals->_sceneManager.changeScene(9400);
		break;
	default:
		break;
	}
}

void Scene9350::dispatch() {
	if (_action == 0) {
		if ((_globals->_player._position.x > 300) && (_globals->_player._position.y < 160)) {
			_globals->_player.disableControl();
			_sceneState = 9356;
			setAction(&_sequenceManager, this, 9356, &_globals->_player, &_object2, 0);
		} else if ((_globals->_player._position.x > 110) && (_globals->_player._position.y >= 195)) {
			_globals->_player.disableControl();
			_sceneState = 9357;
			setAction(&_sequenceManager, this, 9357, &_globals->_player, &_object2, 0);
		} else if ((_globals->_player._position.x < 10) || ((_globals->_player._position.x <= 110) && (_globals->_player._position.y >= 195))) {
			_globals->_player.disableControl();
			_sceneState = 9355;
			setAction(&_sequenceManager, this, 9355, &_globals->_player, &_object2, 0);
		}
	} else {
		Scene::dispatch();
	}
}

void Scene9350::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	setZoomPercents(95, 80, 200, 100);
	_globals->_player.postInit();

	_object1.setup(9350, 1, 3, 139, 97, 0);
	_sceneHotspot1.setup(42, 0, 97, 60, 9350, 0, -1);
	_sceneHotspot2.setup(37, 205, 82, 256, 9350, 0, -1);
	_sceneHotspot3.setup(29, 93, 92, 174, 9350, 1, -1);
	_sceneHotspot4.setup(0, 308, 109, 320, 9350, 2, -1);
	_sceneHotspot5.setup(0, 0, 200, 320, 9350, 3, -1);

	_globals->_events.setCursor(CURSOR_WALK);
	_globals->_player.disableControl();

	if (_globals->_sceneManager._previousScene == 9360) {
		_globals->_player.disableControl();
		_sceneState = 9352;
		setAction(&_sequenceManager, this, 9352, &_globals->_player, &_object2, 0);
	} else if (_globals->_sceneManager._previousScene == 9400) {
		_globals->_player.disableControl();
		_sceneState = 9353;
		setAction(&_sequenceManager, this, 9353, &_globals->_player, &_object2, 0);
	} else {
		if (!_globals->getFlag(84)) {
			_globals->clearFlag(84);
			_object2.postInit();
			_globals->_player.disableControl();
			_sceneState = 9359;
			setAction(&_sequenceManager, this, 9359, &_globals->_player, &_object2, 0);
		} else {
			_globals->_player.disableControl();
			_sceneState = 9354;
			setAction(&_sequenceManager, this, 9354, &_globals->_player, &_object2, 0);
		}
	}
}

/*--------------------------------------------------------------------------
 * Scene 9360
 *
 *--------------------------------------------------------------------------*/

void Scene9360::signal() {
	switch (_sceneState ++) {
	case 0:
	case 9362:
	case 9363:
	case 9364:
		_globals->_player.enableControl();
		break;
	case 9365:
		_globals->_sceneManager.changeScene(9350);
		break;
	case 9366:
		_globals->_sceneManager.changeScene(9200);
		break;
	case 9367:
		_globals->_sceneManager.changeScene(9450);
		break;
	default:
		break;
	}
}

void Scene9360::dispatch() {
	if (_action == 0) {
		if ((_globals->_player._position.x > 300) && (_globals->_player._position.y < 160)) {
			_globals->_player.disableControl();
			_sceneState = 9366;
			setAction(&_sequenceManager, this, 9366, &_globals->_player, 0);
		} else if ((_globals->_player._position.x > 110) && (_globals->_player._position.y >= 195)) {
			_globals->_player.disableControl();
			_sceneState = 9367;
			setAction(&_sequenceManager, this, 9367, &_globals->_player, 0);
		} else if ((_globals->_player._position.x < 10) || ((_globals->_player._position.x <= 110) && (_globals->_player._position.y >= 195))) {
			_globals->_player.disableControl();
			_sceneState = 9365;
			setAction(&_sequenceManager, this, 9365, &_globals->_player, 0);
		}
	} else {
		Scene::dispatch();
	}
}

void Scene9360::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	setZoomPercents(95, 80, 200, 100);
	_globals->_player.postInit();

	_hotspot1.setup(37, 92, 93, 173, 9360, 0, 1);
	_hotspot2.setup(42, 0, 100, 63, 9360, 2, -1);
	_hotspot3.setup(36, 205, 82, 260, 9360, 3, -1);
	_hotspot4.setup(103, 2, 200, 320, 9360, 4, -1);
	_hotspot5.setup(0, 0, 37, 320, 9360, 4, -1);
	_hotspot6.setup(35, 61, 103, 92, 9360, 4, -1);
	_hotspot7.setup(33, 174, 93, 207, 9360, 4, -1);
	_hotspot8.setup(28, 257, 149, 320, 9360, 4, -1);
	_globals->_events.setCursor(CURSOR_WALK);
	_globals->_player.disableControl();
	if (_globals->_sceneManager._previousScene == 9350) {
		_globals->_player.disableControl();
		_sceneState = 9364;
		setAction(&_sequenceManager, this, 9364, &_globals->_player, 0);
	} else if (_globals->_sceneManager._previousScene == 9450) {
		_globals->_player.disableControl();
		_sceneState = 9363;
		setAction(&_sequenceManager, this, 9363, &_globals->_player, 0);
	} else {
		_globals->_player.disableControl();
		_sceneState = 9362;
		setAction(&_sequenceManager, this, 9362, &_globals->_player, 0);
	}
	_object1.setup(9351, 1, 1, 131, 90, 0);
}

/*--------------------------------------------------------------------------
 * Scene 9400
 *
 *--------------------------------------------------------------------------*/
Scene9400::Scene9400() {
	_field1032 = 0;
}

void Scene9400::SceneHotspot7::doAction(int action) {
	Scene9400 *scene = (Scene9400 *)_globals->_sceneManager._scene;

	if ((action == CURSOR_USE) && (_globals->_inventory._straw._sceneNumber != 1)) {
		scene->_sceneState = 1;
		scene->setAction(&scene->_sequenceManager, scene, 9408, &_globals->_player, 0);
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9400::SceneHotspot8::doAction(int action) {
	Scene9400 *scene = (Scene9400 *)_globals->_sceneManager._scene;

	if (action == CURSOR_TALK) {
		_globals->_player.disableControl();
		scene->_sceneState = 2;
		scene->signal();
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9400::signal() {
	switch (_sceneState ++) {
	case 0:
		_object1._numFrames = 6;
		_stripManager.start(9400, this);
		break;
	case 1:
		_object1._numFrames = 6;
		_object1.animate(ANIM_MODE_2, 0);
		_globals->_player.enableControl();
		break;
	case 2:
		_object1.animate(ANIM_MODE_5, this);
		break;
	case 3:
		_stripManager.start(9405, this);
		break;
	case 4:
		_object1.animate(ANIM_MODE_2, this);
		_globals->_player.enableControl();
		break;
	case 9350:
		_globals->_sceneManager.changeScene(9350);
		break;
	default:
		break;
	}
}

void Scene9400::dispatch() {
	if ((_object1._animateMode == 2) && (_object1._strip == 1) && (_object1._frame == 4)){
		if (_field1032 == 0) {
			_soundHandler.startSound(296, 0, 127);
			_field1032 = 1;
		}
	} else {
		_field1032 = 0;
	}
	if (_action == 0) {
		if (_globals->_player._position.y < 120) {
			_sceneState = 9350;
			_globals->_player.disableControl();
			setAction(&_action1);
			Common::Point pt(-45, 88);
			NpcMover *mover = new NpcMover();
			_globals->_player.addMover(mover, &pt, this);
		}
	} else {
		Scene::dispatch();
	}
}

void Scene9400::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	_sceneNumber = 9400;
	setZoomPercents(0, 100, 200, 100);
	_globals->_player.postInit();
	_object1.postInit(0);
	_object3.postInit(0);
	_speakerQText._textPos.x = 20;

	_hotspot7.setup(157, 66, 180, 110, 9400, 21, 23);
	_hotspot5.setup(130, 133, 152, 198, 9400, 22, -1);
	_hotspot1.setup(33, 280, 69, 297, 9400, 1, 2);
	_hotspot2.setup(73, 96, 87, 159, 9400, 3, 4);
	_hotspot3.setup(89, 253, 111, 305, 9400, 5, 6);
	_hotspot4.setup(46, 0, 116, 35, 9400, 7, 8);
	_hotspot8.setup(58, 169, 122, 200, 9400, 9, 10);
	_hotspot6.setup(0, 0, 199, 319, 9400, 16, 0);

	_stripManager.addSpeaker(&_speakerQText);
	_stripManager.addSpeaker(&_speakerOR);
	_stripManager.addSpeaker(&_speakerOText);

	_globals->_events.setCursor(CURSOR_WALK);
	_globals->_player.disableControl();

	// Useless check (skipped) : if (_globals->_sceneManager._previousScene == 9350)
	_sceneState = 2;
	if (!_globals->getFlag(89)) {
		_globals->setFlag(89);
		_sceneState = 0;
	}

	setAction(&_sequenceManager, this, 9400, &_globals->_player, &_object1, &_object3, 0);
}

/*--------------------------------------------------------------------------
 * Scene 9450
 *
 *--------------------------------------------------------------------------*/
void Scene9450::Object2::signal() {
	Scene9450 *scene = (Scene9450 *)_globals->_sceneManager._scene;

	this->setAction(&scene->_sequenceManager3, this, 9458, &scene->_object1, 0);
}

void Scene9450::Object3::dispatch() {
	SceneObject::dispatch();
	_percent = (_percent * 20) / 30;
}

void Scene9450::Hotspot1::doAction(int action) {
	Scene9450 *scene = (Scene9450 *)_globals->_sceneManager._scene;

	if (action == CURSOR_USE) {
		if (scene->_object2._action)
			scene->_object2._action->remove();
		scene->_sceneMode = 9459;
		_globals->_player.disableControl();
		setAction(&scene->_sequenceManager1, scene, 9459, &scene->_object2, &scene->_object1, &scene->_object3, &_globals->_player, 0);
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9450::Hotspot3::doAction(int action) {
	Scene9450 *scene = (Scene9450 *)_globals->_sceneManager._scene;

	switch (action) {
	case OBJECT_CLOAK:
	case OBJECT_JACKET:
	case OBJECT_TUNIC2:
		scene->_sceneMode = 9460;
		_globals->_player.disableControl();
		setAction(&scene->_sequenceManager1, scene, 9460, &_globals->_player, &scene->_object2, &scene->_object1, 0);
		break;
	case OBJECT_TUNIC:
		SceneItem::display(9450, 49, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
		break;
	case CURSOR_WALK:
		// nothing
		break;
	case CURSOR_LOOK:
		SceneItem::display(9450, 41, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
		break;
	case CURSOR_USE:
	case CURSOR_TALK:
		if (_globals->_inventory._tunic._sceneNumber == 9450) {
			if (scene->_object2._action)
				scene->_object2._action->remove();
			scene->_sceneMode = 9459;
			_globals->_player.disableControl();
			setAction(&scene->_sequenceManager1, scene, 9459, &scene->_object2, &scene->_object1, &scene->_object3, &_globals->_player, 0);
		} else if ((_globals->_inventory._cloak._sceneNumber != 1) && (_globals->_inventory._jacket._sceneNumber != 1) && (_globals->_inventory._tunic2._sceneNumber != 1)) {
			SceneItem::display(9450, 38, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
		} else {
			scene->_sceneMode = 9460;
			_globals->_player.disableControl();
			setAction(&scene->_sequenceManager1, scene, 9460, &_globals->_player, &scene->_object2, &scene->_object1, 0);
		}
		break;
	default:
		SceneItem::display(9450, 45, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
		break;
	}
}

void Scene9450::signal() {
	switch (_sceneMode++) {
	case 1002:
	case 1004:
		// Drink
		setAction(&_sequenceManager1, this, 9456, &_object2, &_object1, &_object3, 0);
		break;
	case 1005:
		// Bring me more wine
		setAction(&_sequenceManager1, this, 9457, &_object2, &_object1, &_object3, 0);
		break;
	case 9451:
		if (_globals->getFlag(87)) {
			_globals->_player.enableControl();
		} else {
			_sceneMode = 1001;
			if (_object2._action)
				_object2._action->remove();
		}
		// No break on purpose
	case 1001:
	case 1003:
		// Eat
		setAction(&_sequenceManager1, this, 9455, &_object2, &_object1, &_object3, 0);
		break;
	case 9453:
		_globals->_sceneManager.changeScene(9360);
		break;
	case 9459:
		_object2.signal();
		_globals->_events.setCursor(CURSOR_WALK);
		_hotspot1.remove();
		break;
	case 1006:
		_globals->setFlag(87);
		// No break on purpose
	default:
		_globals->_player.enableControl();
		break;
	}
}

void Scene9450::dispatch() {
	if (_action) {
		_action->dispatch();
	} else {
		if ((_globals->_player._position.y < 98) && (_globals->_player._position.x > 241) && (_globals->_player._position.x < 282)) {
			_globals->_player.disableControl();
			_sceneMode = 9452;
			setAction(&_sequenceManager1, this, 9452, &_globals->_player, 0);
		} else if ((_globals->_player._position.y < 99) && (_globals->_player._position.x > 68) && (_globals->_player._position.x < 103)) {
			_globals->_player.disableControl();
			_sceneMode = 9453;
			setAction(&_sequenceManager1, this, 9453, &_globals->_player, 0);
		}
	}
}

void Scene9450::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	setZoomPercents(84, 75, 167, 150);
	_globals->_events.setCursor(CURSOR_WALK);
	_globals->_player.postInit();

	_object2.postInit();
	_object1.postInit();
	_object1.hide();

	_globals->_player.disableControl();
	_sceneMode = 9451;
	setAction(&_sequenceManager1, this, 9451, &_globals->_player, 0);

	if (_globals->getFlag(87)) {
		if (_globals->_inventory._tunic._sceneNumber == 1) {
			_object2.signal();
		} else {
			_object2.setPosition(Common::Point(184, 144), 0);
			_object2.setVisage(9451);
			_object2.setPriority2(250);
			_object2._strip = 5;
			_object2._frame = 10;
		}
	} else {
		_object3.postInit();
		_object3.hide();
		_object3.setAction(&_sequenceManager2, 0, 9455, &_object2, &_object1, 0);
	}

	if (_globals->_inventory._tunic._sceneNumber != 1)
		_hotspot1.setup(123, 139, 138, 170, 9450, 37, -1);

	_hotspot2.setup(153, 102, 176, 141, 9450, 39, 40);
	_hotspot3.setup(97, 198, 130, 229, 9450, 41, 42);
	_hotspot15.setup(131, 190, 145, 212, 9450, 43, 44);
	_hotspot4.setup(33, 144, 105, 192, 9450, 0, 1);
	_hotspot5.setup(20, 236, 106, 287, 9450, 2, 3);
	_hotspot6.setup(137, 119, 195, 320, 9450, 4, 5);
	_hotspot7.setup(20, 59, 99, 111, 9450, 6, -1);
	_hotspot8.setup(110, 0, 199, 117, 9450, 7, 8);
	_hotspot9.setup(101, 104, 130, 174, 9450, 9, 10);
	_hotspot10.setup(110, 246, 149, 319, 9450, 11, 12);
	_hotspot11.setup(16, 34, 74, 62, 6450, 13, 14);
	_hotspot12.setup(19, 108, 72, 134, 9450, 15, 16);
	_hotspot13.setup(18, 215, 71, 237, 9450, 17, 18);
	_hotspot14.setup(15, 288, 76, 314, 9450, 19, 20);
	_hotspot16.setup(0, 0, 200, 320, 9450, 46, -1);
}

/*--------------------------------------------------------------------------
 * Scene 9500
 *
 *--------------------------------------------------------------------------*/
void Scene9500::Hotspot1::doAction(int action) {
	Scene9500 *scene = (Scene9500 *)_globals->_sceneManager._scene;

	if (action == OBJECT_SWORD) {
		scene->_sceneMode = 9510;
		_globals->setFlag(92);
		_globals->_inventory._sword._sceneNumber = 9500;
		_globals->_player.disableControl();
		_globals->_sceneItems.remove(this);
		scene->_hotspot2.setup(87, 294, 104, 314, 9400, 17, -1);
		scene->setAction(&scene->_sequenceManager, scene, 9510, &_globals->_player, &scene->_object2, 0);
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9500::Hotspot2::doAction(int action) {
	Scene9500 *scene = (Scene9500 *)_globals->_sceneManager._scene;

	if (action == CURSOR_USE) {
		scene->_sceneMode = 9511;
		_globals->_player.disableControl();
		_globals->_sceneItems.remove(this);
		scene->setAction(&scene->_sequenceManager, scene, 9511, &_globals->_player, &scene->_object2, 0);
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9500::Hotspot3::doAction(int action) {
	Scene9500 *scene = (Scene9500 *)_globals->_sceneManager._scene;

	if ((action == CURSOR_USE) && (_globals->_inventory._candle._sceneNumber != 1)){
		scene->_sceneMode = 9505;
		_globals->_player.disableControl();
		_globals->_sceneItems.remove(this);
		scene->setAction(&scene->_sequenceManager, scene, 9505, &_globals->_player, &scene->_candle, 0);
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9500::Hotspot4::doAction(int action) {
	Scene9500 *scene = (Scene9500 *)_globals->_sceneManager._scene;

	if (action == OBJECT_CANDLE) {
		_globals->_player.disableControl();
		if (_globals->_inventory._straw._sceneNumber == 9500) {
			scene->_sceneMode = 9506;
			_globals->_sceneItems.remove(&scene->_hotspot5);
			_globals->_sceneItems.remove(this);
			scene->setAction(&scene->_sequenceManager, scene, 9506, &_globals->_player, &scene->_object3, 0);
			_globals->_inventory._candle._sceneNumber = 9850;
		} else {
			scene->_sceneMode = 9507;
			scene->setAction(&scene->_sequenceManager, scene, 9507, &_globals->_player, &scene->_object3, 0);
		}
	} else if (action == OBJECT_STRAW) {
		scene->_sceneMode = 9512;
		_globals->_player.disableControl();
		_globals->_inventory._straw._sceneNumber = 9500;
		scene->setAction(&scene->_sequenceManager, scene, 9512, &_globals->_player, &scene->_object3, 0);
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9500::signal() {
	switch (_sceneMode) {
	case 9503:
		_globals->_sceneManager.changeScene(9200);
		_globals->_soundHandler.startSound(295, 0, 127);
		break;
	case 9504:
		_globals->_sceneManager.changeScene(9850);
		break;
	case 9505:
		_candle.setStrip(2);
		_globals->_player.enableControl();
		break;
	case 9506:
		_globals->setFlag(85);
		_globals->_player.enableControl();
		break;
	case 9511:
		_globals->_player.enableControl();
		if (!_globals->getFlag(51)) {
			_globals->setFlag(51);
			_globals->_player.disableControl();
			_sceneMode = 9514;
			setAction(&_sequenceManager, this, 9514, &_globals->_player, 0, 0, 0, 0);
		}
		break;
	case 0:
	case 9514:
	default:
		_globals->_player.enableControl();
		break;
	}
}

void Scene9500::dispatch() {
	if (_action) {
		_action->dispatch();
	} else {
		if (_globals->_player._position.y >= 199) {
			_globals->_player.disableControl();
			_sceneMode = 9503;
			setAction(&_sequenceManager, this, 9503, &_globals->_player, 0, 0, 0, 0);
		} else if (_globals->_player._position.y < 127) {
			_globals->_player.disableControl();
			_sceneMode = 9504;
			setAction(&_sequenceManager, this, 9504, &_globals->_player, 0, 0, 0, 0);
		}
	}

}

void Scene9500::process(Event &event) {
	Scene::process(event);
}

void Scene9500::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	setZoomPercents(110, 75, 200, 150);

	_globals->_player.postInit();
	_globals->_soundHandler.startSound(305, 0, 127);

	_candle.postInit(0);
	_candle.setVisage(9500);
	_candle.setStrip(1);
	_candle.animate(ANIM_MODE_2);
	_candle.setPosition(Common::Point(30, 105), 0);
	if (_globals->_inventory._candle._sceneNumber != 9500)
		_candle.setStrip(2);

	_object3.postInit(0);
	_object3.hide();
	_object3.setPriority2(150);
	_object3.setPosition(Common::Point(166, 133));
	if (_globals->_inventory._straw._sceneNumber == 9500) {
		_object3.show();
		_object3.setVisage(5);
		_object3._strip = 2;
		_object3._frame = 9;
		_object3.setPosition(Common::Point(168, 128));
		if (_globals->getFlag(85)) {
			_object3.setVisage(9500);
			_object3.setStrip(4);
			_object3.animate(ANIM_MODE_8, 0, 0);
			_object3.setPosition(Common::Point(166, 133));
		}
	}

	_object2.postInit(0);
	_object2.hide();
	if (_globals->getFlag(92)) {
		_object2.show();
		_object2.setVisage(9501);
		_object2.setStrip(1);
		_object2.setFrame(_object2.getFrameCount());
		_object2.setPosition(Common::Point(303, 130));
		_object2.setPriority2(132);
		if (_globals->_inventory._helmet._sceneNumber == 1) {
			_hotspot2.setup(87, 294, 104, 314, 9400, 17, -1);
		} else {
			_object2.setStrip(2);
			_object2.setFrame(1);
		}
	} else {
		_hotspot1.setup(105, 295, 134, 313, 9500, 9, 10);
	}

	_hotspot17.setup(101, 293, 135, 315, 9500, 9, 10);
	_hotspot3.setup(84, 12, 107, 47, 9500, 15, 15);
	_hotspot6.setup(93, 11, 167, 46, 9500, 0, 1);
	_hotspot7.setup(100, 70, 125, 139, 9500, 2, 3);

	if (!_globals->getFlag(85)) {
		_hotspot5.setup(111, 68, 155, 244, 9500, 17, -1);
		_hotspot4.setup(57, 71, 120, 126, 9500, 16, -1);
	}

	_hotspot8.setup(60, 24, 90, 53, 9500, 4, 5);
	_hotspot9.setup(72, 143, 93, 163, 9500, 4, 5);
	_hotspot10.setup(70, 205, 92, 228, 9500, 4, 5);
	_hotspot11.setup(66, 291, 90, 317, 9500, 4, 5);
	_hotspot12.setup(22, 58, 101, 145, 9500, 6, 7);
	_hotspot13.setup(121, 57, 163, 249, 9500, 6, 7);
	_hotspot14.setup(115, 133, 135, 252, 9500, 6, 7);
	_hotspot15.setup(55, 240, 125, 254, 9500, 6, 7);
	_hotspot16.setup(53, 251, 132, 288, 9500, 8, -1);
	_hotspot19.setup(101, 207, 120, 225, 9500, 9, 10);
	_hotspot18.setup(98, 144, 117, 162, 9500, 9, 10);
	_hotspot20.setup(102, 27, 132, 50, 9500, 9, 10);

	_globals->_events.setCursor(CURSOR_WALK);
	_globals->_player.disableControl();

	if ((_globals->_sceneManager._previousScene == 9200) || (_globals->_sceneManager._previousScene != 9850)) {
		_sceneMode = 0;
		if (_globals->_inventory._helmet._sceneNumber != 1) {
			setAction(&_sequenceManager, this, 9501, &_globals->_player, &_candle, 0);
		} else {
			_globals->_inventory._helmet._sceneNumber = 9500;
			_hotspot2.setup(87, 294, 104, 314, 9400, 17, -1);
			setAction(&_sequenceManager, this, 9513, &_globals->_player, &_object2, 0);
		}
	} else {
		_sceneMode = 0;
		setAction(&_sequenceManager, this, 9502, &_globals->_player, &_candle, 0);
	}
}

/*--------------------------------------------------------------------------
 * Scene 9700
 *
 *--------------------------------------------------------------------------*/
void Scene9700::signal() {
	switch (_sceneMode ++) {
	case 9703:
		_globals->setFlag(88);
		// No break on purpose
	case 9701:
	case 9702:
		_gfxButton1.setText(EXIT_MSG);
		_gfxButton1._bounds.centre(50, 190);
		_gfxButton1.draw();
		_gfxButton1._bounds.expandPanes();
		_globals->_player.enableControl();
		_globals->_player._canWalk = 0;
		_globals->_events.setCursor(CURSOR_USE);
		break;
	case 9704:
		_globals->_soundHandler.startSound(323, 0, 127);
		_globals->_sceneManager.changeScene(9750);
		break;
	}
}

void Scene9700::process(Event &event) {
	if ((event.eventType == EVENT_BUTTON_DOWN) && (event.kbd.keycode == 0)) {
		if (_gfxButton1.process(event)) {
			_globals->_sceneManager.changeScene(9200);
		} else if (_globals->_events._currentCursor == OBJECT_SCANNER) {
			event.handled = true;
			if (_globals->_inventory._helmet._sceneNumber == 1) {
				_globals->_player.disableControl();
				_sceneMode = 9704;
				setAction(&_sequenceManager, this, 9704, &_globals->_player, &_object1, 0);
			} else {
				_globals->_player.disableControl();
				_sceneMode = 9703;
				setAction(&_sequenceManager, this, 9703, &_globals->_player, &_object1, 0);
			}
		}
	}
}

void Scene9700::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	setZoomPercents(0, 100, 200, 100);

	_sceneHotspot1.setup(84, 218, 151, 278, 9700, 14, -1);
	_sceneHotspot2.setup(89, 11, 151, 121, 9700, 14, -1);
	_sceneHotspot3.setup(69, 119, 138, 218, 9700, 15, 16);
	_sceneHotspot4.setup(34, 13, 88, 116, 9700, 17, -1);
	_sceneHotspot5.setup(52, 119, 68, 204, 9700, 17, -1);
	_sceneHotspot6.setup(0, 22, 56, 275, 9700, 18, -1);

	_object1.postInit();
	_object1.hide();
	_globals->_player.postInit();
	if (_globals->getFlag(97)) {
		_globals->_player.disableControl();
		_sceneMode = 9701;
		setAction(&_sequenceManager, this, 9701, &_globals->_player, &_object1, 0);
		_globals->setFlag(97);
	} else {
		_globals->_player.disableControl();
		_sceneMode = 9702;
		setAction(&_sequenceManager, this, 9702, &_globals->_player, &_object1, 0);
	}
}

/*--------------------------------------------------------------------------
 * Scene 9750
 *
 *--------------------------------------------------------------------------*/
void Scene9750::signal() {
	switch (_sceneMode ++) {
	case 9751:
		_globals->_soundHandler.proc1(this);
		break;
	case 9752:
		_globals->_sceneManager.changeScene(2100);
	default:
		break;
	}
}

void Scene9750::dispatch() {
	Scene::dispatch();
}

void Scene9750::postInit(SceneObjectList *OwnerList) {
	loadScene(9750);
	Scene::postInit();
	setZoomPercents(0, 100, 200, 100);

	_globals->_player.postInit();
	_object1.postInit();
	_object1.hide();
	_object2.postInit();
	_object2.hide();
	_globals->_player.disableControl();
	_sceneMode = 9751;
	setAction(&_sequenceManager, this, 9751, &_globals->_player, &_object1, &_object2, 0);
}


/*--------------------------------------------------------------------------
 * Scene 9850
 *
 *--------------------------------------------------------------------------*/
void Scene9850::Object6::doAction(int action) {
	if ((_flags & OBJFLAG_HIDE) == 0) {
		if (action == CURSOR_LOOK) {
			SceneItem::display(9850, 27, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
		} else if (action == CURSOR_USE) {
			_globals->_inventory._scimitar._sceneNumber = 1;
			hide();
		} else {
			SceneHotspot::doAction(action);
		}
	}
}
void Scene9850::Object7::doAction(int action) {
	if ((_flags & OBJFLAG_HIDE) == 0) {
		if (action == CURSOR_LOOK) {
			SceneItem::display(9850, 28, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
		} else if (action == CURSOR_USE) {
			_globals->_inventory._sword._sceneNumber = 1;
			hide();
		} else {
			SceneHotspot::doAction(action);
		}
	}
}

// Hair covered tunic
void Scene9850::Hotspot12::doAction(int action) {
	Scene9850 *scene = (Scene9850 *)_globals->_sceneManager._scene;

	if (action == CURSOR_USE) {
		if (_globals->_inventory._tunic2._sceneNumber != 1) {
			_globals->_inventory._tunic2._sceneNumber = 1;
			_globals->_player.disableControl();
			scene->_sceneMode = 9858;
			setAction(&scene->_sequenceManager, scene, 9858, &_globals->_player, &scene->_objTunic2, 0);
		} else {
			_globals->_inventory._tunic2._sceneNumber = 9850;
			_globals->_player.disableControl();
			scene->_sceneMode = 9861;
			setAction(&scene->_sequenceManager, scene, 9861, &_globals->_player, &scene->_objTunic2, 0);
		}
	} else if ((action != CURSOR_LOOK) || (_globals->_inventory._tunic2._sceneNumber != 1)) {
		NamedHotspot::doAction(action);
	} else {
		SceneItem::display(9850, 30, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
	}
}

void Scene9850::Hotspot14::doAction(int action) {
	Scene9850 *scene = (Scene9850 *)_globals->_sceneManager._scene;

	if (action == CURSOR_USE) {
		if (_globals->_inventory._jacket._sceneNumber != 1) {
			_globals->_inventory._jacket._sceneNumber = 1;
			_globals->_player.disableControl();
			scene->_sceneMode = 9857;
			setAction(&scene->_sequenceManager, scene, 9857, &_globals->_player, &scene->_objJacket, 0);
		} else {
			_globals->_inventory._jacket._sceneNumber = 9850;
			_globals->_player.disableControl();
			scene->_sceneMode = 9860;
			setAction(&scene->_sequenceManager, scene, 9860, &_globals->_player, &scene->_objJacket, 0);
		}
	} else if ((action != CURSOR_LOOK) || (_globals->_inventory._jacket._sceneNumber != 1)) {
		NamedHotspot::doAction(action);
	} else {
		SceneItem::display(9850, 30, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
	}
}

void Scene9850::Hotspot16::doAction(int action) {
	Scene9850 *scene = (Scene9850 *)_globals->_sceneManager._scene;

	if (action == CURSOR_USE) {
		if (_globals->_inventory._cloak._sceneNumber != 1) {
			_globals->_inventory._cloak._sceneNumber = 1;
			_globals->_player.disableControl();
			scene->_sceneMode = 9862;
			setAction(&scene->_sequenceManager, scene, 9862, &_globals->_player, &scene->_objCloak, 0);
		} else {
			_globals->_inventory._cloak._sceneNumber = 9850;
			_globals->_player.disableControl();
			scene->_sceneMode = 9859;
			setAction(&scene->_sequenceManager, scene, 9859, &_globals->_player, &scene->_objCloak, 0);
		}
	} else if ((action != CURSOR_LOOK) || (_globals->_inventory._cloak._sceneNumber != 1)) {
		NamedHotspot::doAction(action);
	} else {
		SceneItem::display(9850, 30, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
	}
}

void Scene9850::Hotspot17::doAction(int action) {
	Scene9850 *scene = (Scene9850 *)_globals->_sceneManager._scene;

	if (action == OBJECT_SCANNER) {
		SceneItem::display(9850, 32, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
	} else {
		if (action == CURSOR_USE)
			scene->_soundHandler.startSound(306, 0, 127);
		NamedHotspot::doAction(action);
	}
}

void Scene9850::Hotspot18::doAction(int action) {
	Scene9850 *scene = (Scene9850 *)_globals->_sceneManager._scene;

	if (action == OBJECT_SCANNER) {
		SceneItem::display(9850, 32, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
	} else {
		if (action == CURSOR_USE)
			scene->_soundHandler.startSound(306, 0, 127);
		NamedHotspot::doAction(action);
	}
}

void Scene9850::Hotspot19::doAction(int action) {
	Scene9850 *scene = (Scene9850 *)_globals->_sceneManager._scene;

	if (action == OBJECT_SCANNER) {
		SceneItem::display(9850, 31, SET_Y, 20, SET_WIDTH, 200, SET_EXT_BGCOLOUR, 7, LIST_END);
	} else {
		if (action == CURSOR_USE)
			scene->_soundHandler.startSound(313, 0, 127);
		NamedHotspot::doAction(action);
	}
}

// Arrow on Statue
void Scene9850::Hotspot20::doAction(int action) {
	Scene9850 *scene = (Scene9850 *)_globals->_sceneManager._scene;

	if (action == CURSOR_USE) {
		_globals->_player.disableControl();
		if (scene->_objSword._state == 0) {
			if (_globals->_inventory._scimitar._sceneNumber == 9850)
				scene->_objScimitar.show();
			if (_globals->_inventory._sword._sceneNumber == 9850)
				scene->_objSword.show();
			scene->_sceneMode = 11;
			setAction(&scene->_sequenceManager, scene, 9853, &_globals->_player, &scene->_objDoor, &scene->_objLever, 0);
		} else {
			scene->_sceneMode = 10;
			setAction(&scene->_sequenceManager, scene, 9854, &_globals->_player, &scene->_objDoor, &scene->_objLever, 0);
		}
		scene->_objSword._state ^= 1;
	} else {
		NamedHotspot::doAction(action);
	}
}

void Scene9850::signal() {
	switch (_sceneMode ++) {
	case 10:
		// Hidden closet closed
		if (_globals->_inventory._scimitar._sceneNumber == 9850)
			_objScimitar.hide();
		if (_globals->_inventory._sword._sceneNumber == 9850)
			_objSword.hide();
		_globals->_sceneItems.remove(&_objScimitar);
		_globals->_sceneItems.remove(&_objSword);
		_globals->_sceneItems.addItems(&_hotspot19, NULL);
		_globals->_player.enableControl();
		break;
	case 11:
		// Hidden closet opened
		if (_globals->_inventory._scimitar._sceneNumber == 9850)
			_globals->_sceneItems.addItems(&_objScimitar, NULL);
		if (_globals->_inventory._sword._sceneNumber == 9850)
			_globals->_sceneItems.addItems(&_objSword, NULL);
		_globals->_sceneItems.remove(&_hotspot19);
		_globals->_player.enableControl();
		break;
	case 9500:
		_globals->_sceneManager.changeScene(9500);
		break;
	case 0:
	default:
		_globals->_player.enableControl();
		break;
	}
}

void Scene9850::process(Event &event) {
	Scene::process(event);
	if ((event.eventType == EVENT_KEYPRESS) && (event.kbd.keycode == Common::KEYCODE_4)) {
		event.handled = true;
		_globals->_player.disableControl();
		if (_objSword._state == 0) {
			_sceneMode = 0;
			setAction(&_sequenceManager, this, 9853, &_objLever, &_objDoor, &_objScimitar, &_objSword, 0);
		} else {
			_sceneMode = 10;
			setAction(&_sequenceManager, this, 9854, &_objLever, &_objDoor, &_objScimitar, &_objSword, 0);
		}
		_objSword._state ^= 1;
	}
}

void Scene9850::dispatch() {
	if (_action) {
		_action->dispatch();
	} else if (_globals->_player._position.y >= 198) {
		_globals->_player.disableControl();
		_sceneMode = 9500;
		setAction(&_sequenceManager, this, 9852, &_globals->_player, 0);
	}
}

void Scene9850::postInit(SceneObjectList *OwnerList) {
	Scene::postInit();
	_objSword._state = 0;

	_objDoor.postInit();
	_objDoor.setVisage(9850);
	_objDoor.setStrip(1);
	_objDoor.setFrame(1);
	_objDoor.setPosition(Common::Point(28, 118), 0);
	_objDoor.setPriority2(90);

	_objLever.postInit();
	_objLever.setVisage(9850);
	_objLever.setStrip(4);
	_objLever.setFrame(1);
	_objLever.setPosition(Common::Point(256, 35), 0);

	_objCloak.postInit();
	_objCloak.setVisage(9850);
	_objCloak.setStrip(5);
	_objCloak.setFrame(1);
	_objCloak.setPriority2(90);
	_objCloak.setPosition(Common::Point(157, 81), 0);
	if (_globals->_inventory._cloak._sceneNumber != 9850)
		_objCloak.hide();

	_objJacket.postInit();
	_objJacket.setVisage(9850);
	_objJacket.setStrip(5);
	_objJacket.setFrame(2);
	_objJacket.setPriority2(90);
	_objJacket.setPosition(Common::Point(201, 84));
	if (_globals->_inventory._jacket._sceneNumber != 9850)
		_objJacket.hide();

	_objTunic2.postInit();
	_objTunic2.setVisage(9850);
	_objTunic2.setStrip(5);
	_objTunic2.setFrame(3);
	_objTunic2.setPriority2(90);
	_objTunic2.setPosition(Common::Point(295, 90));
	if (_globals->_inventory._tunic2._sceneNumber != 9850)
		_objTunic2.hide();

	if (_globals->_inventory._scimitar._sceneNumber == 9850) {
		_objScimitar.postInit();
		_objScimitar.setVisage(9850);
		_objScimitar.setStrip(2);
		_objScimitar.setFrame(1);
		_objScimitar.setPosition(Common::Point(55, 83), 0);
		_objScimitar.setPriority2(80);
		_objScimitar.hide();
	}

	if (_globals->_inventory._sword._sceneNumber == 9850) {
		_objSword.postInit();
		_objSword.setVisage(9850);
		_objSword.setStrip(3);
		_objSword.setFrame(1);
		_objSword.setPosition(Common::Point(56, 101), 0);
		_objSword.setPriority2(80);
		_objSword.hide();
	}

	_spotLever.setup(30, 251, 45, 270, 9850, 26, -1);
	_hotspot1.setup(123, 0, 200, 320, 9850, 0, 1);
	_hotspot2.setup(107, 87, 133, 308, 9850, 0, 1);
	_hotspot3.setup(2, 28, 53, 80, 9850, 2, 3);
	_hotspot4.setup(13, 0, 55, 27, 9850, 2, 3);
	_hotspot5.setup(8, 74, 27, 91, 9850, 4, 5);
	_hotspot17.setup(61, 0, 125, 28, 9850, 6, 7);
	_hotspot18.setup(51, 95, 105, 145, 9850, 6, 7);
	_hotspot19.setup(56, 28, 115, 97, 9850, 6, 8);
	_hotspot6.setup(0, 223, 115, 257, 9850, 9, 10);
	_hotspot7.setup(15, 254, 33, 268, 9850, 9, -1);
	_hotspot8.setup(17, 218, 37, 233, 9850, 9, 10);
	_hotspot9.setup(8, 113, 26, 221, 9850, 11, 12);
	_hotspot10.setup(14, 94, 53, 112, 9850, 13, 14);
	_hotspot11.setup(5, 269, 29, 303, 9850, 15, 16);
	_hotspot12.setup(43, 278, 91, 317, 9850, 17, 18);
	_hotspot13.setup(47, 263, 112, 282, 9850, 19, 20);
	_hotspot14.setup(43, 188, 86, 224, 9850, 21, 22);
	_hotspot15.setup(43, 162, 92, 191, 9850, 23, 24);
	_hotspot16.setup(40, 146, 90, 169, 9850, 25, -1);

	_globals->_player.postInit();
	_globals->_player.disableControl();
	_sceneMode = 0;
	setAction(&_sequenceManager, this, 9851, &_globals->_player, 0);
}

/*--------------------------------------------------------------------------
 * Scene 9900
 *
 *--------------------------------------------------------------------------*/
void Scene9900::strAction1::signal() {
	RGB8 mask1, mask2;
	mask1.r = mask1.g = mask1.b = 0xff;
	mask2.r = mask2.g = mask2.b = 0;

	Scene9900 *scene = (Scene9900 *)_globals->_sceneManager._scene;

	switch (_actionIndex++) {
	case 0:
		scene->_soundHandler.startSound(351, 0, 127);
		_object9.postInit();
		_object9.setVisage(18);
		_object9._frame = 1;
		_object9._strip = 6;
		_object9.setPosition(Common::Point(171, 59));
		_object9.animate(ANIM_MODE_5, 0);
		_globals->_scenePalette.addRotation(67, 111, 1, 1, this);
		scene->_object2.hide();
		break;
	case 1:
		_palette1.getPalette();
		_globals->_scenePalette.addFader(&mask1, 1, 10, this);
		break;
	case 2:
		_object9.remove();
		_globals->_scenePalette.addFader(&mask2, 1, 5, this);
		break;
	case 3:
		_globals->_soundHandler.startSound(377, 0, 127);
		setDelay(120);
		break;
	case 4:
		_globals->_scenePalette.addFader(_palette1._palette, 256, 1, this);
		break;
	case 5:
		remove();
		break;
	default:
		break;
	}
}

void Scene9900::strAction2::signal() {
	switch (_actionIndex++) {
	case 0:
		_lineNum = 0;
		_txtArray1Index = 0;
		_txtArray1[0]._position.y = 200;
		_txtArray1[0]._position.y = 300;
		_txtArray2[0]._position.y = 400;
		_txtArray2[0]._position.y = 500;
		_var3 = 0;
		// No break on purpose
	case 1: {
		Common::String msg = _vm->_dataManager->getMessage(8030, _lineNum++);
		if (!msg.compareTo("LASTCREDIT")) {
			if (_var3 == 0) {
				// Not used?
				// int x = _txtArray1[_txtArray1Index].getFrame().getBounds().height();
				_txtArray1[_txtArray1Index]._moveDiff.y = 10;

				NpcMover *mover = new NpcMover();
				Common::Point pt(_txtArray1[_txtArray1Index]._moveDiff.x, -100);
				_txtArray1[_txtArray1Index].addMover(mover, &pt, 0);

				// Not used?
				// int x = _txtArray2[_txtArray1Index].getFrame().getBounds().height();
				_txtArray2[_txtArray1Index]._moveDiff.y = 10;
				_txtArray1Index = (_txtArray1Index + 1) % 2;
			}
			_var3 = 1;
			_txtArray1[_txtArray1Index]._textMode = ALIGN_CENTRE;
			_txtArray1[_txtArray1Index]._width = 240;
			_txtArray1[_txtArray1Index]._fontNumber = 2;
			_txtArray1[_txtArray1Index]._colour1 = 7;
			_txtArray1[_txtArray1Index].setup(msg);
			_txtArray1[_txtArray1Index]._field7A = 20;
			_txtArray1[_txtArray1Index]._moveDiff.y = 2;
			_txtArray1[_txtArray1Index].setPriority2(255);
			int frameWidth = _txtArray1[_txtArray1Index].getFrame().getBounds().width();
			int frameHeight = _txtArray1[_txtArray1Index].getFrame().getBounds().height();
			_txtArray1[_txtArray1Index].setPosition(Common::Point((320 - frameWidth) / 2, 200));
			NpcMover *mover2 = new NpcMover();
			Common::Point pt2(_txtArray1[_txtArray1Index]._position.x, 100);
			_txtArray1[_txtArray1Index].addMover(mover2, &pt2, 0);

			_txtArray2[_txtArray1Index]._textMode = ALIGN_CENTRE;
			_txtArray2[_txtArray1Index]._width = 240;
			_txtArray2[_txtArray1Index]._fontNumber = 2;
			_txtArray2[_txtArray1Index]._colour1 = 23;

			msg = _vm->_dataManager->getMessage(8030, _lineNum++);
			_txtArray2[_txtArray1Index].setup(msg);
			_txtArray2[_txtArray1Index]._field7A = 20;
			_txtArray2[_txtArray1Index]._moveDiff.y = 2;
			_txtArray2[_txtArray1Index].setPriority2(255);
			frameWidth = _txtArray2[_txtArray1Index].getFrame().getBounds().width();
			_txtArray2[_txtArray1Index].setPosition(Common::Point((320 - frameWidth) / 2, 200 + frameHeight));
		} else {
			_globals->_player.enableControl();
			_actionIndex = 3;
			signal();
		}
		break;
	}
	case 2:
		setDelay(60);
		_actionIndex = 1;
		break;
	case 3:
		setDelay(7200);
		break;
	case 4:
		_txtArray1[0].remove();
		_txtArray1[1].remove();
		_txtArray2[0].remove();
		_txtArray2[1].remove();
		remove();
		break;
	default:
		break;
	}
}
void Scene9900::strAction2::dispatch() {
//	if (this->_txtArray1[0]._textSurface != 0) {
		int frameHeight = _txtArray1[0].getFrame().getBounds().height();
		_txtArray2[0]._position.y = frameHeight + _txtArray1[0]._position.y;
		_txtArray2[0]._flags |= OBJFLAG_PANES;
//	}
//	if (this->_txtArray1[1]._textSurface != 0) {
		frameHeight = _txtArray1[1].getFrame().getBounds().height();
		_txtArray2[1]._position.y = frameHeight + _txtArray1[1]._position.y;
		_txtArray2[1]._flags |= OBJFLAG_PANES;
//	}
	Action::dispatch();
}

void Scene9900::strAction3::signal() {
	RGB8 mask3, mask4;
	mask3.r = 0xff; mask3.g = mask3.b = 0;
	mask4.r = mask4.g = mask4.b = 0;

	switch (_actionIndex++) {
	case 0:
		_palette2.getPalette();
		_palette3.loadPalette(2003);
		_globals->_scenePalette.addFader(_palette3._palette, 256, 5, this);
		break;
	case 1:
		_globals->_scenePalette.addFader(&mask3, 1, 10, this);
		break;
	case 2:
		_globals->_scenePalette.addFader(&mask4, 1, 1, this);
		break;
	case 3:
		_palette2.loadPalette(17);
		_globals->_sceneManager._scene->loadScene(17);
		_globals->_scenePalette.addFader(_palette2._palette, 256, 5, this);
		break;
	case 4:
		_globals->_game.endGame(9900, 61);
		remove();
	default:
		break;
	}
}

void Scene9900::signal() {
	if ((_sceneMode != 9913) && (_sceneMode != 9905) && (_sceneMode != 9904) && (_sceneMode != 9912)) {
		_object1.hide();
		_object2.hide();
		_object3.hide();
		_object4.hide();
		_object5.hide();
		_object6.hide();
	}

	_object1.animate(ANIM_MODE_NONE, 0);
	_object2.animate(ANIM_MODE_NONE, 0);
	_object3.animate(ANIM_MODE_NONE, 0);
	_object4.animate(ANIM_MODE_NONE, 0);
	_object5.animate(ANIM_MODE_NONE, 0);
	_object6.animate(ANIM_MODE_NONE, 0);

	_object1.setObjectWrapper(0);
	_object2.setObjectWrapper(0);
	_object3.setObjectWrapper(0);
	_object4.setObjectWrapper(0);
	_object5.setObjectWrapper(0);
	_object6.setObjectWrapper(0);

	_object1.addMover(0);
	_object2.addMover(0);
	_object3.addMover(0);
	_object4.addMover(0);
	_object5.addMover(0);
	_object6.addMover(0);

	switch (_sceneMode){
	case 150:
		_globals->_soundHandler.startSound(380, 0, 127);
		_object8.postInit(0);
		_object8.setVisage(2002);
		_object8.setStrip(1);
		_object8.setFrame(1);
		_object8.setPriority2(200);
		_object8.setPosition(Common::Point(64, 199));
		_globals->_player.disableControl();
		_sceneMode = 9908;
		setAction(&_sequenceManager, this, 9908, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		break;
	case 162:
		warning("TBC: shutdown();");
		_globals->_game.quitGame();
		break;
	case 9901:
		_globals->_player.disableControl();
		_sceneMode = 9906;
		setAction(&_sequenceManager, this, 9906, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		_globals->_player._uiEnabled = true;
		_globals->_events.setCursor(CURSOR_USE);
		break;
	case 9902:
		_globals->_player.disableControl();
		_sceneMode = 9901;
		setAction(&_sequenceManager, this, 9901, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		break;
	case 9903:
		_globals->_player.disableControl();
		_sceneMode = 9902;
		setAction(&_sequenceManager, this, 9902, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		break;
	case 9904:
		_globals->_soundHandler.startSound(390, 0, 127);
		_sceneMode = 9912;
		setAction(&_strAction2, this);
		break;
	case 9905:
		_sceneMode = 150;
		setAction(&_strAction1, this);
		break;
	case 9906:
		if (_object8._state == 0) {
			_globals->_player.disableControl();
			_sceneMode = 9913;
			setAction(&_sequenceManager, this, 9913, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		} else {
			_globals->_player.disableControl();
			_sceneMode = 9905;
			setAction(&_sequenceManager, this, 9905, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		}
		break;
	case 9907:
		_globals->_player.disableControl();
		_sceneMode = 9903;
		setAction(&_sequenceManager, this, 9903, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		break;
	case 9908:
		_object8.remove();
		_globals->_player.disableControl();
		_sceneMode = 9904;
		setAction(&_sequenceManager, this, 9904, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		break;
	case 9909:
		_globals->_soundHandler.startSound(375, 0, 127);
		_globals->_player.disableControl();
		_sceneMode = 9907;
		setAction(&_sequenceManager, this, 9907, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		break;
	case 9910:
		_globals->_player.disableControl();
		_sceneMode = 9911;
		setAction(&_sequenceManager, this, 9911, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		break;
	case 9911:
		_globals->_soundHandler.startSound(367, 0, 127);
		_globals->_player.disableControl();
		_sceneMode = 9909;
		setAction(&_sequenceManager, this, 9909, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		break;
	case 9912:
		_globals->_player.disableControl();
		_sceneMode = 9912;
		setAction(&_sequenceManager, this, 9912, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
		_sceneMode = 162;
		_globals->_player.enableControl();
		_globals->_player._canWalk = false;
		break;
	case 9913:
		_sceneMode = 200;
		setAction(&_strAction3, this);
		break;
	default:
		break;
	}
}

void Scene9900::process(Event &event) {
	if (event.handled)
		return;
	Scene::process(event);
	if (_sceneMode != 9906) {
		if ((event.eventType == EVENT_BUTTON_DOWN) && (_globals->_events.getCursor() == OBJECT_ITEMS)) {
			_object8._state = 1;
			_globals->_inventory._items._sceneNumber = 9900;
			_globals->_events.setCursor(CURSOR_USE);
		}
	}
}

void Scene9900::dispatch() {
	if (_action)
		_action->dispatch();
}

void Scene9900::postInit(SceneObjectList *OwnerList) {
	_object1.postInit(0);
	_object1.hide();
	_object2.postInit(0);
	_object2.hide();
	_object3.postInit(0);
	_object3.hide();
	_object4.postInit(0);
	_object4.hide();
	_object5.postInit(0);
	_object5.hide();
	_object6.postInit(0);
	_object6.hide();

	_object8._state = 0;

	_globals->_inventory._concentrator._sceneNumber = 9900;
	_globals->_inventory._items._rlbNum = 3;
	_globals->_inventory._items._cursorNum = 6;
	_globals->_inventory._items._description = Common::String("One of the items from the stasis ship. The other is on the Lance's bridge.");

	_stripManager.addSpeaker(&_speakerMR);
	_globals->_player.disableControl();
	_sceneMode = 9910;
	setAction(&_sequenceManager, this, 9910, &_object1, &_object2, &_object3, &_object4, &_object5, &_object6);
}

/*--------------------------------------------------------------------------
 * Scene 9999
 *
 *--------------------------------------------------------------------------*/

void Scene9999::Action1::signal() {
	switch (_actionIndex++) {
	case 0:
		setDelay(600);
		break;
	case 1:
		_globals->_sceneManager.changeScene(3500);
		break;
	default:
		break;
	}
}

/*--------------------------------------------------------------------------*/

void Scene9999::Action2::signal() {
	switch (_actionIndex++) {
	case 0:
		setDelay(10);
		break;
	case 1:
		SceneItem::display(9999, 0, SET_Y, 10, SET_X, 30, SET_FONT, 2, SET_BG_COLOUR, -1, SET_EXT_BGCOLOUR, 23, SET_WIDTH, 260, SET_KEEP_ONSCREEN, 1, LIST_END);
		setDelay(300);
		break;
	case 2:
		_globals->_stripNum = 3600;
		_globals->_sceneManager.changeScene(3600);
	default:
		break;
	}
}

void Scene9999::postInit(SceneObjectList *OwnerList) {
	loadScene(9998);
	Scene::postInit();
	setZoomPercents(0, 100, 200, 100);

	_object1.postInit();
	_object1.setVisage(1303);
	_object1.setStrip2(3);
	_object1.setPosition(Common::Point(160, 152), 0);

	_globals->_player.postInit();
	_globals->_player.setVisage(1303);
	_globals->_player.setStrip2(1);
	_globals->_player.setPriority2(250);
	_globals->_player.animate(ANIM_MODE_2, 0);
	_globals->_player.setPosition(Common::Point(194, 98), 0);
	_globals->_player._numFrames = 20;
	_globals->_player.disableControl();

	_object2.postInit();
	_object2.setVisage(1303);
	_object2.setStrip2(2);
	_object2.setPriority2(2);
	_object2.setPosition(Common::Point(164, 149), 0);

	_object3.postInit();
	_object3.setVisage(1303);
	_object3.setStrip2(2);
	_object3.setPriority2(2);
	_object3.setFrame(2);
	_object3.setPosition(Common::Point(292, 149), 0);
	_object3.setAction(&_action3);

	if (_globals->_sceneManager._previousScene == 3500)
		setAction(&_action2);
	else
		setAction(&_action1);

	_globals->_sceneManager._scene->_sceneBounds.centre(_globals->_player._position.x, _globals->_player._position.y);
	_globals->_sceneManager._scene->_sceneBounds.contain(_globals->_sceneManager._scene->_backgroundBounds);
	_globals->_sceneOffset.x = (_globals->_sceneManager._scene->_sceneBounds.left / 160) * 160;

	if (_globals->_sceneManager._previousScene == 3500)
		_globals->_stripNum = 2222;
	else
		_globals->_stripNum = 2121;

	_globals->_soundHandler.startSound(118, 0, 127);

}

} // End of namespace tSage