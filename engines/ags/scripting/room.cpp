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
 */

/* Based on the Adventure Game Studio source code, copyright 1999-2011 Chris Jones,
 * which is licensed under the Artistic License 2.0.
 * You may also modify/distribute the code in this file under that license.
 */

#include "engines/ags/scripting/scripting.h"
#include "engines/ags/constants.h"
#include "engines/ags/drawingsurface.h"
#include "engines/ags/gamestate.h"
#include "engines/ags/graphics.h"
#include "engines/ags/room.h"

namespace AGS {

// Room: import static String GetTextProperty(const string property)
// Gets a Custom Property associated with this room.
RuntimeValue Script_Room_GetTextProperty(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	ScriptString *property = (ScriptString *)params[0]._object;

	Common::String string = vm->getStringProperty(property->getString(), vm->getCurrentRoom()->_properties);
	RuntimeValue ret = new ScriptMutableString(string);
	ret._object->DecRef();
	return ret;
}

// Room: import static DrawingSurface* GetDrawingSurfaceForBackground(int backgroundNumber=SCR_NO_VALUE)
// Gets a drawing surface that allows you to manipulate the room background.
RuntimeValue Script_Room_GetDrawingSurfaceForBackground(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint backgroundNumber = params[0]._value;

	if (backgroundNumber == SCR_NO_VALUE)
		backgroundNumber = vm->_state->_bgFrame;

	Room *room = vm->getCurrentRoom();
	if (backgroundNumber >= room->_backgroundScenes.size())
		error("Room::GetDrawingSurfaceForBackground: background %d is too high (only %d backgrounds)",
		backgroundNumber, room->_backgroundScenes.size());

	DrawingSurface *surface = new DrawingSurface(vm);
	surface->_type = dstRoomBackground;
	surface->_id = backgroundNumber;

	RuntimeValue ret = surface;
	ret._object->DecRef();
	return ret;
}

// Room: readonly import static attribute int BottomEdge
// Gets the Y co-ordinate of the bottom edge of the room.
RuntimeValue Script_Room_get_BottomEdge(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Room::get_BottomEdge unimplemented");

	return RuntimeValue();
}

// Room: readonly import static attribute int ColorDepth
// Gets the colour depth of the room background.
RuntimeValue Script_Room_get_ColorDepth(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Room::get_ColorDepth unimplemented");

	return RuntimeValue();
}

// Room: readonly import static attribute int Height
// Gets the height of the room background.
RuntimeValue Script_Room_get_Height(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	return vm->getCurrentRoom()->_height;
}

// Room: readonly import static attribute int LeftEdge
// Gets the X co-ordinate of the left edge of the room.
RuntimeValue Script_Room_get_LeftEdge(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Room::get_LeftEdge unimplemented");

	return RuntimeValue();
}

// Room: readonly import static attribute String Messages[]
// Accesses room messages, as set up in the Room Message Editor.
RuntimeValue Script_Room_geti_Messages(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int index = params[0]._signedValue;
	UNUSED(index);

	// FIXME
	error("Room::geti_Messages unimplemented");

	return RuntimeValue();
}

// Room: readonly import static attribute int MusicOnLoad
// Gets the music that is played when the player enters this room.
RuntimeValue Script_Room_get_MusicOnLoad(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Room::get_MusicOnLoad unimplemented");

	return RuntimeValue();
}

// Room: readonly import static attribute int ObjectCount
// Gets the number of objects in this room.
RuntimeValue Script_Room_get_ObjectCount(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	return vm->getCurrentRoom()->_objects.size();
}

// Room: readonly import static attribute int RightEdge
// Gets the X co-ordinate of the right edge of the room.
RuntimeValue Script_Room_get_RightEdge(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Room::get_RightEdge unimplemented");

	return RuntimeValue();
}

// Room: readonly import static attribute int TopEdge
// Gets the Y co-ordinate of the top edge of the room.
RuntimeValue Script_Room_get_TopEdge(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Room::get_TopEdge unimplemented");

	return RuntimeValue();
}

// Room: readonly import static attribute int Width
// Gets the width of the room background.
RuntimeValue Script_Room_get_Width(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	return vm->getCurrentRoom()->_width;
}

// import void ResetRoom(int roomNumber)
// Resets the room state back to how it was initially set up in the editor.
RuntimeValue Script_ResetRoom(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int roomNumber = params[0]._signedValue;
	UNUSED(roomNumber);

	// FIXME
	warning("ResetRoom unimplemented");

	return RuntimeValue();
}

// import int HasPlayerBeenInRoom(int roomNumber)
// Checks whether the player has been in the specified room yet.
RuntimeValue Script_HasPlayerBeenInRoom(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int roomNumber = params[0]._signedValue;
	UNUSED(roomNumber);

	// FIXME
	error("HasPlayerBeenInRoom unimplemented");

	return RuntimeValue();
}

// import void CallRoomScript(int value)
// Calls the on_call function in the current room.
RuntimeValue Script_CallRoomScript(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint value = params[0]._value;

	// FIXME: vm->canRunDelayedCommand("CallRoomScript");

	vm->_state->_roomScriptFinished = 0;
	vm->queueCustomRoomScript(value);

	return RuntimeValue();
}

// import LocationType GetLocationType(int x, int y)
// Gets what type of thing is in the room at the specified co-ordinates.
RuntimeValue Script_GetLocationType(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int x = params[0]._signedValue;
	int y = params[1]._signedValue;

	return vm->getLocationType(Common::Point(x, y));
}

// import int GetWalkableAreaAt(int screenX, int screenY)
// Returns which walkable area is at the specified position relative to the current viewport.
RuntimeValue Script_GetWalkableAreaAt(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int x = params[0]._signedValue;
	int y = params[1]._signedValue;

	x += vm->divideDownCoordinate(vm->_graphics->_viewportX);
	y += vm->divideDownCoordinate(vm->_graphics->_viewportY);

	if (x < 0 || y < 0 || x >= vm->getCurrentRoom()->_width || y >= vm->getCurrentRoom()->_height)
		return 0;

	x = vm->convertToLowRes(x);
	y = vm->convertToLowRes(y);

	return *(byte *)vm->getCurrentRoom()->_walkableMask.getBasePtr(x, y);
}

// import int GetScalingAt (int x, int y)
// Returns the scaling level at the specified position within the room.
RuntimeValue Script_GetScalingAt(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int x = params[0]._signedValue;
	UNUSED(x);
	int y = params[1]._signedValue;
	UNUSED(y);

	// FIXME
	warning("GetScalingAt unimplemented");

	return RuntimeValue();
}

// import int GetRoomProperty(const string property)
// Gets the specified Custom Property for the current room.
RuntimeValue Script_GetRoomProperty(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	ScriptString *property = (ScriptString *)params[0]._object;

	return vm->getIntProperty(property->getString(), vm->getCurrentRoom()->_properties);
}

// import void GetLocationName(int x, int y, string buffer)
// Old string buffer function.
RuntimeValue Script_GetLocationName(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int x = params[0]._signedValue;
	int y = params[1]._signedValue;
	ScriptString *buffer = (ScriptString *)params[2]._object;

	buffer->setString(vm->getLocationName(Common::Point(x, y)));

	return RuntimeValue();
}

// import void GetRoomPropertyText(const string property, string buffer)
// Old string buffer function.
RuntimeValue Script_GetRoomPropertyText(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	ScriptString *property = (ScriptString *)params[0]._object;
	ScriptString *buffer = (ScriptString *)params[1]._object;

	buffer->setString(vm->getStringProperty(property->getString(), vm->getCurrentRoom()->_properties));

	return RuntimeValue();
}

// import int GetHotspotAt(int x, int y)
// OBSOLETE STUFF
RuntimeValue Script_GetHotspotAt(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int x = params[0]._signedValue;
	int y = params[1]._signedValue;

	x += vm->divideDownCoordinate(vm->_graphics->_viewportX);
	y += vm->divideDownCoordinate(vm->_graphics->_viewportY);

	if (x < 0 || y < 0 || x >= vm->getCurrentRoom()->_width || y >= vm->getCurrentRoom()->_height)
		return 0;

	return vm->getCurrentRoom()->getHotspotAt(x, y);
}

// import int GetObjectAt(int x,int y)
// Undocumented.
RuntimeValue Script_GetObjectAt(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int x = params[0]._signedValue;
	int y = params[1]._signedValue;

	return vm->getCurrentRoom()->getObjectAt(x, y);
}

// import int GetRegionAt (int x, int y)
// Undocumented.
RuntimeValue Script_GetRegionAt(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int x = params[0]._signedValue;
	int y = params[1]._signedValue;

	return vm->getCurrentRoom()->getRegionAt(x, y);
}

// import void DisableHotspot(int hotspot)
// Obsolete hotspot function.
RuntimeValue Script_DisableHotspot(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint hotspot = params[0]._value;

	if (hotspot >= vm->getCurrentRoom()->_hotspots.size())
		error("DisableHotspot: hotspot %d is too high (only have %d)",
			hotspot, vm->getCurrentRoom()->_hotspots.size());

	vm->getCurrentRoom()->_hotspots[hotspot]._enabled = false;

	debugC(kDebugLevelGame, "hotspot %d disabled", hotspot);

	return RuntimeValue();
}

// import void EnableHotspot(int hotspot)
// Obsolete hotspot function.
RuntimeValue Script_EnableHotspot(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint hotspot = params[0]._value;

	if (hotspot >= vm->getCurrentRoom()->_hotspots.size())
		error("EnableHotspot: hotspot %d is too high (only have %d)",
			hotspot, vm->getCurrentRoom()->_hotspots.size());

	vm->getCurrentRoom()->_hotspots[hotspot]._enabled = true;

	debugC(kDebugLevelGame, "hotspot %d enabled", hotspot);

	return RuntimeValue();
}

// import void GetHotspotName(int hotspot, string buffer)
// Obsolete hotspot function.
RuntimeValue Script_GetHotspotName(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint hotspot = params[0]._value;
	ScriptString *buffer = (ScriptString *)params[1]._object;

	if (hotspot >= vm->getCurrentRoom()->_hotspots.size())
		error("GetHotspotName: hotspot %d is too high (only have %d)",
			hotspot, vm->getCurrentRoom()->_hotspots.size());

	buffer->setString(vm->getCurrentRoom()->_hotspots[hotspot]._name);

	return RuntimeValue();
}

// import int GetHotspotPointX(int hotspot)
// Obsolete hotspot function.
RuntimeValue Script_GetHotspotPointX(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint hotspot = params[0]._value;

	if (hotspot >= vm->getCurrentRoom()->_hotspots.size())
		error("GetHotspotPointX: hotspot %d is too high (only have %d)",
			hotspot, vm->getCurrentRoom()->_hotspots.size());

	if (vm->getCurrentRoom()->_hotspots[hotspot]._walkToPos.x < 1)
		return -1;

	return vm->getCurrentRoom()->_hotspots[hotspot]._walkToPos.x;
}

// import int GetHotspotPointY(int hotspot)
// Obsolete hotspot function.
RuntimeValue Script_GetHotspotPointY(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint hotspot = params[0]._value;

	if (hotspot >= vm->getCurrentRoom()->_hotspots.size())
		error("GetHotspotPointY: hotspot %d is too high (only have %d)",
			hotspot, vm->getCurrentRoom()->_hotspots.size());

	if (vm->getCurrentRoom()->_hotspots[hotspot]._walkToPos.y < 1)
		return -1;

	return vm->getCurrentRoom()->_hotspots[hotspot]._walkToPos.y;
}

// import int GetHotspotProperty(int hotspot, const string property)
// Obsolete hotspot function.
RuntimeValue Script_GetHotspotProperty(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint hotspot = params[0]._value;
	ScriptString *property = (ScriptString *)params[1]._object;

	if (hotspot >= vm->getCurrentRoom()->_hotspots.size())
		error("GetHotspotProperty: hotspot %d is too high (only have %d)",
			hotspot, vm->getCurrentRoom()->_hotspots.size());

	return vm->getIntProperty(property->getString(), vm->getCurrentRoom()->_hotspots[hotspot]._properties);
}

// import void GetHotspotPropertyText(int hotspot, const string property, string buffer)
// Obsolete hotspot function.
RuntimeValue Script_GetHotspotPropertyText(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint hotspot = params[0]._value;
	ScriptString *property = (ScriptString *)params[1]._object;
	ScriptString *buffer = (ScriptString *)params[2]._object;

	if (hotspot >= vm->getCurrentRoom()->_hotspots.size())
		error("GetHotspotPropertyText: hotspot %d is too high (only have %d)",
			hotspot, vm->getCurrentRoom()->_hotspots.size());

	buffer->setString(vm->getStringProperty(property->getString(), vm->getCurrentRoom()->_hotspots[hotspot]._properties));

	return RuntimeValue();
}

// import void RunHotspotInteraction (int hotspot, CursorMode)
// Obsolete hotspot function.
RuntimeValue Script_RunHotspotInteraction(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint hotspot = params[0]._value;
	uint32 cursormode = params[1]._value;

	if (hotspot >= vm->getCurrentRoom()->_hotspots.size())
		error("RunHotspotInteraction: hotspot %d is too high (only have %d)",
			hotspot, vm->getCurrentRoom()->_hotspots.size());

	vm->runHotspotInteraction(hotspot, cursormode);

	return RuntimeValue();
}

// import void DisableRegion(int region)
// Obsolete region function.
RuntimeValue Script_DisableRegion(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint region = params[0]._value;

	if (region >= vm->getCurrentRoom()->_regions.size())
		error("DisableRegion: region %d is too high (only have %d)",
			region, vm->getCurrentRoom()->_regions.size());

	vm->getCurrentRoom()->_regions[region]._enabled = false;

	debugC(kDebugLevelGame, "region %d disabled", region);

	return RuntimeValue();
}

// import void EnableRegion(int region)
// Obsolete region function.
RuntimeValue Script_EnableRegion(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint region = params[0]._value;

	if (region >= vm->getCurrentRoom()->_regions.size())
		error("EnableRegion: region %d is too high (only have %d)",
			region, vm->getCurrentRoom()->_regions.size());

	vm->getCurrentRoom()->_regions[region]._enabled = true;

	debugC(kDebugLevelGame, "region %d enabled", region);

	return RuntimeValue();
}

// import void RunRegionInteraction (int region, int event)
// Obsolete region function.
RuntimeValue Script_RunRegionInteraction(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int region = params[0]._signedValue;
	UNUSED(region);
	int event = params[1]._signedValue;
	UNUSED(event);

	// FIXME
	error("RunRegionInteraction unimplemented");

	return RuntimeValue();
}

// import void SetAreaLightLevel(int area, int lightLevel)
// Obsolete region function.
RuntimeValue Script_SetAreaLightLevel(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int area = params[0]._signedValue;
	UNUSED(area);
	int lightLevel = params[1]._signedValue;
	UNUSED(lightLevel);

	// FIXME
	error("SetAreaLightLevel unimplemented");

	return RuntimeValue();
}

// import void SetRegionTint(int area, int red, int green, int blue, int amount)
// Obsolete region function.
RuntimeValue Script_SetRegionTint(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int area = params[0]._signedValue;
	UNUSED(area);
	int red = params[1]._signedValue;
	UNUSED(red);
	int green = params[2]._signedValue;
	UNUSED(green);
	int blue = params[3]._signedValue;
	UNUSED(blue);
	int amount = params[4]._signedValue;
	UNUSED(amount);

	// FIXME
	warning("SetRegionTint unimplemented");

	return RuntimeValue();
}

// import void RemoveWalkableArea(int area)
// Removes the specified walkable area from the room.
RuntimeValue Script_RemoveWalkableArea(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint area = params[0]._value;

	if (area < 1 || area > 15)
		error("RemoveWalkableArea: invalid walkable area %d specified (must be 1-15).", area);

	vm->_state->_walkableAreasOn[area] = 0;
	if (vm->getCurrentRoom())
		vm->getCurrentRoom()->redoWalkableAreas();

	return RuntimeValue();
}

// import void RestoreWalkableArea(int area)
// Brings back a previously removed walkable area.
RuntimeValue Script_RestoreWalkableArea(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint area = params[0]._value;

	if (area < 1 || area > 15)
		error("RemoveWalkableArea: invalid walkable area %d specified (must be 1-15).", area);

	vm->_state->_walkableAreasOn[area] = 1;
	if (vm->getCurrentRoom())
		vm->getCurrentRoom()->redoWalkableAreas();

	return RuntimeValue();
}

// import void SetAreaScaling(int area, int min, int max)
// Changes the specified walkable area's scaling level.
RuntimeValue Script_SetAreaScaling(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int area = params[0]._signedValue;
	UNUSED(area);
	int min = params[1]._signedValue;
	UNUSED(min);
	int max = params[2]._signedValue;
	UNUSED(max);

	// FIXME
	error("SetAreaScaling unimplemented");

	return RuntimeValue();
}

// import void DisableGroundLevelAreas(int disableTints)
// Disables all region events, and optionally light levels and tints.
RuntimeValue Script_DisableGroundLevelAreas(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int disableTints = params[0]._signedValue;
	UNUSED(disableTints);

	// FIXME
	error("DisableGroundLevelAreas unimplemented");

	return RuntimeValue();
}

// import void EnableGroundLevelAreas()
// Re-enables region events, light levels and tints.
RuntimeValue Script_EnableGroundLevelAreas(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("EnableGroundLevelAreas unimplemented");

	return RuntimeValue();
}

// import void SetWalkBehindBase(int area, int baseline)
// Changes the baseline of the specified walk-behind area.
RuntimeValue Script_SetWalkBehindBase(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	uint area = params[0]._value;
	int baseline = params[1]._value;

	// FIXME: check bounds for baseline?

	if (area < 1 || area >= vm->getCurrentRoom()->_walkBehinds.size())
		error("SetWalkBehindBase: walk-behind area %d is invalid", area);

	if (baseline != vm->getCurrentRoom()->_walkBehinds[area]._baseline) {
		vm->getCurrentRoom()->_walkBehinds[area]._baseline = baseline;
		vm->invalidateBackground();
		debugC(kDebugLevelGame, "walk-behind %d baseline changed to %d", area, baseline);
	}

	return RuntimeValue();
}

// Hotspot: import static Hotspot* GetAtScreenXY(int x, int y)
// Gets the hotspot that is at the specified position on the screen.
RuntimeValue Script_Hotspot_GetAtScreenXY(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int x = params[0]._signedValue;
	int y = params[1]._signedValue;

	uint hotspotId = vm->getCurrentRoom()->getHotspotAt(x, y);
	return &vm->getCurrentRoom()->_hotspots[hotspotId];
}

// Hotspot: import void GetName(string buffer)
// Undocumented.
RuntimeValue Script_Hotspot_GetName(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	ScriptString *buffer = (ScriptString *)params[0]._object;

	buffer->setString(self->_name);

	return RuntimeValue();
}

// Hotspot: import void GetPropertyText(const string property, string buffer)
// Undocumented.
RuntimeValue Script_Hotspot_GetPropertyText(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	ScriptString *property = (ScriptString *)params[0]._object;
	ScriptString *buffer = (ScriptString *)params[1]._object;

	buffer->setString(vm->getStringProperty(property->getString(), self->_properties));

	return RuntimeValue();
}

// Hotspot: import int GetProperty(const string property)
// Gets an integer Custom Property for this hotspot.
RuntimeValue Script_Hotspot_GetProperty(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	ScriptString *property = (ScriptString *)params[0]._object;

	return vm->getIntProperty(property->getString(), self->_properties);
}

// Hotspot: import String GetTextProperty(const string property)
// Gets a text Custom Property for this hotspot.
RuntimeValue Script_Hotspot_GetTextProperty(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	ScriptString *property = (ScriptString *)params[0]._object;

	Common::String string = vm->getStringProperty(property->getString(), self->_properties);
	RuntimeValue ret = new ScriptMutableString(string);
	ret._object->DecRef();
	return ret;
}

// Hotspot: import void RunInteraction(CursorMode)
// Runs the specified event handler for this hotspot.
RuntimeValue Script_Hotspot_RunInteraction(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	uint32 cursormode = params[0]._value;

	vm->runHotspotInteraction(self->_id, cursormode);

	return RuntimeValue();
}

// Hotspot: import attribute bool Enabled
// Gets/sets whether this hotspot is enabled.
RuntimeValue Script_Hotspot_get_Enabled(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	return self->_enabled ? 1 : 0;
}

// Hotspot: import attribute bool Enabled
// Gets/sets whether this hotspot is enabled.
RuntimeValue Script_Hotspot_set_Enabled(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	uint32 value = params[0]._value;

	self->_enabled = (bool)value;

	return RuntimeValue();
}

// Hotspot: readonly import attribute int ID
// Gets the ID of the hotspot.
RuntimeValue Script_Hotspot_get_ID(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	return self->_id;
}

// Hotspot: readonly import attribute String Name
// Gets the name of the hotspot.
RuntimeValue Script_Hotspot_get_Name(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	RuntimeValue ret = new ScriptMutableString(self->_name);
	ret._object->DecRef();
	return ret;
}

// Hotspot: readonly import attribute int WalkToX
// Gets the X co-ordinate of the walk-to point for this hotspot.
RuntimeValue Script_Hotspot_get_WalkToX(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	if (self->_walkToPos.x < 1)
		return -1;

	return self->_walkToPos.x;
}

// Hotspot: readonly import attribute int WalkToY
// Gets the Y co-ordinate of the walk-to point for this hotspot.
RuntimeValue Script_Hotspot_get_WalkToY(AGSEngine *vm, RoomHotspot *self, const Common::Array<RuntimeValue> &params) {
	if (self->_walkToPos.y < 1)
		return -1;

	return self->_walkToPos.y;
}

// Region: import static Region* GetAtRoomXY(int x, int y)
// Gets the region at the specified location within this room.
RuntimeValue Script_Region_GetAtRoomXY(AGSEngine *vm, ScriptObject *, const Common::Array<RuntimeValue> &params) {
	int x = params[0]._signedValue;
	int y = params[1]._signedValue;

	uint regionId = vm->getCurrentRoom()->getRegionAt(x, y);
	return &vm->getCurrentRoom()->_regions[regionId];
}

// Region: import void RunInteraction(int event)
// Runs the event handler for the specified event for this region.
RuntimeValue Script_Region_RunInteraction(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	uint event = params[0]._value;

	vm->runRegionInteraction(self->_id, event);

	return RuntimeValue();
}

// Region: import void Tint(int red, int green, int blue, int amount)
// Sets the region tint which will apply to characters that are standing on the region.
RuntimeValue Script_Region_Tint(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	int red = params[0]._signedValue;
	UNUSED(red);
	int green = params[1]._signedValue;
	UNUSED(green);
	int blue = params[2]._signedValue;
	UNUSED(blue);
	int amount = params[3]._signedValue;
	UNUSED(amount);

	// FIXME
	error("Region::Tint unimplemented");

	return RuntimeValue();
}

// Region: import attribute bool Enabled
// Gets/sets whether this region is enabled.
RuntimeValue Script_Region_get_Enabled(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	return self->_enabled ? 1 : 0;
}

// Region: import attribute bool Enabled
// Gets/sets whether this region is enabled.
RuntimeValue Script_Region_set_Enabled(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	uint32 value = params[0]._value;

	self->_enabled = value ? true : false;

	return RuntimeValue();
}

// Region: readonly import attribute int ID
// Gets the ID number for this region.
RuntimeValue Script_Region_get_ID(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Region::get_ID unimplemented");

	return RuntimeValue();
}

// Region: import attribute int LightLevel
// Gets/sets the light level for this region.
RuntimeValue Script_Region_get_LightLevel(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	warning("Region::get_LightLevel unimplemented");

	return RuntimeValue();
}

// Region: import attribute int LightLevel
// Gets/sets the light level for this region.
RuntimeValue Script_Region_set_LightLevel(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	int value = params[0]._signedValue;
	UNUSED(value);

	// FIXME
	warning("Region::set_LightLevel unimplemented");

	return RuntimeValue();
}

// Region: readonly import attribute bool TintEnabled
// Gets whether a colour tint is set for this region.
RuntimeValue Script_Region_get_TintEnabled(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Region::get_TintEnabled unimplemented");

	return RuntimeValue();
}

// Region: readonly import attribute int TintBlue
// Gets the Blue component of this region's colour tint.
RuntimeValue Script_Region_get_TintBlue(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Region::get_TintBlue unimplemented");

	return RuntimeValue();
}

// Region: readonly import attribute int TintGreen
// Gets the Green component of this region's colour tint.
RuntimeValue Script_Region_get_TintGreen(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Region::get_TintGreen unimplemented");

	return RuntimeValue();
}

// Region: readonly import attribute int TintRed
// Gets the Red component of this region's colour tint.
RuntimeValue Script_Region_get_TintRed(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Region::get_TintRed unimplemented");

	return RuntimeValue();
}

// Region: readonly import attribute int TintSaturation
// Gets the Saturation of this region's colour tint.
RuntimeValue Script_Region_get_TintSaturation(AGSEngine *vm, RoomRegion *self, const Common::Array<RuntimeValue> &params) {
	// FIXME
	error("Region::get_TintSaturation unimplemented");

	return RuntimeValue();
}

static const ScriptSystemFunctionInfo ourFunctionList[] = {
	{ "Room::GetTextProperty^1", (ScriptAPIFunction *)&Script_Room_GetTextProperty, "s", sotNone },
	{ "Room::GetDrawingSurfaceForBackground^1", (ScriptAPIFunction *)&Script_Room_GetDrawingSurfaceForBackground, "i", sotNone },
	{ "Room::get_BottomEdge", (ScriptAPIFunction *)&Script_Room_get_BottomEdge, "", sotNone },
	{ "Room::get_ColorDepth", (ScriptAPIFunction *)&Script_Room_get_ColorDepth, "", sotNone },
	{ "Room::get_Height", (ScriptAPIFunction *)&Script_Room_get_Height, "", sotNone },
	{ "Room::get_LeftEdge", (ScriptAPIFunction *)&Script_Room_get_LeftEdge, "", sotNone },
	{ "Room::geti_Messages", (ScriptAPIFunction *)&Script_Room_geti_Messages, "i", sotNone },
	{ "Room::get_MusicOnLoad", (ScriptAPIFunction *)&Script_Room_get_MusicOnLoad, "", sotNone },
	{ "Room::get_ObjectCount", (ScriptAPIFunction *)&Script_Room_get_ObjectCount, "", sotNone },
	{ "Room::get_RightEdge", (ScriptAPIFunction *)&Script_Room_get_RightEdge, "", sotNone },
	{ "Room::get_TopEdge", (ScriptAPIFunction *)&Script_Room_get_TopEdge, "", sotNone },
	{ "Room::get_Width", (ScriptAPIFunction *)&Script_Room_get_Width, "", sotNone },
	{ "ResetRoom", (ScriptAPIFunction *)&Script_ResetRoom, "i", sotNone },
	{ "HasPlayerBeenInRoom", (ScriptAPIFunction *)&Script_HasPlayerBeenInRoom, "i", sotNone },
	{ "CallRoomScript", (ScriptAPIFunction *)&Script_CallRoomScript, "i", sotNone },
	{ "GetLocationType", (ScriptAPIFunction *)&Script_GetLocationType, "ii", sotNone },
	{ "GetWalkableAreaAt", (ScriptAPIFunction *)&Script_GetWalkableAreaAt, "ii", sotNone },
	{ "GetScalingAt", (ScriptAPIFunction *)&Script_GetScalingAt, "ii", sotNone },
	{ "GetRoomProperty", (ScriptAPIFunction *)&Script_GetRoomProperty, "s", sotNone },
	{ "GetLocationName", (ScriptAPIFunction *)&Script_GetLocationName, "iis", sotNone },
	{ "GetRoomPropertyText", (ScriptAPIFunction *)&Script_GetRoomPropertyText, "ss", sotNone },
	{ "GetHotspotAt", (ScriptAPIFunction *)&Script_GetHotspotAt, "ii", sotNone },
	{ "GetObjectAt", (ScriptAPIFunction *)&Script_GetObjectAt, "ii", sotNone },
	{ "GetRegionAt", (ScriptAPIFunction *)&Script_GetRegionAt, "ii", sotNone },
	{ "DisableHotspot", (ScriptAPIFunction *)&Script_DisableHotspot, "i", sotNone },
	{ "EnableHotspot", (ScriptAPIFunction *)&Script_EnableHotspot, "i", sotNone },
	{ "GetHotspotName", (ScriptAPIFunction *)&Script_GetHotspotName, "is", sotNone },
	{ "GetHotspotPointX", (ScriptAPIFunction *)&Script_GetHotspotPointX, "i", sotNone },
	{ "GetHotspotPointY", (ScriptAPIFunction *)&Script_GetHotspotPointY, "i", sotNone },
	{ "GetHotspotProperty", (ScriptAPIFunction *)&Script_GetHotspotProperty, "is", sotNone },
	{ "GetHotspotPropertyText", (ScriptAPIFunction *)&Script_GetHotspotPropertyText, "iss", sotNone },
	{ "RunHotspotInteraction", (ScriptAPIFunction *)&Script_RunHotspotInteraction, "ii", sotNone },
	{ "DisableRegion", (ScriptAPIFunction *)&Script_DisableRegion, "i", sotNone },
	{ "EnableRegion", (ScriptAPIFunction *)&Script_EnableRegion, "i", sotNone },
	{ "RunRegionInteraction", (ScriptAPIFunction *)&Script_RunRegionInteraction, "ii", sotNone },
	{ "SetAreaLightLevel", (ScriptAPIFunction *)&Script_SetAreaLightLevel, "ii", sotNone },
	{ "SetRegionTint", (ScriptAPIFunction *)&Script_SetRegionTint, "iiiii", sotNone },
	{ "RemoveWalkableArea", (ScriptAPIFunction *)&Script_RemoveWalkableArea, "i", sotNone },
	{ "RestoreWalkableArea", (ScriptAPIFunction *)&Script_RestoreWalkableArea, "i", sotNone },
	{ "SetAreaScaling", (ScriptAPIFunction *)&Script_SetAreaScaling, "iii", sotNone },
	{ "DisableGroundLevelAreas", (ScriptAPIFunction *)&Script_DisableGroundLevelAreas, "i", sotNone },
	{ "EnableGroundLevelAreas", (ScriptAPIFunction *)&Script_EnableGroundLevelAreas, "", sotNone },
	{ "SetWalkBehindBase", (ScriptAPIFunction *)&Script_SetWalkBehindBase, "ii", sotNone },
	{ "Hotspot::GetAtScreenXY^2", (ScriptAPIFunction *)&Script_Hotspot_GetAtScreenXY, "ii", sotNone },
	{ "Hotspot::GetName^1", (ScriptAPIFunction *)&Script_Hotspot_GetName, "s", sotHotspot },
	{ "Hotspot::GetPropertyText^2", (ScriptAPIFunction *)&Script_Hotspot_GetPropertyText, "ss", sotHotspot },
	{ "Hotspot::GetProperty^1", (ScriptAPIFunction *)&Script_Hotspot_GetProperty, "s", sotHotspot },
	{ "Hotspot::GetTextProperty^1", (ScriptAPIFunction *)&Script_Hotspot_GetTextProperty, "s", sotHotspot },
	{ "Hotspot::RunInteraction^1", (ScriptAPIFunction *)&Script_Hotspot_RunInteraction, "i", sotHotspot },
	{ "Hotspot::get_Enabled", (ScriptAPIFunction *)&Script_Hotspot_get_Enabled, "", sotHotspot },
	{ "Hotspot::set_Enabled", (ScriptAPIFunction *)&Script_Hotspot_set_Enabled, "i", sotHotspot },
	{ "Hotspot::get_ID", (ScriptAPIFunction *)&Script_Hotspot_get_ID, "", sotHotspot },
	{ "Hotspot::get_Name", (ScriptAPIFunction *)&Script_Hotspot_get_Name, "", sotHotspot },
	{ "Hotspot::get_WalkToX", (ScriptAPIFunction *)&Script_Hotspot_get_WalkToX, "", sotHotspot },
	{ "Hotspot::get_WalkToY", (ScriptAPIFunction *)&Script_Hotspot_get_WalkToY, "", sotHotspot },
	{ "Region::GetAtRoomXY^2", (ScriptAPIFunction *)&Script_Region_GetAtRoomXY, "ii", sotNone },
	{ "Region::RunInteraction^1", (ScriptAPIFunction *)&Script_Region_RunInteraction, "i", sotRegion },
	{ "Region::Tint^4", (ScriptAPIFunction *)&Script_Region_Tint, "iiii", sotRegion },
	{ "Region::get_Enabled", (ScriptAPIFunction *)&Script_Region_get_Enabled, "", sotRegion },
	{ "Region::set_Enabled", (ScriptAPIFunction *)&Script_Region_set_Enabled, "i", sotRegion },
	{ "Region::get_ID", (ScriptAPIFunction *)&Script_Region_get_ID, "", sotRegion },
	{ "Region::get_LightLevel", (ScriptAPIFunction *)&Script_Region_get_LightLevel, "", sotRegion },
	{ "Region::set_LightLevel", (ScriptAPIFunction *)&Script_Region_set_LightLevel, "i", sotRegion },
	{ "Region::get_TintEnabled", (ScriptAPIFunction *)&Script_Region_get_TintEnabled, "", sotRegion },
	{ "Region::get_TintBlue", (ScriptAPIFunction *)&Script_Region_get_TintBlue, "", sotRegion },
	{ "Region::get_TintGreen", (ScriptAPIFunction *)&Script_Region_get_TintGreen, "", sotRegion },
	{ "Region::get_TintRed", (ScriptAPIFunction *)&Script_Region_get_TintRed, "", sotRegion },
	{ "Region::get_TintSaturation", (ScriptAPIFunction *)&Script_Region_get_TintSaturation, "", sotRegion },
};

void addRoomSystemScripting(AGSEngine *vm) {
	GlobalScriptState *state = vm->getScriptState();

	state->addSystemFunctionImportList(ourFunctionList, ARRAYSIZE(ourFunctionList));
}

} // End of namespace AGS
