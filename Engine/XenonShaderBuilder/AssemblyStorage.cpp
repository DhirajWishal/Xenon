// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "AssemblyStorage.hpp"

#include <sstream>

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

		void AssemblyStorage::setDeclarationOpFunctionEnd(std::string&& instruction)
		{
			m_FunctionDeclarations.back().m_OpFunctionEnd = std::move(instruction);
		}

		void AssemblyStorage::beginFunctionDefinition()
		{
			m_FunctionDefinitions.emplace_back();
		}

		void AssemblyStorage::setDefinitionOpFunction(std::string&& instruction)
		{
			m_FunctionDefinitions.back().m_OpFunction = std::move(instruction);
		}

		void AssemblyStorage::insertDefinitionOpFunctionParameter(std::string&& instruction)
		{
			m_FunctionDefinitions.back().m_OpFunctionParameters.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::setDefinitionOpFunctionEnd(std::string&& instruction)
		{
			m_FunctionDefinitions.back().m_OpFunctionEnd = std::move(instruction);
		}

		void AssemblyStorage::beginFunctionBlock()
		{
			m_FunctionDefinitions.back().m_BlockStack.emplace();
		}

		void AssemblyStorage::insertFunctionInstruction(std::string&& instruction)
		{
			m_FunctionDefinitions.back().m_BlockStack.top().m_Instructions.emplace_back(std::move(instruction));
		}

		void AssemblyStorage::endFunctionBlock()
		{
			auto& currentDefinition = m_FunctionDefinitions.back();
			currentDefinition.m_Blocks.emplace_back(std::move(currentDefinition.m_BlockStack.top()));
			currentDefinition.m_BlockStack.pop();
		}

		std::string AssemblyStorage::compile() const
		{
			std::stringstream instructions;

			return instructions.str();
		}
	}
}