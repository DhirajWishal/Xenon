// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <nlohmann/json.hpp>

#include <fstream>

namespace Xenon
{
	using JsonDocument = nlohmann::json;
	using OrderedJsonDocument = nlohmann::ordered_json;
	using JsonObject = JsonDocument::object_t;

	/**
	 * Global configuration class.
	 * This is a CRTP object which is intended to have one derived class per requirement (ie: per backend).
	 * Internally it contains a JSON document and can load/ save content from it.
	 * This class is made to contain one instance per engine instance (as a global object).
	 *
	 * @tparam Derived The derived class type.
	 * @tparam JsonDocumentType The json document type. It could be either JsonDocument (the default) or OrderedJsonDocument.
	 */
	template<class Derived, class JsonDocumentType = JsonDocument>
	class GlobalConfiguration
	{
	protected:
		/**
		 * Default constructor.
		 */
		GlobalConfiguration() = default;

		/**
		 * Virtual destructor.
		 */
		virtual ~GlobalConfiguration() = default;

	public:
		/**
		 * Load data from a config file.
		 *
		 * @param configFile The state file to load the config from.
		 */
		void load(const std::filesystem::path& configFile)
		{
			auto inputFile = std::ifstream(configFile);
			if (inputFile.is_open())
				m_Document = JsonDocumentType::from_cbor(inputFile);
		}

		/**
		 * Save the JSON data to a file.
		 * This will optimize the output but converting JSON to CBOR before saving.
		 *
		 * @param configFile The state file to save the config to.
		 */
		void save(const std::filesystem::path& configFile) const
		{
			auto outputFile = std::ofstream(configFile);
			if (outputFile.is_open())
			{
				const auto cborData = JsonDocument::to_cbor(m_Document);
				outputFile.write(std::bit_cast<const char*>(cborData.data()), cborData.size());
			}
		}

		/**
		 * Get the static instance of the object.
		 *
		 * @return The object reference.
		 */
		[[nodiscard]] static Derived& GetInstance()
		{
			static Derived config;
			return config;
		}

		/**
		 * Get the JSON document.
		 *
		 * @return The document.
		 */
		[[nodiscard]] JsonDocumentType& getDocument() noexcept { return m_Document; }

		/**
		 * Get the JSON document.
		 *
		 * @return The document.
		 */
		[[nodiscard]] const JsonDocumentType& getDocument() const noexcept { return m_Document; }

	protected:
		JsonDocumentType m_Document;
	};
}