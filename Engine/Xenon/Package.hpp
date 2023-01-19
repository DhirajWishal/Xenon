// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <variant>

namespace Xenon
{
	/**
	 * Package entry type enum.
	 */
	enum class PackageEntryType : uint8_t
	{
		Int8,
		Int16,
		Int32,
		Int64,

		Uint8,
		Uint16,
		Uint32,
		Uint64,

		Float,
		Double,

		String,
		Binary,

		Char = Int8
	};

	/**
	 * Package entry type.
	 * This is just a variant with multiple candidate types.
	 */
	using PackageEntry = std::variant<
		int32_t,
		int16_t,
		int32_t,
		int64_t,

		uint32_t,
		uint16_t,
		uint32_t,
		uint64_t,

		float,
		double,

		std::string,
		std::vector<std::byte>
	>;

	/**
	 * Package class.
	 * This class contains data organized into a dictionary like structure. This means that data can be accessed using the a key string.
	 */
	class Package final
	{
	public:
		/**
		 * Default constructor.
		 */
		Package() = default;

		/**
		 * Pack all the stored data to a serialized binary.
		 *
		 * @return The serialized bytes.
		 */
		std::vector<std::byte> pack() const;

		/**
		 * Unpack the binary and initialize the package.
		 *
		 * @param bytes The bytes to initialize with.
		 */
		void unpack(std::vector<std::byte> bytes);

	private:
		std::unordered_map<std::string, PackageEntry> m_Entries;
	};
}