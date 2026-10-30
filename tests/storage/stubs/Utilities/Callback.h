#pragma once
class QueryResult;
namespace MaNGOS {
class IQueryCallback {
public:
    virtual ~IQueryCallback() = default;
    void SetResult(QueryResult*) {}
    bool IsThreadSafe() { return false; }
    void Execute() {}
};
}
