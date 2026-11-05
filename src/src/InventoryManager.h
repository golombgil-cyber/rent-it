#include <string>
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "Item.h"
#ifndef InventoryManager_H
#define InventoryManager_H

class InventoryManager{
    private:
    std::vector<Item>items;
    std::mutex mtx;
    std::condition_variable cv;
    Item& findItemById(int itemId);
    public:
    InventoryManager();
    std::string listItems();
    void borrowItem(int itemId, const std::string& username);
    void returnItem(int itemId, const std::string& username);
    void waitUntilAvailable(int itemId, const std::string& username);
    
};
#endif
