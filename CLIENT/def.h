#ifndef DEF_H
#define DEF_H

#include <string>

#define ID_SIZE 16
#define CLIENT_VER 1
#define PUBLIC_KEY_SIZE 160
#define MAX_NAME_SIZE 255
#define RECIVED_FILE "server.info"
#define OUT_FILE "me.info"
#define REG_10_CODE 1000//register
#define REG_20_CODE 1001//get users list
#define REG_30_CODE 1002//get user pub key
#define REG_40_CODE 1004//get wating msg
#define REG_50_CODE 1003//send message to user
#define REG_51_CODE 1003//ask simetric key
#define REG_52_CODE 1003//send simeric key

#define RSP_10_CODE 2000//register
#define RSP_20_CODE 2001//get users list
#define RSP_30_CODE 2002//get user pub key
#define RSP_40_CODE 2004//get wating msg
#define RSP_50_CODE 2003//send message to user
#define RSP_51_CODE 2003//ask simetric key
#define RSP_52_CODE 2003//send simeric key
#define RSP_ERROR_CODE 9000

#define GET_SYM_MSG_TYPE 1
#define SEND_SYM_MSG_TYPE 2
#define SEND_TEXT_MSG_TYPE 3

#define REGISTER_REQ 10
#define CLIENTS_LST 20 
#define PUB_KEY_REQ 30
#define WAIT_MSG_REQ 40
#define TXT_MSG_REQ 50
#define SYM_KEY_REQ 51
#define SENT_YOUR_SYM_KEY_REQ 52
#define EXIT 0

#define DISPLAY_MSG_MANU   "\nMessageU client at your service.\n\
10) Register\n\
20) Request for clients list \n\
30) Request for public key\n\
40) Request for waiting messages \n\
50) Send a text message \n\
51) Send a request for symmetric key \n\
52) Send your symmetric key \n\
0) Exit client \n\
?"
#define KEY_ERR_MSG "Unvalid key was pressed."
#define MAX_LENGH 1024
#define FAIL -1
#define SUCCESS 0


#define SYM_KEY_REQUEST_CONTENT "Content:\n<Request for symmetric key>\n-----<EOM>-----\n\n"
#define SYM_KEY_SEND_CONTENT "Content:\n<symmetric key received>\n-----<EOM>-----\n\n"


typedef struct __attribute__((__packed__)) 
{
    uint8_t Version;
    uint16_t Code;
    uint32_t PayloadSize;
}recive_hdr;
typedef struct __attribute__((__packed__)) 
{
    uint8_t ClientID[ID_SIZE];
    uint8_t Version;
    uint16_t Code;
    uint32_t PayloadSize;
}req_hdr;

typedef struct __attribute__((__packed__)) 
{
    uint8_t ClientID[ID_SIZE];
    uint32_t MessageId;
    uint8_t MessageType;
    uint32_t MessageSize;
}recive_msg_hdr;

typedef struct __attribute__((__packed__)) 
{
    uint8_t ClientID[ID_SIZE];
    uint8_t MessageType;
    uint32_t ContentSize;
}req_textmsg_hdr;

typedef struct __attribute__((__packed__)) 
{
    uint8_t Name[MAX_NAME_SIZE];
    uint8_t PublicKey[PUBLIC_KEY_SIZE];
}req_register_pay;
typedef struct __attribute__((__packed__)) 
{
    uint8_t ClientID[ID_SIZE];
    uint8_t PublicKey[PUBLIC_KEY_SIZE];
}req_get_pubkeymsg;

typedef struct __attribute__((__packed__)) 
{
    uint8_t ClientID[ID_SIZE];
}recive_register_pay;

typedef struct __attribute__((__packed__)) 
{
    uint8_t ClientID[ID_SIZE];
    uint8_t Name[MAX_NAME_SIZE];
}recive_user_pay;

typedef struct __attribute__((__packed__)) 
{
    uint8_t ClientID[ID_SIZE];
}req_pubkey_pay;



#endif