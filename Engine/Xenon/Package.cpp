// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Package.hpp"

#include "../XenonCore/Common.hpp"

/**
 * The Xenon package format is quite simple and is made to be that way to be easier to load and unload a package from it's binary format.
 *
 * Header:
 * The package format starts off with 2 32 bit unsigned integers containing the magic number (720138338) and a package version number.
 * Then it's followed by 12 entires of type information, which contains 2 64 bit unsigned integers containing the entry count and entry offset (in the data block) respectively.
 *
 * Package Entry Block:
 * Then contains a 64 bit unsigned integer specifying the offset in the data block in which the value is stored.
 * Followed by another 64 bit integer specifying the data size.
 * Then contains a 64 bit unsigned integer specifying the number of package entries that are stored.
 * A package entry contains
 */

namespace /* anonymous */
{
	/**
	 * Entry type information structure.
	 * This contains information about a single entry type.
	 */
	struct EntryTypeInformation final
	{
		uint64_t m_EntryCount = 0;
		uint64_t m_EntryOffset = 0;
	};

	/**
	 * Header structure.
	 * This contains the header information of a package binary.
	 */
	struct Header final
	{
		uint32_t m_MagicNumber = 0b00101010111011000111000001100010;
		uint32_t m_PackageVersion = 0;

		EntryTypeInformation m_Int8 = {};
		EntryTypeInformation m_Int16 = {};
		EntryTypeInformation m_Int32 = {};
		EntryTypeInformation m_Int64 = {};

		EntryTypeInformation m_Uint8 = {};
		EntryTypeInformation m_Uint16 = {};
		EntryTypeInformation m_Uint32 = {};
		EntryTypeInformation m_Uint64 = {};

		EntryTypeInformation m_Float = {};
		EntryTypeInformation m_Double = {};

		EntryTypeInformation m_String = {};
		EntryTypeInformation m_Binary = {};
	};

	struct PackageEntry final
	{
		uint64_t m_DataOffset = 0;
		uint32_t m_IdentifierLength = 0;
		std::string m_Identifier;
	};

	/**
	 * Data block structure.
	 * This stores information about the data block.
	 */
	struct DataBlock final
	{
		std::vector<int8_t> m_Int8;
		std::vector<int16_t> m_Int16;
		std::vector<int32_t> m_Int32;
		std::vector<int64_t> m_Int64;
		std::vector<uint8_t> m_Uint8;
		std::vector<uint16_t> m_Uint16;
		std::vector<uint32_t> m_Uint32;
		std::vector<uint64_t> m_Uint64;
		std::vector<float> m_Float;
		std::vector<double> m_Double;
		std::vector<std::string> m_String;
		std::vector<std::vector<std::byte>> m_Binary;
	};
}

namespace Xenon
{
	std::vector<std::byte> Package::pack() const
	{
		std::vector<std::byte> packedBinary;

		uint64_t binarySize = sizeof(Header);
		for (const auto& [key, value] : m_Entries)
		{
			binarySize += key.size();

			switch (static_cast<PackageEntryType>(value.index()))
			{
			case Xenon::PackageEntryType::Int8:
				break;

			case Xenon::PackageEntryType::Int16:
				break;

			case Xenon::PackageEntryType::Int32:
				break;

			case Xenon::PackageEntryType::Int64:
				break;

			case Xenon::PackageEntryType::Uint8:
				break;

			case Xenon::PackageEntryType::Uint16:
				break;

			case Xenon::PackageEntryType::Uint32:
				break;

			case Xenon::PackageEntryType::Uint64:
				break;

			case Xenon::PackageEntryType::Float:
				break;

			case Xenon::PackageEntryType::Double:
				break;

			case Xenon::PackageEntryType::String:
				break;

			case Xenon::PackageEntryType::Binary:
				break;

			default:
				break;
			}
		}

		packedBinary.resize(binarySize);

		return packedBinary;
	}

	void Package::unpack(std::vector<std::byte> bytes)
	{

	}
}