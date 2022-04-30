#include "userData.h"

UserData::UserData(std::string name, std::vector<uint8_t> id): name(name), id(id),
symKeyExists(false), pubKeyExists(false){}

std::string UserData::getName()
{
    return this->name;
}

std::vector<uint8_t> UserData::getId()
{
    return this->id;
}

std::string UserData::getPublicKey()
{
    return std::string(this->publicKey);

}

std::string UserData::getSymKey()
{
    return std::string(this->symKey);
}

void UserData::setPublicKey(std::string pubKey)
{
    this->publicKey = std::string(pubKey);
    this->pubKeyExists = true;

}

void UserData::setSymKey(std::string symKey)
{
    this->symKey = symKey;
    symKeyExists = true;
}