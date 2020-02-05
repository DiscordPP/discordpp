//
// Created by Aidan on 2/2/2020.
//

#pragma once

namespace discordpp{
	template<class BASE>
	class PluginOverload: public BASE, virtual BotStruct{
	public:
		virtual void ccall(
				const std::string &requestType,
				const std::string &targetURL
		){
			BASE::call(
					std::make_shared<std::string>(requestType),
					std::make_shared<std::string>(targetURL),
					nullptr,
					nullptr
			);
		};

		virtual void ccall(
				const std::string &requestType,
				const std::string &targetURL,
				const json &body
		){
			BASE::call(
					std::make_shared<std::string>(requestType),
					std::make_shared<std::string>(targetURL),
					std::make_shared<json>(body),
					nullptr
			);
		};

		virtual void ccall(
				const std::string &requestType,
				const std::string &targetURL,
				const std::function<void(const json)> &callback
		){
			BASE::call(
					std::make_shared<std::string>(requestType),
					std::make_shared<std::string>(targetURL),
					nullptr,
					std::make_shared<std::function<void(const json)>>(callback)
			);
		};

		virtual void ccall(
				const std::string &requestType,
				const std::string &targetURL,
				const json &body,
				const std::function<void(const json)> &callback
		){
			BASE::call(
					std::make_shared<std::string>(requestType),
					std::make_shared<std::string>(targetURL),
					std::make_shared<json>(body),
					std::make_shared<std::function<void(const json)>>(callback)
			);
		};

		virtual void call(
				sptr<const std::string> requestType,
				sptr<const std::string> targetURL
		){
			BASE::call(std::move(requestType), std::move(targetURL), nullptr, nullptr);
		};

		virtual void call(
				sptr<const std::string> requestType,
				sptr<const std::string> targetURL,
				sptr<const json> body
		){
			BASE::call(std::move(requestType), std::move(targetURL), std::move(body), nullptr);
		};

		virtual void call(
				sptr<const std::string> requestType,
				sptr<const std::string> targetURL,
				sptr<const std::function<void(const json)>> callback
		){
			BASE::call(std::move(requestType), std::move(targetURL), nullptr, std::move(callback));
		};

		virtual void csend(const int opcode){
			BASE::send(opcode, nullptr, nullptr);
		};

		virtual void csend(const int opcode, const json &payload){
			BASE::send(opcode, std::make_shared<json>(payload), nullptr);
		};

		virtual void csend(const int opcode, const std::function<void()> &callback){
			BASE::send(opcode, nullptr, std::make_shared<const std::function<void()>>(callback));
		};

		virtual void csend(const int opcode, const json &payload, const std::function<void()> &callback){
			BASE::send(opcode, std::make_shared<json>(payload), std::make_shared<const std::function<void()>>(callback));
		};

		virtual void send(const int opcode){
			BASE::send(opcode, nullptr, nullptr);
		};

		virtual void send(const int opcode, sptr<const json> payload){
			BASE::send(opcode, std::move(payload), nullptr);
		};

		virtual void send(const int opcode, sptr<const std::function<void()>> callback){
			BASE::send(opcode, nullptr, std::move(callback));
		};
	};
}