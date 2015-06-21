#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;

int main(int argc, char * argv[])
{
  // Daytime
  try 
  {

	boost::asio::io_service io;
	ip::tcp::endpoint ep( ip::address::from_string("172.16.35.113"), 2001);
    ip::tcp::socket socket(io);
	socket.connect(ep);

    while (true) 
	{
      boost::array<char, 128> buffer;
      boost::system::error_code error;
      size_t len = socket.read_some(boost::asio::buffer(buffer), error);

      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error) 
	    throw boost::system::system_error(error); // Some other error.
      
      std::cout.write(buffer.data(), len);
    }
  }
  catch (std::exception & e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}