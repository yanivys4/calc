#include "../include/boost_client.hpp"

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }
    boost::system::error_code error;
    boost::system::error_code ignored_error;
    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
      resolver.resolve(argv[1], "http");

    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

    bool stop = false;
    while (!stop && !_kbhit()) {
      int char_value = getCharInput();
      
      std::string msg = std::to_string(char_value);
      boost::asio::write(socket, boost::asio::buffer(msg), ignored_error);
      if(char_value == '=')
      {
        clearOutput();
        boost::array<char, 128> buf;
        // read from server
        size_t len = socket.read_some(boost::asio::buffer(buf), error);
        
        // print server msg 
        std::cout << "result= ";
        std::cout.write(buf.data(),len);
      }
      else if(char_value == 's')
      {
        stop = true;
      }else{
        print(static_cast<char>(char_value));
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}