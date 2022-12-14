// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "AssemblyStorage.hpp"

#include "../XenonCore/Common.hpp"

#include <sstream>
#include <set>

namespace Xenon
{
	namespace ShaderBuilder
	{
		void AssemblyStorage::insertOpCapability(std::string&& instruction)
		{
			m_OpCapabilities.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertOpExtension(std::string&& instruction)
		{
			m_OpExtensions.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertOpExtInstImport(std::string&& instruction)
		{
			m_OpExtInstImports.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::setOpMemoryModel(std::string&& instruction)
		{
			m_OpMemoryModel = std::move(instruction);
		}

		void AssemblyStorage::insertOpEntryPoint(std::string&& instruction)
		{
			m_OpEntryPoints.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertExecutionMode(std::string&& instruction)
		{
			m_ExecutionModes.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertDebugSource(std::string&& instruction)
		{
			m_DebugSource.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertName(std::string&& instruction)
		{
			m_OpNames.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertOpModuleProcessed(std::string&& instruction)
		{
			m_OpModulesProcessed.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertAnnotation(std::string&& instruction)
		{
			m_Annotations.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertType(std::string&& instruction)
		{
			m_TypeDeclarations.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::beginFunctionDeclaration()
		{
			m_FunctionDeclarations.emplace_back();
		}

		void AssemblyStorage::setDeclarationOpFunction(std::string&& instruction)
		{
			m_FunctionDeclarations.back().m_OpFunction = std::move(instruction);
		}

		void AssemblyStorage::insertDeclarationOpFunctionParameter(std::string&& instruction)
		{
			m_FunctionDeclarations.back().m_OpFunctionParameters.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::beginFunctionDefinition()
		{
			auto& block = m_FunctionDefinitions.emplace_back();
			block.m_VariableInstructions.emplace_back(fmt::format("%{} = OpLabel", getUniqueID()));
		}

		void AssemblyStorage::setDefinitionOpFunction(std::string&& instruction)
		{
			m_FunctionDefinitions.back().m_Declaration.m_OpFunction = std::move(instruction);
		}

		void AssemblyStorage::insertDefinitionOpFunctionParameter(std::string&& instruction)
		{
			m_FunctionDefinitions.back().m_Declaration.m_OpFunctionParameters.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertFunctionVariable(std::string&& instruction)
		{
			m_FunctionDefinitions.back().m_VariableInstructions.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::insertFunctionInstruction(std::string&& instruction)
		{
			m_FunctionDefinitions.back().m_Instructions.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::setFunctionOpReturn(std::string&& instruction)
		{
			m_FunctionDefinitions.back().m_OpReturn = std::move(instruction);
		}

		std::string AssemblyStorage::compile() const
		{
			std::stringstream instructions;
			instructions << "; Magic:     0x07230203 (SPIR-V)" << std::endl;
			instructions << "; Version:   0x00010000 (Version: 1.0.0)" << std::endl;
			instructions << "; Generator: 0x00000000 (Xenon Shader Builder; 1)" << std::endl;
			instructions << "; Bound:     " << m_UniqueIdentifier << std::endl;
			instructions << "; Schema:    0" << std::endl;

			// Insert the capabilities.
			instructions << std::endl << "; Capabilities." << std::endl;
			for (const auto& instruction : m_OpCapabilities)
				instructions << instruction << std::endl;

			// Insert the extensions.
			instructions << std::endl << "; Extensions." << std::endl;
			for (const auto& instruction : m_OpExtensions)
				instructions << instruction << std::endl;

			// Insert the extended instructions.
			instructions << std::endl << "; Extended Instructions." << std::endl;
			for (const auto& instruction : m_OpExtInstImports)
				instructions << instruction << std::endl;

			// Set the memory model.
			instructions << std::endl << "; Memory Model." << std::endl;
			instructions << m_OpMemoryModel << std::endl;

			// Insert the entry points.
			instructions << std::endl << "; Entry Points." << std::endl;
			for (const auto& instruction : m_OpEntryPoints)
				instructions << instruction << std::endl;

			// Insert the execution modes.
			instructions << std::endl << "; Execution modes." << std::endl;
			for (const auto& instruction : m_ExecutionModes)
				instructions << instruction << std::endl;

			// Insert the debug information.
			instructions << std::endl << "; Debug information." << std::endl;
			for (const auto& instruction : m_DebugSource)
				instructions << instruction << std::endl;

			for (const auto& instruction : m_OpNames)
				instructions << instruction << std::endl;

			for (const auto& instruction : m_OpModulesProcessed)
				instructions << instruction << std::endl;

			// Insert the annotations.
			instructions << std::endl << "; Annotations." << std::endl;
			for (const auto& instruction : m_Annotations)
				instructions << instruction << std::endl;

			// Insert the type definitions.
			// This must ensure that each entry is unique.
			instructions << std::endl << "; Type definitions." << std::endl;
			std::set<uint64_t> uniqueStrings;
			for (const auto& instruction : m_TypeDeclarations)
			{
				if (uniqueStrings.emplace(GenerateHash(ToBytes(instruction.data()), instruction.size())).second)
					instructions << instruction << std::endl;
			}

			// Insert function declarations.
			instructions << std::endl << "; Function declarations." << std::endl;
			for (const auto& declaration : m_FunctionDeclarations)
			{
				instructions << declaration.m_OpFunction << std::endl;

				// Insert the parameters.
				for (const auto& instruction : declaration.m_OpFunctionParameters)
					instructions << instruction << std::endl;

				instructions << "OpFunctionEnd" << std::endl;
			}

			// Insert function definitions.
			instructions << std::endl << "; Function definitions." << std::endl;
			for (const auto& definition : m_FunctionDefinitions)
			{
				instructions << definition.m_Declaration.m_OpFunction << std::endl;

				// Insert the parameters.
				for (const auto& instruction : definition.m_Declaration.m_OpFunctionParameters)
					instructions << instruction << std::endl;

				// Insert the variable instructions.
				for (const auto& instruction : definition.m_VariableInstructions)
					instructions << instruction << std::endl;

				// Insert the function instructions.
				for (const auto& instruction : definition.m_Instructions)
					instructions << instruction << std::endl;

				instructions << definition.m_OpReturn << std::endl;
				instructions << "OpFunctionEnd" << std::endl;
			}

			return instructions.str();
		}
	}
}