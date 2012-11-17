#include "dynamicarray.h"

namespace AGS {
	
ScriptDynamicArray::ScriptDynamicArray(uint32 elementSize, uint32 elementCount, bool isManaged) {
	this->_elementCount = elementCount;
	this->_elementSize = elementSize;
	this->_isManaged = isManaged;
	this->_array = new Common::Array<RuntimeValue>();
	this->_array->resize(elementCount);
}

ScriptDynamicArray::~ScriptDynamicArray() {
	delete _array;
}

uint32 ScriptDynamicArray::readUint32(uint offset) {
	uint32 objectId = offset / _elementSize;
	if (objectId >= _array->size())
		error("readUint32: offset %d is beyond end of array of %s (size %d)", offset, "dynamic array", _array->size());
	return (*_array)[objectId]._value;
}


/*
ScriptDynamicArray *ccInstance::createDynamicArray(uint32 elementSize, uint32 numElems, bool isManaged) {
	Common::Array<RuntimeValue> foo = new Common::Array<RuntimeValue>();
	foo.resize(numElems);
	ScriptDynamicArray *nu = new ScriptDynamicArray(foo, elementSize);
	return nu;
}	
*/	
};