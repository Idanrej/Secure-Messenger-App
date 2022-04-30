
#include "clients.h"
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>

#include "file_exception.cpp"

using boost::asio::ip::tcp;
#include "def.h"
#include "client_exception.cpp"



/* This function print the menu to the screen check if it is a 
 * valid option and return the choosen number
 * recive - 
 * return - the selected number
*/
int cosole()
{
    std::string num;
    std::cout << DISPLAY_MSG_MANU << std::endl;
    std::getline(std::cin,num);
    int retNum;
    try
    {
        retNum = std::stoi(num);
    }
    catch(const std::exception& e)
    {
        std::cerr << KEY_ERR_MSG << '\n';
        return -1;
    }
    return retNum;
    
} 

int main(int argc, char* argv[])
{
    Clients client;

    try
    {
        for( ; ; )
        {
            int req = cosole();            
            if(req == 0)
            {
                break;
            }
            try{
                int p = client.request(req);
                if(p<0)
                {
                    std::cout<<"eror was made action wasen't coplete";
                }
            }
            catch(RegisterException &e)
            {
                std::cout << e.what() << std::endl;
            }            
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}


