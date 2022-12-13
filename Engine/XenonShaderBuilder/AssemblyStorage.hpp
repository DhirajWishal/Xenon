// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TypeTraits.hpp"

#include <string>
#include <vector>
#include <stack>

#include <fmt/format.h>

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Get the constant value's identifier.
		 * Make sure that the type is registered.
		 *
		 * @tparam Type The type of the value.
		 * @param value The constant value.
		 * @return The identifier string.
		 */
		template<class Type>
		[[nodiscard]] static std::string GetConstantIdentifier(const Type& value)
		{
			return fmt::format("const_{}_{}", TypeTraits<Type>::RawIdentifier, value);
		}

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
			 * Function definition structure.
			 */
			struct FunctionDefinition final
			{
				FunctionDeclaration m_Declaration;

				std::vector<std::string> m_VariableInstructions;
				std::vector<std::string> m_Instructions;
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
			 * Insert a new type instruction.
			 * This will just insert it to the internal vector and when compiling it will only use the unique instructions.
			 *
			 * @param instruction.
			 */
			void insertType(std::string&& instruction);

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
			 * Insert a function variable.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertFunctionVariable(std::string&& instruction);

			/**
			 * Insert an instruction to the current function block.
			 *
			 * @param instruction The instruction to insert.
			 */
			void insertFunctionInstruction(std::string&& instruction);

			/**
			 * Register type function.
			 *
			 * @tparam Type The type to register.
			 */
			template<class Type>
			constexpr void registerType()
			{
				// Try and register value types if the Type is complex.
				if constexpr (TypeTraits<Type>::ComponentCount > 1)
					registerType<typename TypeTraits<Type>::ComponentType>();

				insertType(fmt::format(FMT_STRING("{} = {}"), TypeTraits<Type>::Identifier, TypeTraits<Type>::Declaration));
			}

			/**
			 * Register multiple types function.
			 *
			 * @tparam Type The type to register.
			 * @tparam Types The rest of the types.
			 */
			template<class Type, class... Types>
			constexpr void registerTypes()
			{
				// Try and register the type.
				registerType<Type>();

				// Register the rest if available.
				if constexpr (sizeof...(Types) > 0)
					registerTypes<Types...>();
			}

			/**
			 * Store a constant to the storage.
			 * The identifier will be const_<type identifier>_<value as a integer>.
			 *
			 * @tparam Type The type of the value.
			 * @param value The constant value.
			 */
			template<class Type>
			constexpr void storeConstant(const Type& value)
			{
				registerType<Type>();
				insertType(fmt::format("%{} = OpConstant {} {}", GetConstantIdentifier(value), TypeTraits<Type>::Identifier, value));
			}

			/**
			 * Register an array function.
			 *
			 * @tparam ValueType The value type of the array.
			 * @tparam Size The size of the array.
			 */
			template<class ValueType, size_t Size>
			constexpr void registerArray()
			{
				// Try and register value types if the Type is complex.
				if constexpr (TypeTraits<ValueType>::ComponentCount > 1)
					registerType<typename TypeTraits<ValueType>::ComponentType>();

				storeConstant<uint32_t>(Size);
				insertType(fmt::format("%array_{}_{} = OpTypeArray {} %{}", TypeTraits<ValueType>::RawIdentifier, Size, TypeTraits<ValueType>::Identifier, GetConstantIdentifier<uint32_t>(Size)));
			}

			/**
			 * Get type identifier.
			 *
			 * @tparam Type The type to get the identifier of.
			 * @return The identifier.
			 */
			template<class Type>
			[[nodiscard]] constexpr std::string getTypeIdentifier()
			{
				registerType<Type>();
				return fmt::format(FMT_STRING("{} "), TypeTraits<Type>::Identifier);
			}

			/**
			 * Get multiple type identifiers.
			 *
			 * @tparam Type The type to get the identifier of.
			 * @tparam Types The rest of the types.
			 * @return The identifier.
			 */
			template<class Type, class... Types>
			[[nodiscard]] constexpr std::string getTypeIdentifiers()
			{
				registerType<Type>();
				if constexpr (sizeof...(Types) > 0)
					return fmt::format("{} {}", TypeTraits<Type>::Identifier, getTypeIdentifiers<Types...>());

				else
					return fmt::format(FMT_STRING("{} "), TypeTraits<Type>::Identifier);
			}

			/**
			 * Compile all the source instructions to one source code string and return it.
			 * This function can be very expensive so it must be done at the very end.
			 *
			 * @return The compiled assembly.
			 */
			[[nodiscard]] std::string compile() const;

			/**
			 * Get a new unique ID.
			 *
			 * @return The unique ID integer.
			 */
			[[nodiscard]] uint32_t getUniqueID() { return m_UniqueIdentifier++; }

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

			uint32_t m_UniqueIdentifier = 1;
		};
	}
}