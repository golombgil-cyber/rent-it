#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <unistd.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string BOLD = "\033[1m";
const std::string RESET = "\033[0m"; 
const std::string BLUE = "\033[34m";
const std::string YELLOW = "\033[33m";

void printLogo() {
    std::cout << BOLD << BLUE<< R"(
 /$$$$$$                                           /$$                                  
|_  $$_/                                          | $$                                  
  | $$   /$$$$$$$  /$$    /$$ /$$$$$$  /$$$$$$$  /$$$$$$    /$$$$$$   /$$$$$$  /$$   /$$
  | $$  | $$__  $$|  $$  /$$//$$__  $$| $$__  $$|_  $$_/   /$$__  $$ /$$__  $$| $$  | $$
  | $$  | $$  \ $$ \  $$/$$/| $$$$$$$$| $$  \ $$  | $$    | $$  \ $$| $$  \__/| $$  | $$
  | $$  | $$  | $$  \  $$$/ | $$_____/| $$  | $$  | $$ /$$| $$  | $$| $$      | $$  | $$
 /$$$$$$| $$  | $$   \  $/  |  $$$$$$$| $$  | $$  |  $$$$/|  $$$$$$/| $$      |  $$$$$$$
|______/|__/  |__/    \_/    \_______/|__/  |__/   \___/   \______/ |__/       \____  $$
                                                                               /$$  | $$
                                                                              |  $$$$$$/
                                                                               \______/                                                
    )" << RESET << "\n";
    std::cout << BOLD << "Welcome to the Inventory System v1.0" << RESET << "\n";
    std::cout << "------------------------------------\n";
}

bool readline(int socket, std::string& line){
    char c;
    line = "";
    while(true){
        int  n = recv(socket, &c ,1 ,0);
        if(n <= 0)return false;
        if(c == '\n')break;
        if (c != '\r') line += c;
    }
    return true;
}

void sendLine(int socket, const std::string& msg){
    std::string sendMsg= msg;
    if (sendMsg.empty()||sendMsg.back()!='\n'){
        sendMsg+='\n';
    }
    send(socket, sendMsg.c_str() , sendMsg.size() , 0);
}

void listenToServer(int socket){
    try{
        std::string serverMsg;
        while(readline(socket, serverMsg)){
            if (serverMsg.find("ERR") == 0) {
                std::cout << RED << "Server: " << serverMsg << RESET << "\n";

            }else if (serverMsg.find("FREE") != std::string::npos) {
                std::cout << "\r" << BLUE<< serverMsg << RESET << "\n";
            }else if (serverMsg.find("BORROWED") != std::string::npos) {
                std::cout << "\r" << YELLOW << serverMsg << RESET << "\n";
            }else if (serverMsg.find("AVAILABLE") != std::string::npos) {
                std::cout << "\r" << BOLD << GREEN << "[*] " << serverMsg << RESET << "\n";
            }else if(serverMsg == "OK BYE"){
                std::cout << "\r" << BOLD << YELLOW << "Disconnected from server." << RESET << "\n";
                exit(0);
            } else {
                std::cout << GREEN << "Server: " << serverMsg << RESET << "\n";
            }

            if(serverMsg == "OK BYE"){
                std::cout << BOLD << "Disconnected from server." << RESET << "\n";
                exit(0);
            }
        }
    }catch(...){
        std::cerr << RED << "Unexpected error while listening to server. \n" << RESET;
    }
    exit(0);
}

int main(int argc, char* argv[]){
    printLogo();

    if(argc != 3){
        std::cerr << RED << "Usage: ./client <server_ip> <port>  \n" << RESET;
        return 1;
    }
    int port = std::stoi(argv[2]);
    if(port <= 0 || port > 65535){
        std::cerr<< RED <<"Invalid port number. \n " << RESET;
        return 1;
    }
    std::string server_ip = argv[1];
    
    int client_fd = socket(AF_INET , SOCK_STREAM,0);
    if(client_fd < 0){
        std::cerr << RED;
        perror("Socket Failed");
        std::cerr << RESET;
        return 1;
   }
   
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port=htons(port);
    if(inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << RED << "Invalid address / Address not supported \n" << RESET;
        return 1;
    }

    if(connect(client_fd,(sockaddr*)&server_addr,sizeof(server_addr)) < 0){
        std::cerr << RED;
        perror("Connect Failed");
        std::cerr << RESET;
        close(client_fd);
        return 1;
    }
    
    std::cout << BOLD << GREEN << "Connected to server " << server_ip << " on port " << port << ". \n" << RESET;
    std::cout << BOLD << "You can now type commands. (Try HELLO <name>)" << RESET << "\n";

    std::thread listenerThread(listenToServer, client_fd);
    listenerThread.detach();

    std::string userInput;
    while(std::getline(std::cin, userInput)){
        if(userInput.empty()){
            continue;
        }
        sendLine(client_fd, userInput);
        
        if(userInput == "QUIT"){
            while(true) sleep(1);
        }
    }
    
    close(client_fd);
    return 0;
}
