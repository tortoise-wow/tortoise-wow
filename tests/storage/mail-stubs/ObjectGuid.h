#pragma once
#include "Common.h"
class ObjectGuid {
    uint32 value=0;
public:
    ObjectGuid()=default;
    explicit ObjectGuid(uint32 id):value(id){}
    uint32 GetCounter() const { return value; }
    bool IsEmpty() const { return value==0; }
    bool operator==(ObjectGuid const& other) const { return value==other.value; }
    bool operator!=(ObjectGuid const& other) const { return value!=other.value; }
    bool operator<(ObjectGuid const& other) const { return value<other.value; }
};
