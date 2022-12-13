// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Builder.hpp"

#include "../XenonCore/Logging.hpp"

#include <spirv-tools/libspirv.hpp>
#include <spirv-tools/optimizer.hpp>

#include <fmt/color.h>

namespace Xenon
{
	namespace ShaderBuilder
	{
		Builder::Builder()
		{
			m_InstructionStorage.insertOpCapability("OpCapability Shader");
			m_InstructionStorage.insertOpExtInstImport("%glsl = OpExtInstImport \"GLSL.std.450\"");
			m_InstructionStorage.setOpMemoryModel("OpMemoryModel Logical GLSL450");
		}

		Xenon::Backend::ShaderSource Builder::generate() const
		{
			auto errorMessageConsumer = [](spv_message_level_t level, const char* source, const spv_position_t& position, const char* message)
			{
				fmt::color color = fmt::color::green;
				switch (level)
				{
				case SPV_MSG_FATAL:
					color = fmt::color::red;
					break;

				case SPV_MSG_INTERNAL_ERROR:
					color = fmt::color::orange;
					break;

				case SPV_MSG_ERROR:
					color = fmt::color::orange_red;
					break;

				case SPV_MSG_WARNING:
					color = fmt::color::yellow;
					break;

				case SPV_MSG_INFO:
					color = fmt::color::green;
					break;

				case SPV_MSG_DEBUG:
					color = fmt::color::blue;
					break;

				default:
					break;
				}

				fmt::print(fg(color), "Source: {}\n", source);
				fmt::print(fg(color), "Line: {}\n", position.line);
				fmt::print(fg(color), "Index: {}\n", position.index);
				fmt::print(fg(color), "Column: {}\n", position.column);
				fmt::print(fg(color), "{}\n", message);
			};

			auto tools = spvtools::SpirvTools(SPV_ENV_UNIVERSAL_1_6);
			tools.SetMessageConsumer(errorMessageConsumer);

			std::vector<uint32_t> spirv;
			if (!tools.Assemble(m_InstructionStorage.compile(), &spirv))
				XENON_LOG_FATAL("Failed the assemble the assembly!");

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

		std::string_view Builder::getShaderTypeString(Backend::ShaderType shaderType) const noexcept
		{
			switch (shaderType)
			{
			case Xenon::Backend::ShaderType::Vertex:
				return "Vertex";

			case Xenon::Backend::ShaderType::Fragment:
				return "Fragment";

			case Xenon::Backend::ShaderType::RayGen:
				return "RayGenerationKHR";

			case Xenon::Backend::ShaderType::Intersection:
				return "IntersectionKHR";

			case Xenon::Backend::ShaderType::AnyHit:
				return "AnyHitKHR";

			case Xenon::Backend::ShaderType::ClosestHit:
				return "ClosestHitKHR";

			case Xenon::Backend::ShaderType::Miss:
				return "MissKHR";

			case Xenon::Backend::ShaderType::Callable:
				return "CallableKHR";

			case Xenon::Backend::ShaderType::Compute:
				return "GLCompute";

			default:
				return "";
			}
		}
	}
}