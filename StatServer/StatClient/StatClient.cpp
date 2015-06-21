#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include "util.h"

using namespace boost::asio;
using namespace std;

namespace
{
        string getNewCommand()
        {
                cout << "[C] > ";
                string str;
				std::getline(std::cin, str);
				if (!cin.good())
                        throw domain_error("wrong input");
				str.resize(str.size() + 1);
                return str;
        }

        string recieveAnswer(ip::tcp::socket& socket)
        {
                string answer;
                boost::asio::streambuf buffer;
                size_t len = read_until(socket, buffer, '\0');
                if (len)
                        answer += to_string(buffer);
                return answer;
        }
}

int main()
{
  try 
  {
            boost::asio::io_service io;
                ip::tcp::resolver::query query(ip::tcp::v4(), "localhost" ,"");
                ip::tcp::resolver resolver(io);
                ip::tcp::resolver::iterator iterator = resolver.resolve(query);
                ip::tcp::endpoint ep( *iterator );
                ip::tcp::endpoint ep_con(ep.address(), 9999);
        ip::tcp::socket socket(io);
        socket.connect(ep_con);

        while (true) 
        {
						string command = getNewCommand();
                        size_t len = socket.write_some(boost::asio::buffer(command));
                        string answer = recieveAnswer(socket);
                        cout << answer << endl;
						if (equalCommands(QuitCommand, command))
							break;
        }
  }
  catch (exception & e) 
  {
      cout << e.what() << endl;
  }

  return 0;
}