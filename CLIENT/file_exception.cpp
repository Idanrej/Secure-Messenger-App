#ifndef FILE_EXCEPTION
#define FILE_EXCEPTION

#include <exception>

class FileException : public std::exception 
{
    public:
        const char * what () const throw ()
        {
            return "Wrong server.info file format";
        }
};

#endif