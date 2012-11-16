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

#ifndef AGS_SPRITES_H
#define AGS_SPRITES_H

#include "common/array.h"
#include "common/stream.h"

namespace Graphics {
	struct Surface;
}

namespace AGS {

void unpackSpriteBits(Common::SeekableReadStream *stream, byte *dest, uint32 size);
void unpackSpriteBits16(Common::SeekableReadStream *stream, uint16 *dest, uint32 size);
void unpackSpriteBits32(Common::SeekableReadStream *stream, uint32 *dest, uint32 size);

struct SpriteInfo {
	uint32 _offset;
	uint32 _width, _height;
};

struct Sprite {
	Sprite(uint spriteId, Graphics::Surface *surf) : _id(spriteId), _surface(surf), _refCount(0) { }
	~Sprite();

	uint _id;
	Graphics::Surface *_surface;
	uint _refCount;
};

class AGSEngine;

class SpriteSet {
public:
	SpriteSet(AGSEngine *vm, Common::SeekableReadStream *stream);
	~SpriteSet();

	uint getSpriteCount() { return _spriteInfo.size(); }
	uint getSpriteWidth(uint id) { return _spriteInfo[id]._width; }
	uint getSpriteHeight(uint id) { return _spriteInfo[id]._height; }
	Sprite *getSprite(uint32 spriteId);
	void releaseSprite(Sprite *sprite);

protected:
	AGSEngine *_vm;
	Common::SeekableReadStream *_stream;
	bool _spritesAreCompressed;
	Common::Array<SpriteInfo> _spriteInfo;

	// id->sprite mapping
	Common::HashMap<uint, Sprite *> _sprites;
	// unused sprites, in order of last use
	Common::List<Sprite *> _lruSprites;

	bool loadSpriteIndexFile(uint32 spriteFileID);

	void getNewSizeForSprite(uint id);
};

} // End of namespace AGS

#endif // AGS_SPRITES_H
