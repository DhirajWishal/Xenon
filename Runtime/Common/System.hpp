// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Common.hpp"
#include "Logging.hpp"

#include <functional>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>

namespace Xenon
{
	/**
	 * Interface request handler class.
	 * This is the base class for the request handlers which are used by systems.
	 */
	class IRequestHandler {};

	/**
	 * Request handler class.
	 * This class contains the function callback which is registered to systems and are called when handling a request.
	 *
	 * The function type: void(Request&)
	 *
	 * @tparam Request The request type.
	 */
	template<class Request>
	class RequestHandler final : public IRequestHandler
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @tparam Function The function type.
		 * @param function The function which is to be called.
		 */
		template<class Function>
		explicit RequestHandler(Function&& function) : m_Callback(std::move(function)) {}

		/**
		 * Function call operator.
		 * This will call the internally stored function by passing in the request reference.
		 *
		 * @param request The request which is passed to the callback function.
		 */
		void operator()(Request& request) { m_Callback(request); }

	private:
		std::function<void(Request&)> m_Callback;
	};

	/**
	 * System class.
	 * Systems are special classes which provides the support to host a system on another thread, and enables other threads to request
	 * different things from.
	 *
	 * It's recommended to use the constructor to register all the request handlers to avoid unnecessary problems with issuing requests.
	 */
	class System
	{
		template<class Type>
		friend class SystemHandler;

	protected:
		/**
		 * Default constructor.
		 */
		System() = default;

		/**
		 * Virtual destructor.
		 */
		virtual ~System() = default;

		/**
		 * On start pure virtual method.
		 * This method is called on the worker thread as soon as the thread initiates.
		 */
		virtual void onStart() = 0;

		/**
		 * On update pure virtual method.
		 * This method is called by the system on every worker thread iteration after handling the requests.
		 * Note that this method is called before handling the requests.
		 */
		virtual void onUpdate() = 0;

		/**
		 * On termination pure virtual method.
		 * This method is called by the system's worker thread once the system is terminated.
		 */
		virtual void onTermination() = 0;

	public:
		/**
		 * Issue a request to the system.
		 *
		 * @tparam Request The request type.
		 * @tparam Arguments The request constructor argument types.
		 * @param arguments The arguments to be passed to the constructor.
		 * @return The created request pointer.
		 */
		template<class Request, class... Arguments>
		Request* issueRequest(Arguments&&... arguments)
		{
			const auto index = GetTypeIndex<Request>();
			const auto lock = std::scoped_lock(m_RequestMutex);

			// Take the request only if a request handler is registered.
			if (m_RequestHandlerMap.contains(index))
			{
				auto pRequest = new Request(std::forward<Arguments>(arguments)...);
				m_Requests.emplace_back([this, pRequest, pHandler = std::static_pointer_cast<RequestHandler<Request>>(m_RequestHandlerMap[index])]() mutable
					{
						(*pHandler)(*pRequest);
						delete pRequest;
					}
				);

				return pRequest;
			}

			XENON_LOG_ERROR("The request cannot be issued. Make sure that the system supports this request.");
			return nullptr;
		}

	protected:
		/**
		 * Register a request handler which will be automatically called when a request is been made.
		 *
		 * @tparam Request The request type.
		 * @tparam Function The function type.
		 * @param function The function to register.
		 */
		template<class Request, class Function>
		void registerRequestHandler(Function&& function)
		{
			const auto index = GetTypeIndex<Request>();
			const auto lock = std::scoped_lock(m_RequestMutex);

			if (m_RequestHandlerMap.contains(index))
				XENON_LOG_WARNING("A request handler for the request type already exists. Overriding the old handler.");

			m_RequestHandlerMap[index] = std::make_shared<RequestHandler<Request>>(std::move(function));
		}

		/**
		 * This method will handle all the requests which are issued to this system.
		 */
		void handleRequests();

	private:
		std::mutex m_RequestMutex;
		std::vector<std::function<void()>> m_Requests;
		std::unordered_map<std::type_index, std::shared_ptr<IRequestHandler>> m_RequestHandlerMap;
	};
}