
#include "clients.h"

/* This function prepare the register user data vector
   return - vector set up with req data

*/
std::vector<uint8_t> Clients::register_10()
{
    // create and set vectors for messages
    std::vector<uint8_t> request(sizeof(req_hdr) + sizeof(req_register_pay));
    req_hdr *hdr = (req_hdr*) &request[0];
    req_register_pay *payload = (req_register_pay*)&request[sizeof(req_hdr)];
    
    // get user name
    std::string name;
    std::cout<<"Enter user name: ";
    std::cin.clear();
    std::getline(std::cin, name,'\n');
    checkName(name);
    this->setName(name);

    // set header
    hdr ->Code = REG_10_CODE;
    std::memcpy(hdr ->ClientID,"",0);
    hdr ->PayloadSize = sizeof(req_register_pay);
    hdr ->Version = CLIENT_VER;

    //set payload
    std::copy(name.begin(),name.end(),payload ->Name);
    payload -> Name[MAX_NAME_SIZE-1] = '\0';

    //create public key
    RSAPrivateWrapper rsapriv;
	std::string pubkey = rsapriv.getPublicKey();	
    std::string privkey = rsapriv.getPrivateKey();	
   

    this -> setPrivKey(privkey);
    this -> setPubKey(pubkey);
    
    std::memcpy(payload ->PublicKey ,pubkey.c_str(),pubkey.size());
    
    return request;
}
/* This function prepare the request clinet list data vector
   return - vector set up with req data
   
*/
std::vector<uint8_t> Clients::register_20()
{

    std::vector<uint8_t> request(sizeof(req_hdr));
    req_hdr *hdr = ( req_hdr*) &request[0];
    
    //set the header
    for (int i = 0; i < ID_SIZE; i++)
    {   
            hdr -> ClientID[i] = this->id[i];
    }
    hdr ->Code = REG_20_CODE;
    hdr ->PayloadSize = 0;
    hdr ->Version = CLIENT_VER;
    
    
    return request;
}
/* This function check if the name is valid - all letters
*  are in ascii range 32 to 100
*  name length is bigger then 1 and smaller then 255
*  recive - name to check
*/
void Clients::checkName(std::string name)
{
    if((name.size() >= 1) && (name.size() < NAME_MAX))
    {
        for (size_t i = 0; i < name.size(); i++)
        {

            if (((char)name[i] < 32) || ((char)name[i] > 127))
            {
                throw WrongNameFormatException();
            }
        }
    }
    else
    {
        throw NameWrongSizeException();
    }
}

/* This function prepare the get public key data vector
*  recive - the current user name so we can ask for his pub key
*  return - vector set up with req data

*/
std::vector<uint8_t> Clients::register_30(std::string &name)
{
    std::vector<uint8_t> request(sizeof(req_hdr) + sizeof(req_pubkey_pay));
    req_hdr *hdr = ( req_hdr*) &request[0];
    req_pubkey_pay *payload = ( req_pubkey_pay*) &request[sizeof(req_hdr)];
    std::cout<<"Enter user name: ";
    std::cin.clear();
    std::getline(std::cin, name,'\n');
    bool found = false;

    for (std::vector<UserData>::iterator user = std::begin(this->usersLst); user != std::end(this->usersLst); ++user)
    {
        if ((*user).getName().compare(name) == 0)
        {
            for (int i = 0; i < ID_SIZE; i++)
            {   
                payload ->ClientID[i] = (*user).getId()[i];
            }
            found = true;
            break;
        }

    }
    if (!found)
    {
        throw UserNotFoundException();
    }
    //set header
    for (int i = 0; i < ID_SIZE; i++)
    {   
            hdr -> ClientID[i] = this->id[i];
    }
    hdr ->PayloadSize = ID_SIZE;
    hdr ->Version = CLIENT_VER;
    hdr ->Code = REG_30_CODE;
    return request;
}
/* This function prepare the get message data vector
   return - vector set up with req data

*/
std::vector<uint8_t> Clients::register_40()
{
    
    std::vector<uint8_t> request(sizeof(req_hdr));
    req_hdr *hdr = ( req_hdr*) &request[0];
    //set the header
    for (int i = 0; i < ID_SIZE; i++)
    {   
        //put the sender id in header
        hdr ->ClientID[i] = this->id[i];
                
    }
    hdr ->Code = REG_40_CODE;
    hdr ->PayloadSize = 0;
    hdr ->Version = CLIENT_VER;
    return request;
}
/* This function prepare the send text message data vector
   return - vector set up with req data

*/
std::vector<uint8_t> Clients::register_50()
{
    std::string name,message;

    //get user name
    std::cout<<"Enter user name you want to send a message: ";
    std::cin.clear();
    std::getline(std::cin,name,'\n');

    //check if user exist in list
    std::vector<UserData>::iterator user;
    if (!checkUserReqName(user, name, true))
    {
        throw UserNotFoundException();
    }
    if (!user->isSymKeyExists())
    {
        throw InvalidSymKeyException();
    }

    //get message
    std::cout<<"Enter a message: ";
    std::cin.clear();
    std::getline(std::cin,message);

    //  Generate a key and initialize an AESWrapper. You can also create AESWrapper with default constructor which will automatically generates a random key.
	AESWrapper aes((const uint8_t *)user->getSymKey().c_str(), AESWrapper::DEFAULT_KEYLENGTH);

	//  encrypt a message (plain text)
	std::string ciphertext = aes.encrypt(message.c_str(), message.length());
	
    std::vector<uint8_t> request(sizeof(req_hdr) + sizeof(req_textmsg_hdr) + ciphertext.size());
    req_hdr *hdr = ( req_hdr*) &request[0];
    req_textmsg_hdr *payloadheader = ( req_textmsg_hdr*) &request[sizeof(req_hdr)];
    
    //set the header
    hdr ->Code = REG_50_CODE;
    for (int i = 0; i < ID_SIZE; i++)
    {   
        //put the sender id in header
        hdr ->ClientID[i] = this->id[i];
                
    }
    hdr ->PayloadSize = sizeof(req_textmsg_hdr) + ciphertext.size();
    hdr ->Version = CLIENT_VER;
    
    //set the payload
    for (int i = 0; i < ID_SIZE; i++)
    {   
         payloadheader ->ClientID[i] = (*user).getId()[i];
    }
    payloadheader ->MessageType = SEND_TEXT_MSG_TYPE;
    payloadheader ->ContentSize = ciphertext.size();
    std::copy(ciphertext.begin(), ciphertext.end(), ((uint8_t *)payloadheader + sizeof(req_textmsg_hdr)));
   
    return request;
}
/* This function prepare the requesy symetric key data vector
   return - vector set up with req data

*/
std::vector<uint8_t> Clients::register_51()
{
    std::string name,message;

    //get user name
    std::cout<<"Enter user name you want to send a message: ";
    std::cin.clear();
    std::getline(std::cin,name,'\n');

    //check if user exist in list
    std::vector<UserData>::iterator user;
    if (!checkUserReqName(user, name, true))
    {
        throw UserNotFoundException();
    }
   
    std::vector<uint8_t> request(sizeof(req_hdr) + sizeof(req_textmsg_hdr) );
    req_hdr *hdr = ( req_hdr*) &request[0];
    req_textmsg_hdr *payloadheader = ( req_textmsg_hdr*) &request[sizeof(req_hdr)];
    
    //setup header
    hdr ->Code = REG_51_CODE;
    for (int i = 0; i < ID_SIZE; i++)
    {   
        //put the sender id in header
        hdr ->ClientID[i] = this->id[i];           
    }
    hdr ->PayloadSize = sizeof(req_textmsg_hdr) ;
    hdr ->Version = CLIENT_VER;
    
    //setup payload
    for (int i = 0; i < ID_SIZE; i++)
    {   
         payloadheader ->ClientID[i] = (*user).getId()[i];
    }
    payloadheader ->MessageType = GET_SYM_MSG_TYPE;
    payloadheader ->ContentSize = 0;

    return request;
}
/* This function check if the user name/id asked for is in the user list
 * recive - user list reference, the name or id to check, byName - will 
 * indicate if we recive name or id to check
 * return - true,if name/id found. false if not found.
*/
bool Clients::checkUserReqName(std::vector<UserData>::iterator &user, std::string identitor, bool byName)
{
    bool found = false;
    for ( user = std::begin(this->usersLst); user != std::end(this->usersLst); ++user)
    {
        if (byName && ((*user).getName().compare(identitor) == 0))
        {
            
            found = true;
            break;
        }
        std::string id;
        for (int i = 0; i < ID_SIZE; i++)
        {   
            id.push_back((*user).getId()[i]);
        }
        if (id.compare(identitor) == 0)
        {
            
            found = true;
            break;
        }
    
    } 
    if (!found)
    {
        return false;
    }
    return true;
}

/* This function prepare the send user symetric key data vector
   return - vector set up with req data

*/
std::vector<uint8_t> Clients::register_52()
{
    std::string name,message;

    //get user name 
    std::cout<<"Enter user name you want to send your symetric key: ";
    std::cin.clear();
    std::getline(std::cin,name,'\n');

    uint8_t key[AESWrapper::DEFAULT_KEYLENGTH]; 
    std::vector<UserData>::iterator user;
    bool found = false;
    size_t rsaSize = RSAPublicWrapper::BITS / 8;

    //check if user exist in list
    if (!checkUserReqName(user, name, true))
    {
        throw UserNotFoundException();
    }
    //check if public key exist
    if (!(*user).isPubKeyExists())
    {
    
        throw NoPubKeyException();
    }

    std::vector<uint8_t> request(sizeof(req_hdr) + sizeof(req_textmsg_hdr) + rsaSize);
    req_hdr *hdr = ( req_hdr*) &request[0];
    req_textmsg_hdr *payloadheader = ( req_textmsg_hdr*) &request[sizeof(req_hdr)];
    
    //set the header
    for (int i = 0; i < ID_SIZE; i++)
    {   
        //put the sender id in header
        hdr ->ClientID[i] = this->id[i];           
    }
    hdr ->Code = REG_52_CODE;
    hdr ->PayloadSize = sizeof(req_textmsg_hdr) + rsaSize;
    hdr ->Version = CLIENT_VER;
    
    //set the payload
    for (int i = 0; i < ID_SIZE; i++)
    {   
         payloadheader ->ClientID[i] = (*user).getId()[i];
    }
    payloadheader ->MessageType = SEND_SYM_MSG_TYPE;
    payloadheader ->ContentSize = rsaSize;

    //encrypt the data
    RSAPublicWrapper rsapub((*user).getPublicKey());
    AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH);
    std::string cipher = rsapub.encrypt((const char*)key, AESWrapper::DEFAULT_KEYLENGTH);
    user->setSymKey(std::string(key, key + AESWrapper::DEFAULT_KEYLENGTH));
    std::copy(cipher.begin(), cipher.end(), ((uint8_t *)payloadheader + sizeof(req_textmsg_hdr)));
    
    return request;
}

/* constructor
 * 
*/
Clients::Clients(): ip(""), port(0), name(""), id(0), readMe(false)
{
    readFile(); // reads the server file.
    this->isRegistered = false;
}
/* check if me.info exist and valid
 * 
*/
bool Clients::checkMeFile()
{
    std::ifstream f(OUT_FILE);
    return f.good();
}
/* This function recive the req number from the menu, and prevent from register
 * new users if me.info exist.
 * recive - the req number
*/
void Clients::checkReq(int req)
{
    if ((req != REGISTER_REQ) && !checkMeFile())
    {
        throw NotRegisterException();
    }
    else if((req == REGISTER_REQ) && checkMeFile())
    {
        throw AlreadyRegisterException();
    }
    else if (checkMeFile() && (req != REGISTER_REQ) && !readMe)
    {
        readMe = true;
        readMeFile();
    }
}
/* This function recive the mesages from the server it parse them and act 
 * acording to the message it got.
 * recive - the replay from server vector
*/
void Clients::
handle40(std::vector<uint8_t> &reply)
{
    recive_hdr *hdr;
    hdr = ( recive_hdr*)&reply[0];
    recive_msg_hdr *payload = (recive_msg_hdr *)&reply[sizeof(recive_hdr)];
    int size = 0;

    while(reply.size() > sizeof(recive_hdr))
    {
        std::vector<UserData>::iterator user;
        std::string id;
        for (int i = 0; i < ID_SIZE; i++)
        {   
            id.push_back(payload ->ClientID[i]);
        }
        if (!checkUserReqName(user, id, false))
        {
            throw UserNotInListException();
        }
        if(payload ->MessageType == GET_SYM_MSG_TYPE)
        {
            std::cout<<"From: <" << user->getName() << ">" << std::endl;
            std::cout << SYM_KEY_REQUEST_CONTENT;

        }
        else if(payload ->MessageType == SEND_SYM_MSG_TYPE  )
        {
            std::cout<<"From: <" << user->getName() << ">" << std::endl;
            std::cout << SYM_KEY_SEND_CONTENT;
            std::string msg = std::string(reply.begin() + sizeof(recive_hdr) + sizeof(recive_msg_hdr),
                                reply.begin() + sizeof(recive_hdr)+sizeof(recive_msg_hdr)+payload ->MessageSize);
            RSAPrivateWrapper rsapriv(this->getPrivKey());
            std::string symkey = rsapriv.decrypt((const char *)msg.c_str(), payload->MessageSize);
            user->setSymKey(symkey);

        }
        else if(payload ->MessageType == SEND_TEXT_MSG_TYPE)
        {
            if(user ->isSymKeyExists())
            {
                std::cout<<"From: <" << user->getName() << ">"<<std::endl;
                std::string msg = std::string(reply.begin() + sizeof(recive_hdr) + sizeof(recive_msg_hdr),
                    reply.begin() + sizeof(recive_hdr)+sizeof(recive_msg_hdr)+payload ->MessageSize);
                    AESWrapper aes((const uint8_t *)user->getSymKey().c_str(), AESWrapper::DEFAULT_KEYLENGTH);
                std::cout << "Content:\n"<< "<" << aes.decrypt((const char*)msg.c_str(), payload->MessageSize) << ">" << "\n" << "-----<EOM>-----" << std::endl;
            }
            else
            {
                std::cout<<"Missing " <<  user->getName() << " symetric key"<< std::endl;
            }    
        }
        reply.erase(reply.begin() + sizeof(recive_hdr),reply.begin() + sizeof(recive_hdr)+sizeof(recive_msg_hdr)+payload ->MessageSize);
        
    }
}
/* This function recive the public key from the server and save them to user
 * recive - the replay from server vector, name of the user we got from client
*/
void Clients::handle30(std::vector<uint8_t> &reply, std::string name)
{
    recive_hdr *hdr;
    hdr = ( recive_hdr*)&reply[0];
    req_get_pubkeymsg *payload = (req_get_pubkeymsg *)&reply[sizeof(recive_hdr)];
    std::cout << std::hex;
    std::vector<UserData>::iterator user = std::begin(this->usersLst);
    for (; user != std::end(this->usersLst); ++user)
    {
        if ((*user).getName().compare(name) == 0)
        {
            user->setPublicKey(std::string(payload->PublicKey, payload->PublicKey + PUBLIC_KEY_SIZE));
            break;
        }
    }
}
/* This function recive the new user data from the server and save the user
 * recive - the replay from server vector
*/
void Clients::handle10(std::vector<uint8_t> &reply)
{
    recive_hdr *hdr;
    hdr = ( recive_hdr*)&reply[0];
    recive_register_pay *payload = (recive_register_pay *)&reply[sizeof(recive_hdr)];
    this ->id.clear();
    this->id.resize(ID_SIZE);
    std::copy(payload -> ClientID, payload -> ClientID + ID_SIZE, this->id.begin());        
    std::ios::fmtflags f(std::cout.flags());
    std::cout << std::hex;
    std::cout << std::endl;
    std::cout.flags(f);
    this->isRegistered = true;
    writeMeFile();
}
/* This function recive the client list from the server print it and save the user
 * data to a vector of users
 * recive - the replay from server vector
*/
void Clients::handle20(std::vector<uint8_t> &reply)
{
    recive_hdr *hdr;
    hdr = ( recive_hdr*)&reply[0];
    for (int i = 0; i < hdr->PayloadSize / sizeof(recive_user_pay); i++)
    {
        recive_user_pay *clientlist = (recive_user_pay *)(&reply[sizeof(recive_hdr)] + (sizeof(recive_user_pay) * i));
        uint8_t *cutter = std::find(clientlist->Name, clientlist->Name + MAX_NAME_SIZE, '\0');
        std::string name(clientlist->Name, cutter);
        std::vector<uint8_t> id(ID_SIZE);
        std::copy(clientlist ->ClientID, clientlist ->ClientID + ID_SIZE, id.begin());
        std::vector<UserData>::iterator user;
        if (!checkUserReqName(user, name, true))
        {
            this->usersLst.push_back(UserData(name, id));
        }

        // prints the user's details
        std::ios::fmtflags f(std::cout.flags());
        std::cout << name << " - " << std::hex;
        for (int i = 0; i < ID_SIZE; i++)
            std::cout << std::setfill('0') << std::setw(2) << (0xFF & clientlist ->ClientID[i]);
        std::cout << std::endl;
        std::cout.flags(f);
    }

}

/* This function handle the user request and direct them to the asked operetion
 * also recive the vector answer from the server and handle them , write to file or
 * print to screen.
 * recive - the requested operetion number
 * return - FAIL - if we have a problem , SUCCESS - if everything is ok
*/
int Clients::request(int req)
{
   
    checkReq(req);
    if(req == REGISTER_REQ)
    {
        readMe = true;
        std::vector<uint8_t> requestVec(sizeof(req_hdr) + sizeof(req_register_pay));
        requestVec = this->register_10();
        std::vector<uint8_t> reply(sizeof(recive_hdr));
        handleReq(reply, requestVec);
        recive_hdr *hdr;
        hdr = ( recive_hdr*)&reply[0];
        if(hdr -> Code == RSP_10_CODE &&(hdr->Version == 1 || hdr->Version == 2))
        {
            handle10(reply);
            std::cout<<"New user sign succesfully" << std::endl;
        }
        else
            return FAIL;

    }
    else if (req == CLIENTS_LST)
    {
        std::vector<uint8_t> requestVec(sizeof(req_hdr) );
        requestVec.clear();
        requestVec = this->register_20();
        std::vector<uint8_t> reply(sizeof(recive_hdr));
        handleReq(reply, requestVec);
        recive_hdr *hdr;
        hdr = ( recive_hdr*)&reply[0];
        if(hdr -> Code == RSP_20_CODE &&(hdr->Version == 1 || hdr->Version == 2))
             handle20(reply);
        else
            return FAIL;
       

    }
    else if (req == PUB_KEY_REQ)
    {
        
        std::vector<uint8_t> requestVec(sizeof(req_hdr) + sizeof(req_pubkey_pay));
        std::string name;
        requestVec = this->register_30(name);
        std::vector<uint8_t> reply;
        handleReq(reply, requestVec);
        recive_hdr *hdr;
        hdr = ( recive_hdr*)&reply[0];
        if(hdr -> Code == RSP_30_CODE &&(hdr->Version == 1 || hdr->Version == 2))
        {
             handle30(reply, name);
             std::cout<<"Public key recived and saved" << std::endl;
        }
        else
            return FAIL;
        

    }
    else if(req == WAIT_MSG_REQ)
    {
        std::vector<uint8_t> requestVec(sizeof(req_hdr));
        requestVec = this->register_40();
        std::vector<uint8_t> reply(sizeof(recive_hdr));
        if (this -> usersLst.size() ==0)
        {
            throw NoUserListException();
        }
        handleReq(reply, requestVec);
        recive_hdr *hdr;
        hdr = ( recive_hdr*)&reply[0];
        if(hdr -> Code == RSP_40_CODE &&(hdr->Version == 1 || hdr->Version == 2))
             handle40(reply);
        else
            return FAIL;
        
    }
    else if(req == TXT_MSG_REQ)
    {
        std::vector<uint8_t> requestVec(sizeof(req_hdr) + sizeof(req_textmsg_hdr));
        requestVec = this->register_50();
        std::vector<uint8_t> reply(sizeof(recive_hdr));
        handleReq(reply, requestVec);
        recive_hdr *hdr;
        hdr = ( recive_hdr*)&reply[0];
        if(!(hdr -> Code == RSP_50_CODE &&(hdr->Version == 1 || hdr->Version == 2)))
            return FAIL;
        std::cout<<"Message sent successfully" << std::endl;
    }
    else if(req == SYM_KEY_REQ)
    {
        std::vector<uint8_t> requestVec(sizeof(req_hdr) + sizeof(req_textmsg_hdr));
        requestVec = this->register_51();
        std::vector<uint8_t> reply(sizeof(recive_hdr));
        handleReq(reply, requestVec);
        recive_hdr *hdr;
        hdr = ( recive_hdr*)&reply[0];
        if(!(hdr -> Code == RSP_51_CODE &&(hdr->Version == 1 || hdr->Version == 2)))
            return FAIL;
        std::cout<<"Symetric key requesy sent successfully" << std::endl;
    }
    else if(req == SENT_YOUR_SYM_KEY_REQ)
    {
        std::vector<uint8_t> requestVec(sizeof(req_hdr) + sizeof(req_textmsg_hdr));
        requestVec = this->register_52();
        std::vector<uint8_t> reply(sizeof(recive_hdr));
        handleReq(reply, requestVec);
        recive_hdr *hdr;
        hdr = ( recive_hdr*)&reply[0];
        if(!(hdr -> Code == RSP_52_CODE &&(hdr->Version == 1 || hdr->Version == 2)))
            return FAIL;
        std::cout<<"Symetric key sent successfully" << std::endl;
    }
    else if(req ==  RSP_ERROR_CODE)
    {
        std::cout<<"server responded with eror"<<std::endl;
        return FAIL;
    }
    
    return SUCCESS; 
}

/* 
 * This function read the me.info file, and fill the right vars.
*/
void Clients::readMeFile()
{
    std::ifstream file;
    file.open(OUT_FILE, std::ifstream::in);
    if (!file.good())
    {
        throw FileException();
    }
    std::string tmp;
    std::getline(file, this->name, '\n');
    std::getline(file, tmp, '\n');

    // convert tmp to this->id vector.
    for (size_t i = 0; i < tmp.size(); i += 2) {
            std::string byte = tmp.substr(i, 2);
            this->id.push_back(std::strtoul(byte.c_str(), NULL, ID_SIZE));
    }
    // get privkey
    std::string line;
    while (std::getline(file, line, '\n'))
    {
        this->privkey += line;
    }
    RSAPrivateWrapper rsapriv(Base64Wrapper::decode(this->privkey));
    this->setPrivKey( rsapriv.getPrivateKey());
                
    
}

/* 
 * This function read the server.info file
*/
void Clients::readFile()
{
    std::string port;
    std::string ip;
    std::ifstream file;
    file.open(RECIVED_FILE, std::ifstream::in);
    if (!file.good())
    {
        throw FileException();
    }
    std::getline(file, ip, ':');
    std::getline(file, port, '\n');
    file.close();
    if (checkRecFileValidity(port, ip) == FAIL)
    {
        
        throw FileException();
    }
    this->port = static_cast<uint16_t>(std::stoi(port));
    this->ip = std::string(ip);
    if ((this->port <= 0) || (this->port >= 65535)) 
    {
        throw FileException();
    }
}

/* 
 * This function check validety of port and ip.
*/
int Clients::checkRecFileValidity(std::string port, std::string ip)
{
    if ((std::count(ip.begin(), ip.end(), '.') != 3) || !checkIfNum(port))
    {
        return FAIL;
    }
    return SUCCESS;
}
/* This function check if the port has only numbers
 * recive - port
 * return - true if number, false if not
*/
bool Clients::checkIfNum(std::string port)
{
    return std::all_of(port.begin(), port.end(), ::isdigit);
}

/* This function send the data to the server, and recive the data from the server
 * recive - reply - will get the data from the server, 
 *        - request - hold the data we want to send to the server
*/
void Clients::handleReq(std::vector<uint8_t> &reply, std::vector<uint8_t> request)
{
        boost::array<uint8_t, 1024> buf;
        boost::asio::io_context io_context;
        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::system::error_code error;
        boost::asio::connect(s, resolver.resolve(this->ip, std::to_string(this->port)), error);

        boost::asio::write(s, boost::asio::buffer(request, request.size()),error);
        reply.clear();
         while (error!= boost::asio::error::eof)
        {
            size_t reply_length = s.read_some(boost::asio::buffer(buf), error);
            
            reply.resize(reply.size() + reply_length );
            std::copy(buf.begin(),buf.begin() + reply_length, reply.end() - reply_length );
            recive_hdr *hdr;
            hdr = ( recive_hdr*)&reply[0];
            
            if(reply_length<buf.size())
            {
                break;
            }
            buf.assign(0);//clear the buf
        }
}
/* This function write to the .me file
*/
void Clients::writeMeFile()
{
    std::ofstream file;
    file.open(OUT_FILE, std::ifstream::out);
    file << this->name << "\n";
    file << std::hex;
    for (size_t i = 0; i < ID_SIZE; i++)
            file << std::setfill('0') << std::setw(2) << (0xFF & this->id[i]) << (((i + 1) % 16 == 0) ? "\n":"");
    std::string privkey = Base64Wrapper::encode(this->getPrivKey());
    file << privkey;
    file.close();
}
