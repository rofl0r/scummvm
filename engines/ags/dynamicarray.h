#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include "scriptobj.h"
#include "script.h"

namespace AGS {

/*class ScriptDynamicArray : public ScriptObjectArray<AGS::RuntimeValue> {
public:
	ScriptDynamicArray(Common::Array<AGS::RuntimeValue> *array, uint32 elementSize)
	: ScriptObjectArray(array, elementSize, "dynarr") {}
};*/
class ScriptDynamicArray : public ScriptObject {
public:
	virtual const char *getObjectTypeName() { return "ScriptDynamicArray"; }
	virtual bool isOfType(ScriptObjectType objectType) { return (objectType == sotDynamicArray); }
	ScriptDynamicArray(uint32 elementSize, uint32 elementCount, bool isManaged);
	virtual ~ScriptDynamicArray();
	virtual uint32 readUint32(uint offset);
public:
	bool _isManaged;
	uint32 _elementSize;
	uint32 _elementCount;
	Common::Array<RuntimeValue> *_array;
};

};

#endif
