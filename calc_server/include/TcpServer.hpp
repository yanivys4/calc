
#pragma once
#include "./calc/calc.hpp"
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


class TcpServer
{
  public:
    
    TcpServer(int port,boost::asio::io_context &io_context)
     :mSocket(io_context),mAccaptor(io_context,tcp::endpoint(tcp::v4(),port)),
      mError(){}

     void start_accept(boost::asio::io_context &io_context){
        mSocket = tcp::socket(io_context);
        mAccaptor.accept(mSocket);
     };

     void write(const std::string &msg){
        
        boost::asio::write(mSocket,boost::asio::buffer(msg),mError);
     };

     std::string read(){
       boost::array<char, 128> buf{};
       mSocket.read_some(boost::asio::buffer(buf), mError);
       return std::string(buf.begin(),buf.end());
     };

  private:
  
  tcp::socket mSocket;
  tcp::acceptor mAccaptor;
  boost::system::error_code mError;
  
};
