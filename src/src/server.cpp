#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <unistd.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "InventoryManager.h"

bool readline(int client_fd , std::string& line){
    char c;
    line = "";
    while(true){
        int  n = recv(client_fd, &c ,1 ,0);
        if(n <= 0)return false;
        if(c == '\n')break;
        if (c != '\r') line += c;
        
    }
    return true;
}

void sendLine(int client_fd, const std::string& msg){
    std::string sendMsg= msg;
    if (sendMsg.empty()||sendMsg.back()!='\n'){
        sendMsg+='\n';
    }
    send(client_fd, sendMsg.c_str() , sendMsg.size() , 0);
}

void handle_client(int client_fd, InventoryManager& inventory){
    std::string username = "";
    bool connectedcheck = false;
    try{
        std::string commandLine;
        while(readline(client_fd, commandLine)){
            std::stringstream ss(commandLine);
            std::string cmd;
            ss >> cmd;
            
            if(cmd == "QUIT"){
                sendLine(client_fd , "OK BYE");
            break;
            }
            if(cmd == "HELLO"){
                std::string usr;
                ss >> usr;
                if(usr.empty()){
                    sendLine(client_fd , "ERR PROTOCOL missing_username");
                    continue;
                }
                bool valid = true;
                for(char c : usr) {
                    if(!isalnum(c)){
                        valid = false;
                        break;
                    }
                }
                if(!valid){
                    sendLine(client_fd , "ERR PROTOCOL invalid_username");
                    continue;
                }
                username = usr;
                connectedcheck = true;
                sendLine(client_fd , "OK HELLO " + username);
                continue;
            }
            if(!connectedcheck){
                sendLine(client_fd,"ERR STATE not_authenticated");
                continue;
            }

            try{

                if(cmd == "LIST"){
                        std::string itemlist = inventory.listItems();
                        sendLine(client_fd, itemlist);
                    }
            
                else if(cmd == "BORROW"){
                    int id;
                    if(!(ss>> id)){
                        throw std::invalid_argument("ERR PROTOCOL invalid_id");
                    }
                    inventory.borrowItem(id,username);
                    sendLine(client_fd, "OK BORROWED " + std::to_string(id));
                }
                else if(cmd=="RETURN"){
                    int id;
                    if(!(ss>> id)){
                        throw std::invalid_argument("ERR PROTOCOL invalid_id");
                    }
                    inventory.returnItem(id,username);
                    sendLine(client_fd, "OK RETURNED " + std::to_string(id));
                }
                else if(cmd == "WAIT"){
                    int id; 
                    if(!(ss >> id)){
                        throw std::invalid_argument("ERR PROTOCOL invalid_id");
                    }
                    inventory.waitUntilAvailable(id ,username);
                    sendLine(client_fd, "OK AVAILABLE " + std::to_string(id));
                }
                else{
                    throw std::invalid_argument("ERR PROTOCOL invalid_command");
                }
            }catch (const std::exception& e){
                
                sendLine(client_fd, e.what());
            }
        }
    }catch (...){
        std::cerr << " Unexepted error in thread. \n";
    }
    sleep(1);
    close(client_fd);
    std::cout << "Client disconnected. \n";
}

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cerr << "Usage: ./ server <port>  \n" ;
        return 1;
    }
    int port = std::stoi(argv[1]);
    if(port <= 0 || port > 65535){
        std::cerr<<"Invalid port number. \n ";
        return 1;
    }
    InventoryManager inventory;
    int server_fd = socket(AF_INET , SOCK_STREAM,0);
    if(server_fd < 0){
        perror("Socket Failed");
        return 1;
    }
    
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    
    if(bind(server_fd,(sockaddr*)&server_addr,sizeof(server_addr)) < 0){
        perror("bind Failed");
        close(server_fd);
        return 1;
    }

    if(listen (server_fd,5) < 0){
        perror("listen Failed");
        close(server_fd);
        return 1;
    }
    std::cout<<"Server is lisetening on port "<<port<< "... \n";
    while(true){
        int client_fd = accept(server_fd,nullptr,nullptr);
        if(client_fd < 0){
            perror("Accept Failed");
            continue;
        }
        std::cout<< "client: "<<client_fd<<" connected \n";
        std::thread t(handle_client,client_fd, std::ref(inventory));
        t.detach();
    }
        close(server_fd);
        return 0;
}


