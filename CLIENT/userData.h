#ifndef USER_DATA_H
#define USER_DATA_H

#include <vector>
#include <string>

class UserData
{
private:
    std::string name;
    std::vector<uint8_t> id;
    std::string publicKey;
    std::string symKey;
    bool pubKeyExists;
    bool symKeyExists;

public:
    UserData(std::string name, std::vector<uint8_t> id);
    std::string getName();
    std::vector<uint8_t> getId();
    std::string getPublicKey();
    std::string getSymKey();
    void setPublicKey(std::string pubKey);
    void setSymKey(std::string symKey);
    bool isPubKeyExists(){return this->pubKeyExists;}
    bool isSymKeyExists(){return this->symKeyExists;}

};




#endif