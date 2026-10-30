#include <functional>
#include "inventory_save_environment.h"
using ObjectGuid = Guid;

struct PacketTestAccessor
{
    std::map<uint32,Player*> players;
    Player* FindPlayer(Guid id) const
    { auto found=players.find(id.value); return found==players.end() ? nullptr : found->second; }
} sObjectAccessor;

// Commit/rollback are a transport boundary. The production scope's actual ACK and
// deactivation are checked separately; this test includes its real packet helper.
struct AtomicPlayerSaveScope
{
    static bool active;
    static std::vector<std::function<void()>> actions;
    static bool Active() { return active; }
    static void DeferAfterCommit(std::function<void()> action)
    { assert(active); actions.push_back(std::move(action)); }
    static void Confirm()
    {
        active=false;
        auto batch=std::move(actions); actions.clear();
        for(auto& action:batch) action();
    }
    static void Discard() { active=false; actions.clear(); }
};
bool AtomicPlayerSaveScope::active=false;
std::vector<std::function<void()>> AtomicPlayerSaveScope::actions;

#include "../../src/game/Objects/PlayerRewardPackets.inc"

void ResetPackets()
{
    AtomicPlayerSaveScope::Discard();
    sObjectAccessor.players.clear();
}

void SerializedValuesWaitForAcknowledgement()
{
    ResetPackets(); Player player(1); sObjectAccessor.players.emplace(1,&player);
    AtomicPlayerSaveScope::active=true;
    for(uint16 opcode: {uint16(10),uint16(20),uint16(30)})
    {
        WorldPacket packet {opcode,{1,2,3}};
        player._SendRewardPacket(packet);
        packet.bytes[0]=99; // no original packet/item memory is captured
    }
    assert(player.session.packets.empty() && AtomicPlayerSaveScope::actions.size()==3);
    AtomicPlayerSaveScope::Confirm();
    assert(player.session.packets.size()==3);
    for(auto const& packet:player.session.packets) assert(packet.bytes==std::vector<uint8>({1,2,3}));
}

void RejectionDropsPacketsAndActorLookupIsDeferred()
{
    ResetPackets(); Player player(1); sObjectAccessor.players.emplace(1,&player);
    AtomicPlayerSaveScope::active=true;
    player._SendRewardPacket({10,{1}}); player._SendRewardPacket({20,{2}});
    AtomicPlayerSaveScope::Discard();
    assert(player.session.packets.empty());
    AtomicPlayerSaveScope::active=true;
    player._SendRewardPacket({30,{3}});
    sObjectAccessor.players.clear();
    AtomicPlayerSaveScope::Confirm();
    assert(player.session.packets.empty()); // no expired actor/session pointer use
}

void LegacyAndBroadcastSemantics()
{
    ResetPackets(); Player player(1); Group group; player.group=&group;
    sObjectAccessor.players.emplace(1,&player);
    player._SendRewardPacket({10,{1}});
    player._SendRewardPacket({20,{2}},true);
    assert(player.session.packets.size()==1 && group.broadcasts.size()==1);
    AtomicPlayerSaveScope::active=true;
    player._SendRewardPacket({30,{3}},true);
    assert(group.broadcasts.size()==1);
    AtomicPlayerSaveScope::Confirm();
    assert(group.broadcasts.size()==2 && group.broadcasts.back().opcode==30);
    AtomicPlayerSaveScope::active=true;
    player._SendRewardPacket({40,{4}},true);
    player.group=nullptr; // resolve current group after ACK; no stored Group pointer
    AtomicPlayerSaveScope::Confirm();
    assert(player.session.packets.size()==2 && player.session.packets.back().opcode==40);
}

int main()
{
    SerializedValuesWaitForAcknowledgement();
    RejectionDropsPacketsAndActorLookupIsDeferred();
    LegacyAndBroadcastSemantics();
    std::cout << "Production reward packet publication tests passed.\n";
}
