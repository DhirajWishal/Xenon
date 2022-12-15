// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "../XenonCore/Logging.hpp"

#include "Builder.hpp"

#include <spirv-tools/libspirv.hpp>
#include <spirv-tools/optimizer.hpp>

namespace /* anonymous */
{
	/**
	 * SPIR-V tools error message consumer function.
	 * This function will log whatever the error we had when assembling, disassembling and when optimizing the generated SPIR-V.
	 *
	 * @param level The message level.
	 * @param source The source in which the error was thrown from.
	 * @param position The position of the error.
	 * @param message The error message.
	 */
	void SPIRVToolsErrorMessgaeConsumer([[maybe_unused]] spv_message_level_t level, const char* source, const spv_position_t& position, const char* message)
	{
		XENON_LOG_ERROR("SPIR-V Tools error @'{}'", std::launder(source));
		XENON_LOG_ERROR("Line: {}", position.line);
		XENON_LOG_ERROR("Index: {}", position.index);
		XENON_LOG_ERROR("Column: {}", position.column);
		XENON_LOG_ERROR("Message: {}", std::launder(message));
	}
}

namespace Xenon
{
	namespace ShaderBuilder
	{
		Builder::Builder()
		{
			m_InstructionStorage.insertOpExtInstImport("%glsl = OpExtInstImport \"GLSL.std.450\"");
			m_InstructionStorage.setOpMemoryModel("OpMemoryModel Logical GLSL450");
		}

		Xenon::Backend::ShaderSource Builder::generate() const
		{
			auto tools = spvtools::SpirvTools(SPV_ENV_UNIVERSAL_1_6);
			tools.SetMessageConsumer(SPIRVToolsErrorMessgaeConsumer);

			// Assemble the generated SPI-V source code.
			std::vector<uint32_t> spirv;
			if (!tools.Assemble(m_InstructionStorage.compile(), &spirv))
			{
				XENON_LOG_FATAL("Failed the assemble the assembly!");
				return {};
			}

#ifdef XENON_DEBUG
			// Disassemble the assembled SPIR-V and show it to the console.
			std::string disassembled;
			tools.Disassemble(spirv, &disassembled, SPV_BINARY_TO_TEXT_OPTION_COLOR | SPV_BINARY_TO_TEXT_OPTION_PRINT | SPV_BINARY_TO_TEXT_OPTION_INDENT | SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES);

#endif
			// Validate the binary.
			if (!tools.Validate(spirv))
			{
				XENON_LOG_FATAL("The generated SPIR-V is invalid!");
				return {};
			}

			// Optimize the binary if requested to.
			auto optimizer = spvtools::Optimizer(SPV_ENV_UNIVERSAL_1_6);
			optimizer.SetMessageConsumer(SPIRVToolsErrorMessgaeConsumer);

			// Configure it.
			optimizer.RegisterPass(spvtools::CreateFreezeSpecConstantValuePass());
			optimizer.RegisterPass(spvtools::CreateUnifyConstantPass());
			optimizer.RegisterPass(spvtools::CreateStripNonSemanticInfoPass());
			optimizer.RegisterPass(spvtools::CreateEliminateDeadFunctionsPass());
			optimizer.RegisterPass(spvtools::CreateEliminateDeadMembersPass());
			optimizer.RegisterPass(spvtools::CreateStripDebugInfoPass());

			// Optimize!
			if (!optimizer.Run(spirv.data(), spirv.size(), &spirv))
			{
				XENON_LOG_FATAL("Failed to optimize the binary!");
				return {};
			}

			return Backend::ShaderSource(std::move(spirv));
		}
	}
}