#ifndef CLIENT_EXCEPTION
#define CLIENT_EXCEPTION

#include <exception>

class RegisterException : public std::exception 
{
    public:
        const char * what () const throw ()
        {
            return "registered!\n";
        }
};

class NoPubKeyException : public RegisterException
{
    public:
        const char * what () const throw ()
        {
            return "No public key for this user\n";
        }
};

class NotRegisterException : public RegisterException
{
    public:
        const char * what () const throw ()
        {
            return "The user is not registered!\n";
        }
};

class AlreadyRegisterException : public RegisterException 
{
    public:
        const char * what () const throw ()
        {
            return "The user is already registered!\n";
        }
};

class WrongNameFormatException : public RegisterException 
{
    public:
        const char * what () const throw ()
        {
            return "Wrong name format\n";
        }
}; 

class NameWrongSizeException : public RegisterException 
{
    public:
        const char * what () const throw ()
        {
            return "Wrong name size\n";
        }
}; 

class UserNotFoundException : public RegisterException 
{
    public:
        const char * what () const throw ()
        {
            return "Unknown user name \n";
        }
};

class InvalidSymKeyException : public RegisterException 
{
    public:
        const char * what () const throw ()
        {
            return "No symetric key avilable for this user \n";
        }
};

class NoUserListException : public RegisterException 
{
    public:
        const char * what () const throw ()
        {
            return "Please update your client list if you want to recive messages \n";
        }
};

class UserNotInListException : public RegisterException 
{
    public:
        const char * what () const throw ()
        {
            return "User not in list \n";
        }
};


#endif