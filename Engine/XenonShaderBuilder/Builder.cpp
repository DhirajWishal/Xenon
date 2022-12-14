// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Builder.hpp"

#include "../XenonCore/Logging.hpp"

#include <spirv-tools/libspirv.hpp>
#include <spirv-tools/optimizer.hpp>

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
			auto errorMessageConsumer = [](spv_message_level_t level, const char* source, const spv_position_t& position, const char* message)
			{
				fmt::print("Source: {}\n", source);
				fmt::print("Line: {}\n", position.line);
				fmt::print("Index: {}\n", position.index);
				fmt::print("Column: {}\n", position.column);
				fmt::print("{}\n", message);
			};

			auto tools = spvtools::SpirvTools(SPV_ENV_UNIVERSAL_1_6);
			tools.SetMessageConsumer(errorMessageConsumer);

			std::vector<uint32_t> spirv;
			if (!tools.Assemble(m_InstructionStorage.compile(), &spirv))
				XENON_LOG_FATAL("Failed the assemble the assembly!");

#ifdef XENON_DEBUG
			std::string disassembled; 
			tools.Disassemble(spirv, &disassembled, SPV_BINARY_TO_TEXT_OPTION_COLOR | SPV_BINARY_TO_TEXT_OPTION_PRINT | SPV_BINARY_TO_TEXT_OPTION_INDENT | SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES);

#endif

			if (!tools.Validate(spirv))
				XENON_LOG_FATAL("The generated SPIR-V is invalid!");

#ifndef XENON_DEBUG
			// Optimize the binary if requested to.
			auto optimizer = spvtools::Optimizer(SPV_ENV_UNIVERSAL_1_6);
			optimizer.SetMessageConsumer(errorMessageConsumer);

			// Configure it.
			optimizer.RegisterPass(spvtools::CreateFreezeSpecConstantValuePass());
			optimizer.RegisterPass(spvtools::CreateUnifyConstantPass());
			optimizer.RegisterPass(spvtools::CreateStripNonSemanticInfoPass());
			optimizer.RegisterPass(spvtools::CreateEliminateDeadFunctionsPass());
			optimizer.RegisterPass(spvtools::CreateEliminateDeadMembersPass());
			optimizer.RegisterPass(spvtools::CreateStripDebugInfoPass());

			// Optimize!
			if (!optimizer.Run(spirv.data(), spirv.size(), &spirv))
				XENON_LOG_FATAL("Failed to optimize the binary!");

#endif // XENON_DEBUG

			return Backend::ShaderSource(std::move(spirv));
		}
	}
}