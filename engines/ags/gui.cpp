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

#include "engines/ags/ags.h"
#include "engines/ags/character.h"
#include "engines/ags/constants.h"
#include "engines/ags/gamefile.h"
#include "engines/ags/gamestate.h"
#include "engines/ags/graphics.h"
#include "engines/ags/gui.h"
#include "engines/ags/sprites.h"
#include "engines/ags/util.h"

#include "common/events.h"
#include "graphics/font.h"

namespace AGS {
	
void GUITextControl::drawText(Graphics::Surface* surface, Common::String text, int useX, int useY, bool drawDisabled) {
	warning("drawing text %s", text.c_str());
	uint32 color = _vm->_graphics->resolveHardcodedColor(_textColor);
	
	// FIXME override color for test
	//color = 0xAAFFAAFF;
	
	if (drawDisabled)
		color = _vm->_graphics->resolveHardcodedColor(8);
	_vm->_graphics->drawOutlinedString(_font, surface, text, useX, useY, _width, color);
}

void GUITextControl::setFont(uint32 font) {
	if (_font == font)
		return;

	assert(font < _vm->_gameFile->_fonts.size());

	_font = font;
	_parent->invalidate();
}

void GUITextControl::setColor(uint32 color) {
	if (_textColor == color)
		return;

	_textColor = color;
	_parent->invalidate();
}

void GUITextControl::setText(Common::String text) {
	if (_text == text)
		return;

	_text = text;
	_parent->invalidate();
}

bool GUIControl::isOverControl(const Common::Point &pos) {
	Common::Rect r(_x, _y, _x + _width, _y + _height);
	return r.contains(pos);
}

void GUIControl::resize(uint32 width, uint32 height) {
	if (width == _width && height == _height)
		return;

	_width = width;
	_height = height;
	_parent->controlPositionsChanged();
	_parent->invalidate();

	resized();
}

bool GUIControl::isDisabled() {
	if (_flags & GUIF_DISABLED)
		return true;

	// FIXME: global button disabling

	return false;
}

void GUIControl::setEnabled(bool enabled) {
	if (enabled && !(_flags & GUIF_DISABLED))
		return;
	else if (!enabled && (_flags & GUIF_DISABLED))
		return;

	if (enabled)
		_flags &= ~GUIF_DISABLED;
	else
		_flags |= GUIF_DISABLED;

	_parent->controlPositionsChanged();
	_parent->invalidate();
}

void GUIControl::setClickable(bool value) {
	if (value)
		_flags &= ~GUIF_NOCLICKS;
	else
		_flags |= GUIF_NOCLICKS;
}

void GUIControl::readFrom(Common::SeekableReadStream *dta) {
	_flags = dta->readUint32LE();
	_x = dta->readUint32LE();
	_y = dta->readUint32LE();
	_width = dta->readUint32LE();
	_height = dta->readUint32LE();
	_zorder = dta->readUint32LE();
	_activated = (bool)dta->readUint32LE();

	if (_vm->getGUIVersion() >= 106) {
		_scriptName = readString(dta);
	}

	if (_vm->getGUIVersion() >= 108) {
		uint32 eventCount = dta->readUint32LE();
		if (eventCount > getMaxNumEvents())
			error("too many events (%d) when reading GUIControl", eventCount);

		_eventHandlers.resize(eventCount);
		for (uint i = 0; i < eventCount; ++i)
			_eventHandlers[i] = readString(dta);
	}
}

void GUISlider::readFrom(Common::SeekableReadStream *dta) {
	GUIControl::readFrom(dta);

	_min = dta->readUint32LE();
	_max = dta->readUint32LE();
	_value = dta->readUint32LE();
	_mousePressed = dta->readUint32LE();

	if (_vm->getGUIVersion() < 104) {
		_handlePic = 0xffffffff;
		_handleOffset = 0;
		_bgImage = 0;
	} else {
		_handlePic = dta->readUint32LE();
		_handleOffset = dta->readUint32LE();
		_bgImage = dta->readUint32LE();
	}
}

bool GUISlider::isOverControl(const Common::Point &pos) {
	// check the overall boundary
	if (GUIControl::isOverControl(pos))
		return true;

	// FIXME: check the handle too
	return false;
}

void GUISlider::setMin(int32 value) {
	if (_min == value)
		return;

	_min = value;
	_parent->invalidate();
}

void GUISlider::setMax(int32 value) {
	if (_max == value)
		return;

	_max = value;
	_parent->invalidate();
}

void GUISlider::setValue(int32 value) {
	if (_value == value)
		return;

	_value = value;
	_parent->invalidate();
}

void GUISlider::setHandleOffset(int32 value) {
	if (_handleOffset == value)
		return;

	_handleOffset = value;
	_parent->invalidate();
}

void GUISlider::draw(Graphics::Surface *surface) {
	warning("GUISlider::draw unimplemented");
}

void GUILabel::readFrom(Common::SeekableReadStream *dta) {
	GUIControl::readFrom(dta);

	uint32 textLen = 200;
	if (_vm->getGUIVersion() >= 113) {
		textLen = dta->readUint32LE();
	}

	byte *buffer = new byte[textLen + 1];
	dta->read(buffer, textLen);
	buffer[textLen] = '\0';
	_text = (char *)buffer;
	delete[] buffer;

	_font = dta->readUint32LE();
	_textColor = dta->readUint32LE();
	_align = dta->readUint32LE();

	if (_textColor == 0)
		_textColor = 16;
}

void GUILabel::setAlign(uint32 align) {
	if (_align == align)
		return;

	assert(align < 3);

	_align = align;
	_parent->invalidate();
}

void GUILabel::draw(Graphics::Surface *surface) {
	Common::String text = _vm->replaceMacroTokens(_vm->getTranslation(_text));
	uint32 color = _vm->_graphics->resolveHardcodedColor(_textColor);
	Graphics::Font *font = _vm->_graphics->getFont(_font);

	Common::Array<Common::String> lines;
	font->wordWrapText(text, _width, lines);

	uint y = 0;
	for (uint i = 0; i < lines.size(); ++i) {
		int x = _x;
		uint textWidth = font->getStringWidth(lines[i]);
		switch (_align) {
		case GALIGN_LEFT:
			// nothing
			break;
		case GALIGN_CENTRE:
			x += _width / 2 - textWidth / 2;
			break;
		case GALIGN_RIGHT:
			x = _width - textWidth;
			break;
		default:
			error("GUILabel::draw: invalid alignment %d", _align);
		}
		_vm->_graphics->drawOutlinedString(_font, surface, lines[i], x, _y + y, _width, color);
		// FIXME: font multiplier?
		y += font->getFontHeight() + 1;
		if (y > _height)
			return;
	}
}

void GUITextBox::onKeyPress(uint id) {
	size_t l;
	this->_vm->_guiNeedsUpdate = true;
	switch(id) {
		case 8: //backspace
			if(_text.size())
				_text.deleteLastChar();
			return;
		case 13:
			_activated = 1;
			return;
		default:
			if(id >= 128) /*&& (!fontRenderers[font]->SupportsExtendedCharacters(font)))*/
				return;
			l = _text.size();
			if(l < 200)
				_text.insertChar(id, l);
			// FIXME
			// if the new string is too long, remove the new character
			//if (wgettextwidth(text, font) > (wid - (6 + get_fixed_pixel_size(5))))
			//  ... do not add buf to text.
	}
	this->_vm->invalidateGUI();
}

void GUITextBox::readFrom(Common::SeekableReadStream *dta) {
	GUIControl::readFrom(dta);

	char buffer[200 + 1];
	dta->read(&buffer[0], 200);
	buffer[200] = '\0';
	_text = buffer;

	_font = dta->readUint32LE();
	_textColor = dta->readUint32LE();
	_exFlags = dta->readUint32LE();
}

void GUITextBox::draw(Graphics::Surface *surface) {
	warning("GUITextBox::draw unimplemented");
	drawText(surface, getText(), _x, _y, false);
}

void GUIListBox::readFrom(Common::SeekableReadStream *dta) {
	GUIControl::readFrom(dta);

	uint32 itemCount = dta->readUint32LE();
	_selected = dta->readUint32LE();
	_topItem = dta->readUint32LE();
	_mouseXP = dta->readUint32LE();
	_mouseYP = dta->readUint32LE();
	_rowHeight = dta->readUint32LE();
	_numItemsFit = dta->readUint32LE();
	_font = dta->readUint32LE();
	_textColor = dta->readUint32LE();
	_backColor = dta->readUint32LE();
	_exFlags = dta->readUint32LE();

	if (_textColor == 0)
		_textColor = 16;

	if (_vm->getGUIVersion() >= 112) {
		_alignment = dta->readUint32LE();
		dta->skip(4); // reserved1
	} else {
		_alignment = GALIGN_LEFT;
	}

	if (_vm->getGUIVersion() >= 107) {
		_selectedBgColor = dta->readUint32LE();
	} else {
		_selectedBgColor = _textColor;
	}

	_items.resize(itemCount);
	_itemSaveGameIndexes.resize(itemCount);
	for (uint i = 0; i < itemCount; ++i) {
		_items[i] = readString(dta);
		_itemSaveGameIndexes[i] = 0xffff;
	}

	if ((_vm->getGUIVersion() >= 114) && (_exFlags & GLF_SGINDEXVALID)) {
		for (uint i = 0; i < itemCount; ++i)
			_itemSaveGameIndexes[i] = dta->readUint16LE();
	}
}

void GUIListBox::resized() {
	// FIXME
}

void GUIListBox::scrollUp() {
	// FIXME
}

void GUIListBox::scrollDown() {
	// FIXME
}

uint GUIListBox::getItemAt(Common::Point pos) {
	return (uint)-1;
}

bool GUIListBox::addItem(const Common::String &value) {
	_parent->invalidate();

	_items.push_back(value);
	_itemSaveGameIndexes.push_back((uint16)-1);

	return true;
}

bool GUIListBox::insertItemAt(uint index, const Common::String &value) {
	if (index > _items.size())
		return false;

	_parent->invalidate();

	_items.insert_at(index, value);
	_itemSaveGameIndexes.insert_at(index, (uint16)-1);

	if (_selected >= index)
		_selected++;

	return true;
}

void GUIListBox::removeItem(uint index) {
	assert(index < _items.size());

	_items.remove_at(index);
	_itemSaveGameIndexes.remove_at(index);

	if (_selected > index)
		_selected--;
	if (_selected >= _items.size())
		_selected = (uint)-1;

	_parent->invalidate();
}

void GUIListBox::clear() {
	_items.clear();
	_itemSaveGameIndexes.clear();
	_selected = 0;
	_topItem = 0;

	_parent->invalidate();
}

uint GUIListBox::getSelected() {
	if (_selected >= _items.size())
		return (uint)-1;

	return _selected;
}

void GUIListBox::setSelected(uint index) {
	if (index >= _items.size())
		index = (uint)-1;

	if (_selected == index)
		return;

	_selected = index;
	if (index != (uint)-1) {
		// If the new selected entry is off-screen, change
		// the top item as necessary to get it on-screen.
		if (index < _topItem)
			_topItem = index;
		if (index >= _topItem + _numItemsFit)
			_topItem = (index - _numItemsFit) - 1;
	}

	_parent->invalidate();
}

void GUIListBox::setTopItem(uint index) {
	// allow setTopItem(0) on an empty list, but forbid anything else
	if (!_items.empty() || index)
		if (index >= _items.size())
			error("GUIListBox::setTopItem: %d is too high (only %d items)", index, _items.size());

	if (_topItem == index)
		return;

	_topItem = index;
	_parent->invalidate();
}

void GUIListBox::draw(Graphics::Surface *surface) {
	warning("GUIListBox::draw unimplemented");
}

void GUIInvControl::readFrom(Common::SeekableReadStream *dta) {
	GUIControl::readFrom(dta);

	if (_vm->getGUIVersion() >= 109) {
		_charId = dta->readUint32LE();
		_itemWidth = dta->readUint32LE();
		_itemHeight = dta->readUint32LE();
		_topIndex = dta->readUint32LE();
	} else {
		_charId = (uint)-1;
		_itemWidth = 40;
		_itemHeight = 22;
		_topIndex = 0;
	}

	if (_vm->getGameFileVersion() >= kAGSVer270) {
		// ensure that some items are visible
		if (_itemWidth > _width)
			_itemWidth = _width;
		if (_itemHeight > _height)
			_itemHeight = _height;
	}

	// (don't recalculate cells here, screen size might not be available yet)
}

void GUIInvControl::onMouseEnter() {
	_isOver = true;
}

void GUIInvControl::onMouseLeave() {
	_isOver = false;
}

void GUIInvControl::onMouseUp() {
	if (_isOver)
		_activated = true;
}

Character *GUIInvControl::getCharToDisplay() {
	if (_charId >= _vm->_characters.size())
		return _vm->getPlayerChar();

	return _vm->_characters[_charId];
}

void GUIInvControl::resized() {
	recalculateNumCells();
}

uint GUIInvControl::getItemAt(const Common::Point &pos) {
	if (_itemWidth == 0 || _itemHeight == 0) {
		// TODO: verify we can't reach this and remove it
		error("GUIInvControl::getItemAt: item size was zero");
	}

	uint itemId = pos.x / _vm->multiplyUpCoordinate(_itemWidth);
	if (itemId >= _itemsPerLine)
		return (uint)-1;

	itemId += (pos.y / _vm->multiplyUpCoordinate(_itemHeight)) * _itemsPerLine;
	if (itemId >= _itemsPerLine * _numLines)
		return (uint)-1;

	itemId += _topIndex;
	if (itemId >= getCharToDisplay()->_invOrder.size())
		return (uint)-1;

	return getCharToDisplay()->_invOrder[itemId];
}

void GUIInvControl::setTopIndex(uint index) {
	if (_topIndex == index)
		return;

	_topIndex = index;
	_parent->invalidate();
}

void GUIInvControl::scrollUp() {
	if (_topIndex == 0)
		return;

	if (_topIndex < _itemsPerLine)
		_topIndex = 0;
	else
		_topIndex -= _itemsPerLine;

	_parent->invalidate();
}

void GUIInvControl::scrollDown() {
	if (getCharToDisplay()->_invOrder.size() <= (_topIndex + (_itemsPerLine * _numLines)))
		return;

	_topIndex += _itemsPerLine;
	_parent->invalidate();
}

void GUIInvControl::draw(Graphics::Surface *surface) {
	// TODO: Isn't this already checked in the caller?
	if (isDisabled() && (_vm->_guiDisabledStyle == GUIDIS_BLACKOUT))
		return;

	uint numItems = _itemsPerLine * _numLines;
	// backwards compatibility
	_vm->_state->_invNumInLine = _itemsPerLine;
	_vm->_state->_invNumDisplayed = numItems;
	_vm->_state->_invNumOrder = _vm->getPlayerChar()->_invOrder.size();

	if (_vm->_state->_invTop) {
		// if the user changes top_inv_item, switch into backwards
		// compatibiltiy mode
		_vm->_state->_invBackwardsCompatibility = 1;
	}

	if (_vm->_state->_invBackwardsCompatibility)
		_topIndex = _vm->_state->_invTop;

	uint curX = _x, curY = _y;
	for (uint i = 0; i < numItems; ++i) {
		uint indexId = _topIndex + i;
		if (indexId >= getCharToDisplay()->_invOrder.size())
			break;
		uint itemId = getCharToDisplay()->_invOrder[indexId];

		// draw inv graphic
		// FIXME
		Sprite *sprite = _vm->getSprites()->getSprite(_vm->_gameFile->_invItemInfo[itemId]._pic);
		_vm->_graphics->blit(sprite->_surface, surface, Common::Point(curX, curY), 0);

		curX += _vm->multiplyUpCoordinate(_itemWidth);

		// go to the next row when appropriate
		if ((i % _itemsPerLine) == (_itemsPerLine - 1)) {
			curX = _x;
			curY += _vm->multiplyUpCoordinate(_itemHeight);
		}
	}

	// FIXME: darken the inventory when disabled
}

void GUIInvControl::recalculateNumCells() {
	if (_itemWidth == 0 || _itemHeight == 0) {
		// avoid an inconvenient divide by zero
		error("GUIInvControl::recalculateNumCells: item size was zero");
	}

	if (_vm->getGameFileVersion() < kAGSVer270) {
		// pre-2.7 compatibility from JJS
		_itemsPerLine = floor((float)_width / (float)_vm->multiplyUpCoordinate(_itemWidth) + 0.5f);
		_numLines = floor((float)_height / (float)_vm->multiplyUpCoordinate(_itemHeight) + 0.5f);
		return;
	}

	_itemsPerLine = _width / _vm->multiplyUpCoordinate(_itemWidth);
	_numLines = _height / _vm->multiplyUpCoordinate(_itemHeight);
}

void GUIButton::readFrom(Common::SeekableReadStream *dta) {
	GUIControl::readFrom(dta);

	_pic = dta->readUint32LE();
	_overPic = dta->readUint32LE();
	_pushedPic = dta->readUint32LE();

	dta->skip(4); // _usePic
	_usePic = _pic;

	_isPushed = dta->readUint32LE();
	_isOver = dta->readUint32LE();
	_font = dta->readUint32LE();
	_textColor = dta->readUint32LE();
	_leftClick = dta->readUint32LE();
	_rightClick = dta->readUint32LE();
	_leftClickData = dta->readUint32LE();
	_rightClickData = dta->readUint32LE();

	char buffer[50 + 1];
	dta->read(&buffer[0], 50);
	buffer[50] = '\0';
	_text = buffer;

	if (_vm->getGUIVersion() >= 111) {
		_textAlignment = dta->readUint32LE();
		dta->skip(4); // reserved1
	} else {
		_textAlignment = GBUT_ALIGN_TOPMIDDLE;
	}
}

void GUIButton::onMouseEnter() {
	if (_isPushed)
		_usePic = _pushedPic;
	else
		_usePic = _overPic;

	_isOver = true;
}

void GUIButton::onMouseLeave() {
	_usePic = _pic;
	_isOver = false;
}

bool GUIButton::onMouseDown() {
	if (_pushedPic > 0)
		_usePic = _pushedPic;

	_isPushed = true;
	return false;
}

void GUIButton::onMouseUp() {
	if (_isOver) {
		_usePic = _overPic;
		if (!isDisabled() && isClickable())
			_activated = true;
	} else
		_usePic = _pic;

	_isPushed = false;
}

uint32 GUIButton::getDisplayedGraphic() {
	if ((int)_usePic < 0)
		return _pic;

	return _usePic;
}

void GUIButton::setNormalGraphic(uint32 pic) {
	if (_pic == pic)
		return;

	_pic = pic;
	if ((!_isOver || (int)_overPic < 1) && !_isPushed)
		_usePic = pic;

	// FIXME: resize self to size of sprite

	_parent->invalidate();
	stopAnimation();
}

void GUIButton::setMouseOverGraphic(uint32 pic) {
	if (_overPic == pic)
		return;

	_overPic = pic;
	if (_isOver && !_isPushed)
		_usePic = pic;

	_parent->invalidate();
	stopAnimation();
}

void GUIButton::setPushedGraphic(uint32 pic) {
	if (_pushedPic == pic)
		return;

	_pushedPic = pic;
	if (_isPushed)
		_usePic = pic;

	_parent->invalidate();
	stopAnimation();
}


void GUIButton::stopAnimation() {
	// FIXME
}

void GUIButton::draw(Graphics::Surface *surface) {
	bool drawDisabled = isDisabled();

	// if it's "Unchanged when disabled" or "GUI Off", don't grey out
	if (_vm->_guiDisabledStyle == GUIDIS_UNCHANGED || _vm->_guiDisabledStyle == GUIDIS_GUIOFF)
		drawDisabled = false;

	if ((int)_usePic <= 0 || drawDisabled)
		_usePic = _pic;

	// buttons off when disabled
	// TODO: Isn't this already checked in the caller?
	if (drawDisabled && (_vm->_guiDisabledStyle == GUIDIS_BLACKOUT))
		return;

	// First, we draw the graphical bits.
	if ((int)_usePic > 0 && (int)_pic > 0) {
		// graphical button

		// FIXME
		Sprite *sprite = _vm->getSprites()->getSprite(_usePic);
		_vm->_graphics->blit(sprite->_surface, surface, Common::Point(_x, _y), 0);

		// FIXME
	} else if (_text.size()) {
		// text button

		// FIXME
	}

	// Then, we try drawing the text (if any).
	if (_text.empty())
		return;
	// Don't print text of (INV) (INVSHR) (INVNS)
	if (_text.hasPrefix("(IN"))
		return;
	// Don't print the text if there's a graphic and it hasn't been named
	if (_text == "New Button" && (int)_usePic > 0 && (int)_pic > 0)
		return;

	Common::String text = _vm->getTranslation(_text);

	int useX = _x, useY = _y;

	// move the text a bit while pushed
	if (_isPushed && _isOver) {
		useX++;
		useY++;
	}
	
	Graphics::Font *font = _vm->_graphics->getFont(_font);
	// FIXME: This is the wrong height. Also, font multiplier?
	uint fontHeight = font->getFontHeight();
	// FIXME: font multiplier?
	uint textWidth = font->getStringWidth(text);
	
	// We don't use Graphics::TextAlign here, because we have to be pixel-perfect.
	switch (_textAlignment) {
	case GBUT_ALIGN_TOPMIDDLE:
		useX += (_width / 2 - textWidth / 2);
		useY += 2;
		break;
	case GBUT_ALIGN_TOPLEFT:
		useX += 2;
		useY += 2;
		break;
	case GBUT_ALIGN_TOPRIGHT:
		useX += _width - textWidth - 2;
		useY += 2;
		break;
	case GBUT_ALIGN_MIDDLELEFT:
		useX += 2;
		useY += (_height / 2) - ((fontHeight + 1) / 2);
		break;
	case GBUT_ALIGN_CENTRED:
		useX += (_width / 2 - textWidth / 2);
		useY += (_height / 2) - ((fontHeight + 1) / 2);
		break;
	case GBUT_ALIGN_MIDDLERIGHT:
		useX += _width - textWidth - 2;
		useY += (_height / 2) - ((fontHeight + 1) / 2);
		break;
	case GBUT_ALIGN_BOTTOMLEFT:
		useX += 2;
		useY += _height - fontHeight - 2;
		break;
	case GBUT_ALIGN_BOTTOMMIDDLE:
		useX += (_width / 2 - textWidth / 2);
		useY += _height - fontHeight - 2;
		break;
	case GBUT_ALIGN_BOTTOMRIGHT:
		useX += _width - textWidth - 2;
		useY += _height - fontHeight - 2;
		break;
	}
	
	drawText(surface, text, useX, useY, drawDisabled);

}

GUIGroup::GUIGroup(AGSEngine *vm) : _vm(vm), _width(0), _height(0), _needsUpdate(true), _transparency(0) {
}

GUIGroup::~GUIGroup() {
	_surface.free();
}

void GUIGroup::setSize(uint32 width, uint32 height) {
	if (_surface.pixels && width == _width && height == _height)
		return;

	_width = width;
	_height = height;
	_surface.free();

	if (!_visible)
		return;

	bool isAlpha = true;
	if ((int)_bgPic > 0)
		isAlpha = (_vm->_gameFile->_spriteFlags[_bgPic] & SPF_ALPHACHANNEL);
	else if (_bgColor > 0)
		isAlpha = false;

	_surface.create(width, height, _vm->_graphics->getPixelFormat(isAlpha));

	invalidate();
}

void GUIGroup::setBackgroundPicture(uint32 pic) {
	if (pic == _bgPic)
		return;

	// FIXME: recreate surface with/without alpha if necessary
	_bgPic = pic;
	invalidate();
}

void GUIGroup::setZOrder(uint zorder) {
	if (_zorder == zorder)
		return;

	_zorder = zorder;
	_vm->resortGUIs();
}

void GUIGroup::setTransparency(uint val) {
	assert(val <= 100);

	if (val == 0)
		val = 0;
	else if (val == 100)
		val = 255;
	else
		val = ((100 - val) * 25) / 10;

	if (_transparency == val)
		return;

	_transparency = val;
}

uint GUIGroup::getTransparency() {
	if (_transparency == 0)
		return 0;
	if (_transparency == 255)
		return 100;
	return 100 - ((_transparency * 10) / 25);
}

void GUIGroup::invalidate() {
	_needsUpdate = true;
}

void GUIGroup::controlPositionsChanged() {
	// force it to re-check for which control is under the mouse
	Common::Point mousePos = _vm->_system->getEventManager()->getMousePos();
	onMouseMove(mousePos);
}

void GUIGroup::onMouseMove(const Common::Point &pos) {
	Common::Point p = pos;
	p.x -= _x;
	p.y -= _y;

	if (_mouseOver == MOVER_MOUSEDOWNLOCKED) {
		// A control has grabbed the input.
		_controls[_mouseDownOn]->onMouseMove(p);
		return;
	}

	GUIControl *control = getControlAt(p);

	if (!control || control->_id != (uint)_mouseOver) {
		// The mouse is now over a different control.
		if (_mouseOver >= 0)
			_controls[_mouseOver]->onMouseLeave();

		if (!control || control->isDisabled() || !control->isClickable()) {
			// either no control, or the control is disabled or not clickable - ignore it
			_mouseOver = -1;
			return;
		}

		_mouseOver = control->_id;
		control->onMouseEnter();
		control->onMouseMove(p);

		invalidate();
	} else if (_mouseOver >= 0) {
		// The mouse is still over the same control.
		control->onMouseMove(p);
	}
}

void GUIGroup::onMouseDown(const Common::Point &pos) {
	if (_mouseOver < 0)
		return;

	GUIControl *control = _controls[_mouseOver];

	if (control->isDisabled() || !control->isVisible() || !control->isClickable())
		return;

	_mouseDownOn = _mouseOver;
	if (control->onMouseDown())
		_mouseOver = MOVER_MOUSEDOWNLOCKED;

	control->onMouseMove(pos - Common::Point(_x, _y));
	invalidate();
}

void GUIGroup::onMouseUp(const Common::Point &pos) {
	if (_mouseOver == MOVER_MOUSEDOWNLOCKED) {
		// focus was locked - reset it back to normal, but on the
		// locked object so that a MouseLeave gets fired if necessary
		_mouseOver = _mouseDownOn;
		// force update
		controlPositionsChanged();
	}

	if (_mouseDownOn < 0)
		return;

	_controls[_mouseDownOn]->onMouseUp();
	_mouseDownOn = -1;
	invalidate();
}

bool GUIGroup::isMouseOver(const Common::Point &pos) {
	if (!_visible)
		return false;

	if (_flags & GUIF_NOCLICK)
		return false;

	return (pos.x >= _x && pos.y >= _y && pos.x <= _x + (int)_width && pos.y <= _y + (int)_height);
}

GUIControl *GUIGroup::getControlAt(const Common::Point &pos, bool mustBeClickable) {
	for (uint i = 0; i < _controls.size(); ++i) {
		uint16 controlId = _controlDrawOrder[i];
		GUIControl *control = _controls[controlId];

		if (!control->isVisible())
			continue;

		if (mustBeClickable && !control->isClickable())
			continue;

		if (control->isOverControl(pos))
			return control;
	}

	return NULL;
}

bool GUIGroup::isTextWindow() const {
	return _vText[0] == GUI_TEXTWINDOW;
}

void GUIGroup::interfaceOn() {
	_vm->endSkippingUntilCharStops();

	if (_visible)
		return;

	if (!_enabled)
		setEnabled(true);
	if (_popup != POPUP_MOUSEY)
		setVisible(true);

	if (_popup == POPUP_SCRIPT)
		_vm->pauseGame();

	controlPositionsChanged();
	Common::Point mousePos = _vm->_system->getEventManager()->getMousePos();
	onMouseMove(mousePos);
}

void GUIGroup::interfaceOff() {
	if (!_enabled)
		return;
	if (_popup != POPUP_MOUSEY && !_visible)
		return;

	setVisible(false);

	if (_mouseOver >= 0) {
		// Make sure that the overpic is turned off when the GUI goes off
		_controls[_mouseOver]->onMouseLeave();
		_mouseOver = -1;
	}

	controlPositionsChanged();
	if (_popup == POPUP_MOUSEY)
		setEnabled(false);
	else if (_popup == POPUP_SCRIPT)
		_vm->unpauseGame();
}

void GUIGroup::setEnabled(bool enabled) {
	if (_enabled == enabled)
		return;

	assert(_enabled || !_visible);

	_enabled = enabled;
	if (!_enabled) {
		_visible = false;
		_surface.free();
	} else
		setSize(_width, _height);
}

void GUIGroup::setVisible(bool visible) {
	if (_visible == visible)
		return;

	assert(_enabled);

	_visible = visible;
	if (!_visible)
		_surface.free();
	else
		setSize(_width, _height);
}

const Graphics::Surface *GUIGroup::getDrawSurface() {
	assert(_surface.pixels);

	if (_needsUpdate)
		draw();

	return &_surface;
}

void GUIGroup::draw() {
	// stop border being transparent, if the whole GUI isn't
	// TODO: move this to some sanity-check?
	if ((_fgColor == 0) && (_bgColor != 0))
		_fgColor = 16;

	// clear the surface, filling with either transparency or the background color
	uint32 bgColor;
	if (_bgColor != 0)
		bgColor = _vm->_graphics->resolveHardcodedColor(_bgColor);
	else
		bgColor = _vm->_graphics->getTransparentColor();
	_surface.fillRect(Common::Rect(0, 0, _width, _height), bgColor);

	if (_bgColor != _fgColor) {
		// draw the border
		// FIXME
		warning("ignoring border");
	}

	if ((int)_bgPic > 0) {
		// draw the background picture
		// TODO: don't discard sprite
		Sprite *sprite = _vm->getSprites()->getSprite(_bgPic);
		_vm->_graphics->blit(sprite->_surface, &_surface, Common::Point(0, 0), 0);
	}

	for (uint i = 0; i < _controls.size(); ++i) {
		uint16 controlId = _controlDrawOrder[i];
		GUIControl *control = _controls[controlId];

		// only visible controls should be drawn
		if (!control->isVisible())
			continue;

		// if disabled controls shouldn't be drawn, don't draw them
		if (control->isDisabled() && (_vm->_guiDisabledStyle == GUIDIS_BLACKOUT))
			continue;

		// FIXME
		control->draw(&_surface);

		// FIXME: highlighting
		// _surface.frameRect(Common::Rect(control->_x, control->_y, control->_x + control->_width, control->_y + control->_height), _vm->_graphics->resolveHardcodedColor(14));
	}

	_needsUpdate = false;
}

struct GUIZOrderLess {
	bool operator()(const GUIControl *a, const GUIControl *b) const {
		return a->_zorder < b->_zorder;
	}
};

void GUIGroup::sortControls() {
	Common::Array<GUIControl *> controls = _controls;

	Common::sort(controls.begin(), controls.end(), GUIZOrderLess());

	_controlDrawOrder.resize(controls.size());
	for (uint i = 0; i < controls.size(); ++i)
		_controlDrawOrder[i] = _controls[i]->_id;
}

} // End of namespace AGS
