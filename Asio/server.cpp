#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
  std::time_t now = time(0);
  return ctime(&now);
}

int main()
{
  try 
  {
    std::cout << "Initiating server..." << std::endl;
	boost::asio::io_service io;
	tcp::acceptor acceptor (io, tcp::endpoint(tcp::v4(), 2001));

    while (true) 
	{
      tcp::socket socket (io);
      acceptor.accept(socket);

      std::string message = make_daytime_string();

      boost::system::error_code ignored_error;
      boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
    }
  }
  catch (std::exception & e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}