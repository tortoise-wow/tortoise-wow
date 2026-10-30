// Focused ownership regression using the existing boundary fixture and ACTUAL
// PreparedMail.inc, not copied mail persistence logic.
#define main ExistingPreparedMailSuiteMain
#include "prepared_mail_test.cpp"
#undef main
int main(){
    Fixture fixture;
    Item* inventoryItem=new Item(42,1,ObjectGuid(1));
    inventoryItem->state=ITEM_UNCHANGED;
    uint32 guid=inventoryItem->GetGUIDLow();
    fixture.player.inventory[guid]=inventoryItem;
    unsigned deleted=Item::deleted;
    MailDraft("hook self-inventory pointer").AddItem(inventoryItem).SendMailTo(
        MailReceiver(&fixture.player),MailSender(MAIL_CREATURE,11u));
    bool rejected=fixture.collector.failed&&CharacterDatabase.queued.empty()&&Item::deleted==deleted;
    if(!rejected){
        CharacterDatabase.Ack(false);
        fixture.collector.mails.clear();
        std::cout<<"COUNTEREXAMPLE: clean unbagged actor inventory item passed admission; failed ACK deleted="
            <<(Item::deleted-deleted)<<", inventory still holds pointer="
            <<(fixture.player.GetItemByGuid(ObjectGuid(guid))==inventoryItem)<<'\n';
        return 1;
    }
    Check(fixture.player.GetItemByGuid(ObjectGuid(guid))==inventoryItem,"actor inventory ownership retained");
    fixture.player.inventory.clear();delete inventoryItem;
    std::cout<<"PASS clean actor inventory member refused before mail pointer adoption\n";
    return 0;
}
