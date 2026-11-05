#include "Item.h"
#include <stdexcept>


Item::Item(int id, const std::string& name) {
    this->id = id;
    this->name = name;
    this->isBorrowed = false;
    this->borrowedBy = "";
}
    
    int Item::getId() const {
        return id;
    }
    std::string& Item::getName() {
        return name;
    }
    bool Item::isAvailable() const {
        return !isBorrowed;
    }
    std::string& Item::getBorrower(){
        return borrowedBy;
    }
    void Item::borrow(const std::string& username){
        if (isBorrowed == true){
            throw std::runtime_error("ERR UNAVAILABLE borrowed_by=" + borrowedBy);
        }else{
            isBorrowed = true;
            borrowedBy = username;
        }
    }
    void Item::returnBack(const std::string& username){
        if(isBorrowed == false || borrowedBy != username){
            throw std::runtime_error("ERR PERMISSION not_owner");
        }
        isBorrowed = false;
        borrowedBy = "";
    }
    std::string Item::toString(){
        std::string str = std::to_string(id) + " "+ name;
        if (isBorrowed){
            str += " BORROWED by=" + borrowedBy;
        }else{
            str += " FREE";
        }
        return str;
    }
