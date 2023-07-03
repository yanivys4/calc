#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include <queue>
#include <algorithm>
#include <boost/sml.hpp>
#include <boost/asio.hpp>
#include <cassert>
#include "../TcpServer.hpp"


namespace sml = boost::sml;

namespace Calc {
	enum class Operation {
		Add,
		Sub,
		Mul,
		Div
	};

	struct variables {
		
		
		std::string a = "0";
		std::string b = "0";
		float current_result = 0.f; // used for appending calcs
		Operation op;
		boost::asio::io_context io_context;
		TcpServer * server;
		
		variables(TcpServer * ptserver):server(ptserver){}
		
		
	};
	//outer states
	struct clear {};
	struct off {};
	

	// inner states
	struct operand1 {}; // first operand entered event
	struct operand2 {}; // second operand entered event
	struct opEntered {}; // operator event entered
	struct result {}; // calculate and show result

	//inner events
	struct operand_e {
		char value = '0';
	};
	struct operator_e {
		Operation op = Operation::Add;
	};
	struct equal_e {};


    const auto clearOutput = []() {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
    };

	const auto map_operator_to_str = [](const Operation& op) -> std::string {
		std::string result;
		switch (op) {
		case Operation::Add:
			result = "+";
			break;
		case Operation::Sub:
			result = "-";
			break;
		case Operation::Div:
			result = "/";
			break;
		case Operation::Mul:
			result = "*";
			break;

		}
		return result;
	};

	const auto print = [](const std::string& s) {
		clearOutput();
		std::cout << s;
	};
	const auto clear_action = [](variables& v) {
		v.a = "0";
		v.b = "0";
		v.current_result = 0.f;
		//clearOutput();
		
	};
	auto operand1_action = [](const auto& e,variables& v) {
		v.a.push_back(e.value);
		//print((v.a).substr(1));
	};
	auto operand2_action = [](const auto &e, variables& v) {
		v.b.push_back(e.value);
		//print((v.b).substr(1));
	};
	auto operator_action = [](const auto &e, variables& v) {
		v.op = e.op;
		//print(map_operator_to_str(v.op));
	};

	auto send_result = [](variables& v) {
		
		float a = std::stoi(v.a);
		float b = std::stoi(v.b);
		float result = 0.f;
		switch (v.op) {
		case Operation::Add:
			result = a + b;
			break;
		case Operation::Sub:
			result = a - b;
			break;
		case Operation::Div:
			if (b == 0) {
				v.server->write("can't divide by zero!");
				return;
			}
			else
				result = a / b;
			break;
		case Operation::Mul:
			result = a * b;
			break;

		}
		v.current_result = result;
		v.server->write(std::to_string(result));
	};

	auto start_over_action = [](const operand_e& event,sml::back::process<operand_e> processEvent,
		variables& v) 
	{
		// clear "memory" of the state machine 
		clear_action(v);
		//start a fresh new calc with the new operand
		processEvent(event);
	};
	
	auto append_calc_action = [](const operator_e& event, 
		 variables& v)
	{
		// update first operand to latest result
		v.a = std::to_string(v.current_result);
		// reset second operand 
		v.b = "0";
		// update the operator on machine "memory"
		operator_action(event,v);
		
	};

	auto append_equal_action = [](variables& v)
	{
		// update first operand to latest result
		v.a = std::to_string(static_cast<int>(v.current_result));
		send_result(v);
	};

	struct on {
		auto operator()() const noexcept {
			using namespace sml;
			const auto init_state = state<operand1>;
			return make_transition_table(
				*init_state + event<operand_e> / operand1_action = state<operand1>,
				state<operand1> +event<operator_e>/ operator_action = state<opEntered>,
				state<opEntered> +event<operator_e> / operator_action = state<opEntered>,
				state<opEntered> +event<operand_e>/ operand2_action = state<operand2>,
				state<operand2> + event<operand_e> / operand2_action = state<operand2>,
				state<operand2> +event<equal_e> / send_result = state<result>,

				// append equal after result (result -> equal)
				state<result> + event<equal_e> / append_equal_action = state<result>,

				// append operator after result => continue caclculation
				state<result> + event<operator_e> / append_calc_action = state<opEntered>,

				// new number after result => start over
				state<result> +event<operand_e> / start_over_action = state<operand1>
			);

		};
	};


	struct states {
		auto operator()() const noexcept {
			using namespace sml;

			auto init_state = state<on>;
			return make_transition_table(
				*init_state + event<clear> / clear_action = state<on>
				, init_state + event<off> = X
				);
		};
	};
}


