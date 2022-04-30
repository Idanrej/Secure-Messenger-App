#ifndef CLIENTS_H
#define CLIENTS_H

// #include "client_req.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <boost/array.hpp>
#include "def.h"
#include "file_exception.cpp"
#include "client_exception.cpp"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"
#include "AESWrapper.h"
using boost::asio::ip::tcp;
#include "userData.h"

class Clients
{
private:
    
    std::string name; 
    std::vector<uint8_t> id;
    std::string pubkey;
    std::string privkey;
    std::string ip;
    uint16_t port;
    bool readMe;

    bool isRegistered;
    void readFile();
    int checkRecFileValidity(std::string port, std::string ip);
    bool checkIfNum(std::string port);
    void handleReq(std::vector<uint8_t> &reply, std::vector<uint8_t> request);
    void writeMeFile();
    bool checkMeFile();
    void readMeFile();
    void checkReq(int req);
    std::vector<UserData> usersLst;
    void copy(std::vector<uint8_t> &vec1, std::vector<uint8_t> &vec2);
    void checkName(std::string name);
    bool checkUserReqName(std::vector<UserData>::iterator &user, std::string identitor, bool byName);
    // void copyStr(std::string &copyTo, char* copyFrom);
    void handle10(std::vector<uint8_t> &reply);
    void handle20(std::vector<uint8_t> &reply);
    void handle40(std::vector<uint8_t> &reply);
    void handle30(std::vector<uint8_t> &reply, std::string name);



public:
    Clients();
    int request(int req);
    std::string getName(){return this->name;}
    std::vector<uint8_t> getId() {return this->id;}
    std::string getPubKey(){return this->pubkey;}
    std::string getPrivKey(){return this->privkey;}

    void setName(std::string name){ this->name = name;}
    void setId(std::vector<uint8_t> id){this ->id = id;}
    void setPubKey(std::string pubkey){ this->pubkey = std::string(pubkey);}
    void setPrivKey(std::string privkey){ this->privkey = std::string(privkey);}
    
    std::vector<uint8_t>  register_10();
    std::vector<uint8_t>  register_20();
    std::vector<uint8_t>  register_30(std::string &name);
    std::vector<uint8_t>  register_40();
    std::vector<uint8_t>  register_50();
    std::vector<uint8_t>  register_51();
    std::vector<uint8_t>  register_52();
    
    int cosole();
};

#endif