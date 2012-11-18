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

#include "common/debug.h"
#include "common/endian.h"
#include "common/stream.h"
#include "common/substream.h"
#include "common/textconsole.h"

#include "ags/resourceman.h"

static const uint32 kClib = MKTAG('C', 'L', 'I', 'B');
static const uint32 k1234 = MKTAG('\x1', '\x2', '\x3', '\x4');
static const uint32 kSigE = MKTAG('S', 'I', 'G', 'E');

// The master archive, which might be attached to the EXE, is always referenced
// as "ac2game.ags" in V2.
static const char *kMasterArchiveName = "ac2game.ags";

static const char *kSecretArchivePassword = "My\x1\xde\x4Jibzle";

namespace AGS {

ResourceManager::MasterArchive::MasterArchive(const Common::String n) : name(n) {
}

ResourceManager::MasterArchive::~MasterArchive() {
	file.close();
}


ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
	for (Common::Array<Common::File *>::iterator a = _archives.begin();
	     a != _archives.end(); ++a) {
		(*a)->close();
		delete *a;
	}
}

void ResourceManager::decryptText(uint8 *str, uint32 max) {
	int passPos = 0;
	while (max-- > 0) {
		*str -= (uint8) kSecretArchivePassword[passPos];
		if (!*str)
			break;

		str++;

		passPos = (passPos + 1) % 11;
	}
}

void ResourceManager::getCString(Common::File &file, Common::String &str) {
	char c = file.readByte();
	while (c) {
		str += c;

		c = file.readByte();
	}
}

bool ResourceManager::openMasterArchive(MasterArchive &master) {
	if (!master.file.open(master.name))
		return false;

	master.version = 0;
	master.offset  = 0;

	// Find the start of the master
	if (master.file.readUint32BE() != kClib) {
		// It's not at the start of the file

		// Seek to the end
		if (!master.file.seek(-16, SEEK_END))
			return false;

		// This could be our potential start offset
		master.offset = master.file.readUint32LE();

		// Make sure the end signature is there
		if (master.file.readUint32BE() != kClib)
			return false;
		if (master.file.readUint32BE() != k1234)
			return false;
		if (master.file.readUint32BE() != kSigE)
			return false;

		// Seek to the start offset
		if (!master.file.seek(master.offset))
			return false;

		// Make sure the master.file signature is there
		if (master.file.readUint32BE() != kClib)
			return false;
	}

	master.file.skip(1); // Unknown
	return true;
}

bool ResourceManager::init(const Common::String &masterArchive) {
	if (masterArchive.empty())
		return false;

	MasterArchive master(masterArchive);

	if (!openMasterArchive(master)) {
		debug(1, "ResourceManager::init(): Failed to open master archive \"%s\"",
		        master.name.c_str());
		return false;
	}

	master.version = master.file.readByte();
	debug(4, "master archive has version %d", master.version);

	// We only support these versions
	if ((master.version !=  6) && (master.version != 10) &&
	    (master.version != 11) && (master.version != 15) &&
	    (master.version != 20) && (master.version != 21)) {

		warning("ResourceManager::setArchive(): Unsupported version %d", master.version);
		return false;
	}

	if (master.version < 10)
		return readArchiveList_v06(master);

	if (master.file.readByte() != 0) {
		warning("ResourceManager::setArchive(): Archive not the first one in the chain");
		return false;
	}

	if (master.version < 20)
		return readArchiveList_v10(master);

	if (master.version == 20)
		return readArchiveList_v20(master);

	if (master.version == 21)
		return readArchiveList_v21(master);

	// Not possible :P
	warning("ResourceManager::init(): unsupported master archive version %d", master.version);

	return false;
}

bool ResourceManager::openArchives(MasterArchive &master,
                                   const Common::Array<Common::String> &archives) {

	// Open the archive files
	_archives.reserve(archives.size());
	for (Common::Array<Common::String>::const_iterator archive = archives.begin();
	     archive != archives.end(); ++archive) {

		_archives.push_back(new Common::File);

		// Make sure that we open the right archive file
		if (archive->equalsIgnoreCase(kMasterArchiveName)) {
			if (!_archives.back()->open(master.name))
				return false;
		} else {
			if (!_archives.back()->open(*archive))
				return false;
		}
	}

	// Fix up the offsets for files in our master archive, which might be attached to the EXE
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file) {
		// Sanity check
		if (file->archive >= _archives.size()) {
			warning("openArchives: archive id %d too high (only %d archives)", file->archive, _archives.size());
			return false;
		}

		if (file->archive == 0)
			file->offset += master.offset;
	}

	return true;
}

bool ResourceManager::readArchiveList_v06(MasterArchive &master) {
	// Oldest Format

	uint8 password = master.file.readByte(); // "Encryption" password

	master.file.skip(1); // Unused

	uint16 fileCount = master.file.readUint16LE();
	_files.resize(fileCount);
	
	Common::Array<Common::String> archives;
	archives.resize(1);
	archives[0] = "no_name";

	master.file.skip(13); // "password dooberry", whatever that's supposed to mean

	// File names
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file) {
		uint8 fileName[14];
		if (master.file.read(fileName, 13) != 13)
			return false;
		fileName[13] = '\0';

		// "Decrypt"
		uint8 length = strlen((const char *) fileName);
		for (uint8 i = 0; i < length; i++)
			fileName[i] -= password;

		file->name = (const char *) fileName;
		debug(5, "file %s", fileName);
	}

	// File sizes
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->size = master.file.readUint32LE();

	// Skip file flags and ratios
	master.file.skip(2 * fileCount);

	// File offsets
	uint32 offset = master.file.pos();
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file) {
		file->offset  = offset;
		file->archive = 0;

		offset += file->size;
	}

	if (master.file.err())
		return false;

	master.file.close();
	
	if (!openArchives(master, archives))
		return false;
	
	createFileMap();
	
	return true;
}

bool ResourceManager::readArchiveList_v10(MasterArchive &master) {
	// New format

	uint32 archiveCount = master.file.readUint32LE();

	Common::Array<Common::String> archives;
	archives.resize(archiveCount);

	// Archive files
	for (Common::Array<Common::String>::iterator archive = archives.begin();
	     archive != archives.end(); ++archive) {

		uint8 archiveName[21];
		if (master.file.read(archiveName, 20) != 20)
			return false;
		archiveName[20] = '\0';

		*archive = (const char *) archiveName;
	}

	uint32 fileCount = master.file.readUint32LE();
	_files.resize(fileCount);

	// File names
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file) {
		uint8 fileName[26];
		if (master.file.read(fileName, 25) != 25)
			return false;
		fileName[25] = '\0';

		if (master.version >= 11)
			decryptText(fileName, 25);

		file->name = (const char *) fileName;
		debug(5, "file %s", fileName);
	}

	// File offsets
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->offset = master.file.readUint32LE();

	// File sizes
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->size = master.file.readUint32LE();

	// File archive indices
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->archive = master.file.readByte();

	if (master.file.err())
		return false;

	master.file.close();

	if (!openArchives(master, archives))
		return false;

	createFileMap();

	return true;
}

bool ResourceManager::readArchiveList_v20(MasterArchive &master) {
	// "New new" format

	warning("UNTESTED: ResourceManager::readArchiveList_v20()");

	uint32 archiveCount = master.file.readUint32LE();

	Common::Array<Common::String> archives;
	archives.resize(archiveCount);

	// Archive files
	for (Common::Array<Common::String>::iterator archive = archives.begin();
	     archive != archives.end(); ++archive) {

		getCString(master.file, *archive);
	}

	uint32 fileCount = master.file.readUint32LE();
	_files.resize(fileCount);

	// File names
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file) {
		uint16 nameLength = master.file.readUint16LE();

		// Yeah, I have no clue either... o_O
		nameLength /= 5;

		uint8 *fileName = new uint8[nameLength + 1];
		if (master.file.read(fileName, nameLength) != nameLength)
			return false;
		fileName[nameLength] = '\0';

		decryptText(fileName, nameLength);

		file->name = (const char *) fileName;

		delete[] fileName;
	}

	// File offsets
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->offset = master.file.readUint32LE();

	// File sizes
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->size = master.file.readUint32LE();

	// File archive indices
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->archive = master.file.readByte();

	if (master.file.err())
		return false;

	master.file.close();

	if (!openArchives(master, archives))
		return false;

	createFileMap();

	return true;
}

static uint16 getPseudoRand(uint32 &seed) {
	seed = seed * 214013L + 2531011L;
	return (seed >> 16) & 0x7fff;
}

static uint32 getUint32Encrypted(uint32 &seed, Common::File &file) {
	char buf[4];
	file.read(buf, 4);
	for (uint i = 0; i < 4; ++i)
		buf[i] -= getPseudoRand(seed);
	return READ_LE_UINT32(buf);
}

static Common::String getEncryptedCString(uint32 &seed, Common::File &file) {
	Common::String str;

	char c = (uint16)file.readByte() - getPseudoRand(seed);
	while (c) {
		str += c;

		c = (uint16)file.readByte() - getPseudoRand(seed);
	}

	return str;
}

bool ResourceManager::readArchiveList_v21(MasterArchive &master) {
	// Similiar to readArchiveList_v20, but encrypted.

	const int RAND_SEED_SALT = 9338638;
	uint32 seed = master.file.readUint32LE() + RAND_SEED_SALT;

	uint32 archiveCount = getUint32Encrypted(seed, master.file);

	Common::Array<Common::String> archives;
	archives.resize(archiveCount);

	// Archive files
	for (Common::Array<Common::String>::iterator archive = archives.begin();
	     archive != archives.end(); ++archive) {

		*archive = getEncryptedCString(seed, master.file);
		debug(5, "archive %s", archive->c_str());
	}

	uint32 fileCount = getUint32Encrypted(seed, master.file);
	_files.resize(fileCount);

	// File names
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file) {
		file->name = getEncryptedCString(seed, master.file);
		debug(5, "file %s", file->name.c_str());
	}

	// File offsets
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->offset = getUint32Encrypted(seed, master.file);

	// File sizes
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->size = getUint32Encrypted(seed, master.file);

	// File archive indices
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		file->archive = (byte)((uint16)master.file.readByte() - getPseudoRand(seed));

	if (master.file.err())
		return false;

	master.file.close();

	if (!openArchives(master, archives))
		return false;

	createFileMap();

	return true;
}

void ResourceManager::createFileMap() {
	for (Common::Array<File>::iterator file = _files.begin(); file != _files.end(); ++file)
		_fileMap.setVal(file->name, &*file);
}

bool ResourceManager::hasFile(const Common::String &file) const {
	return _fileMap.contains(file);
}

Common::SeekableReadStream *ResourceManager::getFile(const Common::String &file) const {
	FileMap::const_iterator it = _fileMap.find(file);
	if (it == _fileMap.end())
		return 0;

	File &f = *it->_value;

	return new Common::SeekableSubReadStream(_archives[f.archive], f.offset, f.offset + f.size);
}

Common::Array<Common::String> ResourceManager::getFilenames() const {
	Common::Array<Common::String> filenames;

	for (Common::Array<File>::const_iterator file = _files.begin(); file != _files.end(); ++file)
		filenames.push_back(file->name);

	return filenames;
}

bool ResourceManager::dumpFile(const Common::String &file) const {
	Common::SeekableReadStream *stream = getFile(file);
	if (!stream)
		return false;

	Common::DumpFile dump;

	if (dump.open(file)) {
		byte buffer[4096];

		while (!stream->eos()) {
			uint32 n = stream->read(buffer, 4096);

			dump.write(buffer, n);
		}

		dump.flush();
		dump.close();
	}

	delete stream;

	return true;
}

} // End of namespace AGS
