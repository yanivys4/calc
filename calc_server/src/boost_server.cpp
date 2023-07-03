
#include "../include/boost_server.hpp"


void runCommand(const int & command,SM * p2sm){

	if (command >= 48 && command <= 57) {
		p2sm->process_event(Calc::operand_e{ static_cast<char>(command)});
	}
	else if (command == '=') {
		p2sm->process_event(Calc::equal_e{});
	}
	else {
		switch (command) {
		case '*':
			p2sm->process_event(Calc::operator_e{ Calc::Operation::Mul });
			break;
		case '-':
			p2sm->process_event(Calc::operator_e{ Calc::Operation::Sub });
			break;
		case '+':
			p2sm->process_event(Calc::operator_e{ Calc::Operation::Add });
			break;
		case '/':
			p2sm->process_event(Calc::operator_e{ Calc::Operation::Div });
			break;
		case 'c':
			p2sm->process_event(Calc::clear{});
			break;
		case 's':
			p2sm -> process_event(Calc::off{});
			break;
		}
	}	
}
int main()
{
  using namespace sml;
  
    boost::asio::io_context io_context;
	while(true){
		try
  		{
			TcpServer server(80,io_context);
			TcpServer * ptoserver = &server; 
			Calc::variables v{ptoserver};
			SM sm{v};
			server.start_accept(io_context);
			
			for (;;)
			{
				
				// read from client
				std::string command = server.read();
				
				runCommand(std::stoi(command),&sm);

			}

			sm.process_event(Calc::off{});
			assert(sm.is(X));	
  		}
		catch (std::invalid_argument){
			std::cout << "client stopped" << std::endl;
		}
  		catch (std::exception& e)
		{	
		std::cerr << e.what() << std::endl;
		}
	}  

  return 0;
}