#include <iostream>
#include <winsock2.h>
#include <mysql.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "util.h"

using namespace boost::asio;
using namespace std;

namespace
{
	MYSQL *connection = nullptr;

	class commandParser
	{	
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

		void processQuit()
		{
			answer.assign("[S] > OK");
			//mustCloseCon = true;
		}

		void processData(tokenizer::iterator& tok_iter, const tokenizer::iterator& end_iter)
		{
			++tok_iter;
			if (tok_iter == end_iter)
			{
				answer.assign("[S] > ERR (Address is not specified)");
				return;
			}
			
			boost::system::error_code error;
			string IPadrr = removeEndLSymbol(*tok_iter);
			ip::address_v4 addr = ip::address_v4::from_string(IPadrr, error);
			if (error)
			{
				answer.assign("[S] > ERR (Wrong IP-address)");
				return;
			}

			++tok_iter;
			if (tok_iter == end_iter)
			{
				answer.assign("[S] > ERR (Amount of bytes is not specified)");
				return;
			}

			size_t bytesAmount = 0;
			try
			{
				bytesAmount = boost::lexical_cast<size_t>(removeEndLSymbol(*tok_iter));
			}
			catch(boost::bad_lexical_cast&)
			{
				answer.assign("[S] > ERR (Illegal amount of bytes specified)");
				return;
			}
			string querry("insert into stat values (" + boost::lexical_cast<string>(addr.to_ulong()) + ", " + boost::lexical_cast<string>(bytesAmount) + ");");
			int query_state = mysql_query(connection, querry.c_str());
			if (query_state !=0) 
			{
					answer.assign("[S] > ERR " + string(mysql_error(connection)));
					return;
			}
			answer.assign("[S] > OK");
		}

		void processStat()
		{
			int query_state = mysql_query(connection, "select * from stat;");
			if (query_state !=0) 
			{
					answer.assign("[S] > ERR " + string(mysql_error(connection)));
					return;
			}

			answer += "[S] > OK";
			answer += '\n';
			MYSQL_RES *result = mysql_store_result(connection);
			MYSQL_ROW row;
			while (( row = mysql_fetch_row(result)) != NULL) 
			{
				answer += "[S] > ";
				answer += string(row[0]);
				answer += ' ';
				answer += string(row[1]);
				answer += '\n';	
			}
			answer += "[S] > END";
			answer += '\n';
			mysql_free_result(result);
		}
	public:
		commandParser(boost::asio::streambuf& command) //: mustCloseCon(false)
		{
			string strCommand = to_string(command);
			boost::char_separator<char> sepSpace(" ");
			tokenizer tokens(strCommand, sepSpace);
			
			auto tok_iter = tokens.begin();
			if (tok_iter == tokens.end())
			{
				answer.assign("[S] > ERR (No command)");
			}
			else
			{
				if (equalCommands(QuitCommand, *tok_iter))
					processQuit();
				else if (equalCommands(DataCommand, *tok_iter))
					processData(tok_iter, tokens.end());
				else if (equalCommands(StatCommand, *tok_iter))
					processStat();
				else
					answer.assign("[S] > ERR (Unknown command)");
			}
			answer.resize(answer.size() + 1);
		}

		bool mustCloseConnectiont()
		{
			return false;//mustCloseCon;
		}

		string getAnswer()
		{
			return answer;
		}
	private:
		//bool mustCloseCon;
		string answer;
	};
}

int main()
{
    MYSQL mysql;
    mysql_init(&mysql);

    connection = mysql_real_connect(&mysql, "localhost", "root", "dendi", "StatServer", 3306, 0, 0);
    if (connection == NULL) 
    {
            std::cout << mysql_error(&mysql) << std::endl;
            return 1;
    }
        
    try 
    {
      std::cout << "Initiating server..." << std::endl;
      boost::asio::io_service io;
      ip::tcp::acceptor acceptor (io, ip::tcp::endpoint(ip::tcp::v4(), 9999));

      ip::tcp::socket socket (io);
      acceptor.accept(socket);

      while (true) 
      {
        boost::asio::streambuf buffer;
        size_t len = read_until(socket, buffer, '\0');

        if (len)
		{
			/*std::cout << " Receive message: ";
            std::cout.write(buffer.data(), len) << std::endl;
            std::cout << " Send message: OK" << std::endl;*/
			commandParser parser(buffer);
			
			string answer = parser.getAnswer();
			socket.write_some(boost::asio::buffer(answer));
			/*
			if (parser.mustCloseConnectiont())
				break;
			*/
            //write(socket, buffer);
		}
      }
    }
    catch (std::exception & e) 
    {
      std::cerr << e.what() << std::endl;
    }

    mysql_close(connection);
    return 0;
}