// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>
#include <stack>

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Assembly storage class.
		 * This class is used to store all the SPIR-V assembly instructions in an efficient manner and then compiled to a single assembly string when needed.
		 *
		 * Note that this is just a container and does not do any internal validation. Some entries will be filtered out to be unique.
		 * This can also be used to generate unique IDs for variables.
		 */
		class AssemblyStorage final
		{
			/**
			 * Function declaration structure.
			 */
			struct FunctionDeclaration final
			{
				std::string m_OpFunction;
				std::string m_OpFunctionEnd;

				std::vector<std::string> m_OpFunctionParameters;
			};

			/**
			 * Block structure.
			 */
			struct Block final
			{
				std::string m_OpLabel;
				std::string m_Termination;

				std::vector<std::string> m_Instructions;
			};

			/**
			 * Function definition structure.
			 */
			struct FunctionDefinition final
			{
				Block m_VariableBlock;

				std::string m_OpFunction;
				std::string m_OpFunctionEnd;

				std::stack<Block> m_BlockStack;

				std::vector<Block> m_Blocks;
				std::vector<std::string> m_OpFunctionParameters;
			};

		public:
			/**
			 * Default constructor.
			 */
			AssemblyStorage() = default;

			/**
			 * Insert an OpCapability instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertOpCapability(std::string&& instruction);

			/**
			 * Insert an OpExtension instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertOpExtension(std::string&& instruction);

			/**
			 * Insert an OpExtInstImport instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertOpExtInstImport(std::string&& instruction);

			/**
			 * Set the OpMemoryModel instruction.
			 *
			 * @param instruction The instruction to set.
			 */
			void setOpMemoryModel(std::string&& instruction);

			/**
			 * Insert an OpEntryPoint instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertOpEntryPoint(std::string&& instruction);

			/**
			 * Insert an execution mode instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertExecutionMode(std::string&& instruction);

			/**
			 * Insert a debug source instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertDebugSource(std::string&& instruction);

			/**
			 * Insert a name instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertName(std::string&& instruction);

			/**
			 * Insert an OpModuleProcessed instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertOpModuleProcessed(std::string&& instruction);

			/**
			 * Insert an annotation instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertAnnotation(std::string&& instruction);

			/**
			 * Begin a function declaration.
			 * This will create a new function declaration and instructions can be recorded to it afterwards.
			 * Note that this function MUST BE CALLED in order for those function to work.
			 */
			void beginFunctionDeclaration();

			/**
			 * set an OpFunction declaration instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void setDeclarationOpFunction(std::string&& instruction);

			/**
			 * Insert an OpFunctionParameter declaration instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertDeclarationOpFunctionParameter(std::string&& instruction);

			/**
			 * set an OpFunctionEnd declaration instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void setDeclarationOpFunctionEnd(std::string&& instruction);

			/**
			 * Begin a function definition.
			 * This is the same as before (the function declaration) as the class works as before; the proceeding function calls which require the function definition
			 * will not work without this function call.
			 */
			void beginFunctionDefinition();

			/**
			 * set an OpFunction definition instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void setDefinitionOpFunction(std::string&& instruction);

			/**
			 * Insert an OpFunctionParameter definition instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertDefinitionOpFunctionParameter(std::string&& instruction);

			/**
			 * set an OpFunctionEnd definition instruction.
			 *
			 * @param instruction The instruction to insert.
			 */
			void setDefinitionOpFunctionEnd(std::string&& instruction);

			/**
			 * Begin a new function block.
			 * This will push a new function block to the stack and it will be used in the proceeding functions which require the function block.
			 */
			void beginFunctionBlock();

			/**
			 * Insert an instruction to the current function block.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertFunctionInstruction(std::string&& instruction);

			/**
			 * End the current function block.
			 */
			void endFunctionBlock();

			/**
			 * Compile all the source instructions to one source code string and return it.
			 *
			 * @return The compiled assembly.
			 */
			[[nodiscard]] std::string compile() const;

		private:
			std::string m_OpMemoryModel;

			std::vector<std::string> m_OpCapabilities;
			std::vector<std::string> m_OpExtensions;
			std::vector<std::string> m_OpExtInstImports;

			std::vector<std::string> m_OpEntryPoints;
			std::vector<std::string> m_ExecutionModes;

			std::vector<std::string> m_DebugSource;
			std::vector<std::string> m_OpNames;
			std::vector<std::string> m_OpModulesProcessed;

			std::vector<std::string> m_Annotations;

			std::vector<std::string> m_TypeDeclarations;	// This needs to be unique!

			std::vector<FunctionDeclaration> m_FunctionDeclarations;

			std::vector<FunctionDefinition> m_FunctionDefinitions;
		};
	}
}