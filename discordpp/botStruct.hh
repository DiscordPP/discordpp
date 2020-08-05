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
	
	using handleEvent = std::function<void(const json msg)>;
	using handleRead = std::function<void(const bool error, const json msg)>;
	using handleWrite = std::function<void(const bool error)>;
	using handleSent = std::function<void()>;

	struct ratelimit{
		int millis;
	};

	class BotStruct{
	public:
		std::multimap<std::string, handleEvent> handlers;

		virtual ~BotStruct(){};

		virtual void call(
				sptr<const std::string> requestType,
				sptr<const std::string> targetURL,
				sptr<const json> body = nullptr,
				sptr<const handleWrite> onWrite = nullptr,
				sptr<const handleRead> onRead = nullptr
		) = 0;

		virtual void send(const int opcode, sptr<const json> payload, sptr<const handleSent> callback) = 0;

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
			work = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(boost::asio::make_work_guard(*aioc));
			aioc->run();
			std::cerr << "Ending run loop" << '\n';
		}

		virtual void connect() = 0;
		virtual void disconnect() = 0;
		virtual void reconnect(const bool resume = true) = 0;

		virtual void receivePayload(json payload) = 0;

		std::map<std::string, bool> needInit;
		unsigned int apiVersion = 6;
		std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work;
		std::shared_ptr<boost::asio::io_context> aioc;
		std::string token;
		bool connecting = true;
	};
}
