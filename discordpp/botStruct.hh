//
// Created by Aidan on 6/21/2018.
//

#pragma once

#include <iostream>

#include <boost/asio.hpp>

#include <nlohmann/json.hpp>
#include <utility>

namespace discordpp{
	using json = nlohmann::json;

	template<typename T>
	using sptr = std::shared_ptr<T>;

	struct ratelimit{
		int millis;
	};

	class BotStruct{
	public:
		std::multimap<std::string, std::function<void(json)>> handlers;

		virtual ~BotStruct(){};

		virtual void call(
				sptr<const std::string> requestType,
				sptr<const std::string> targetURL,
				sptr<const json> body = nullptr,
				sptr<const std::function<void()>> onWrite = nullptr,
				sptr<const std::function<void(const json)>> onRead = nullptr
		) = 0;

		virtual void send(const int opcode, sptr<const json> payload, sptr<const std::function<void()>> callback) = 0;

		void run(){
			for(auto module: needInit){
				if(module.second){
					std::cerr << "Forgot to initialize: " << module.first << '\n';
					exit(1);
				}
			}
			runctd();
		}

	protected:
		virtual void runctd(){
			std::cerr << "Starting run loop" << '\n';
			aioc->run();
			std::cerr << "Ending run loop" << '\n';
		}

		virtual void connect() = 0;
		virtual void disconnect() = 0;
		virtual void reconnect(const bool resume = true) = 0;

		virtual void receivePayload(json payload) = 0;

		std::map<std::string, bool> needInit;
		unsigned int apiVersion = 6;
		std::shared_ptr<boost::asio::io_context> aioc;
		std::string token;
		bool connecting = true;
	};
}
