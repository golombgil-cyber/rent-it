#include "InventoryManager.h"

InventoryManager::InventoryManager() {
items.emplace_back(1, "Camera");
items.emplace_back(2, "Tripod");
items.emplace_back(3, "Laptop");
items.emplace_back(4, "Projector");
items.emplace_back(5, "Microphone");
items.emplace_back(6, "Speaker");
items.emplace_back(7, "HDMI_Cable");
items.emplace_back(8, "Ethernet_Cable");
items.emplace_back(9, "Keyboard");
items.emplace_back(10, "Mouse");
items.emplace_back(11, "Monitor");
items.emplace_back(12, "USB_Hub");
items.emplace_back(13, "Power_Bank");
items.emplace_back(14, "Router");
items.emplace_back(15, "VR_Headset");
}
Item& InventoryManager::findItemById(int itemId) {
    for(auto& item : items) {
        if(item.getId() == itemId) {
            return item;
        }
    }
    throw std::runtime_error("ERR NOT_FOUND item");
}
std::string InventoryManager::listItems() {
    std::lock_guard<std::mutex> lock(mtx);
    std::string response = "LIST " + std::to_string(items.size()) + "\n";
    for(auto& item : items) {
        response += item.toString() + "\n";
    }
    return response;
}
void InventoryManager::borrowItem(int itemId, const std::string& username) {
    std::lock_guard<std::mutex> lock(mtx);
    Item& item = findItemById(itemId);
    item.borrow(username);
}
void InventoryManager::returnItem(int itemId, const std::string& username){
    std::lock_guard<std::mutex> lock(mtx);
    Item& item = findItemById(itemId);
    item.returnBack(username);
    cv.notify_all();
}
void InventoryManager::waitUntilAvailable(int itemId, const std::string& username){
    std::unique_lock<std::mutex> lock(mtx);
    Item& item = findItemById(itemId);
    if (!item.isAvailable() && item.getBorrower() == username){
        throw std::runtime_error("ERR DEADLOCK item");
    }
    while(!item.isAvailable()){
        cv.wait(lock);
    }
}
