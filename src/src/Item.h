#ifndef ITEM_H
#define ITEM_H
#include <string>
#include <iostream>

class Item{
private:
    int id;
    std::string name;
    bool isBorrowed;
    std::string borrowedBy;
public:
    Item(int id,const std::string& name);
    int getId() const;
    std::string& getName();
    bool isAvailable() const;
    std::string& getBorrower();
    void borrow(const std::string& username);
    void returnBack(const std::string& username);
    std::string toString();

};
#endif
