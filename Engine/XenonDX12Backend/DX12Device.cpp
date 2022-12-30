// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Device.hpp"
#include "DX12Macros.hpp"

#include <optick.h>
#include <spirv_hlsl.hpp>

namespace /* anonymous */
{
	/**
	 * Get the execution model from the shader type.
	 *
	 * @param type The shader type.
	 * @return The execution model.
	 */
	[[nodiscard]] constexpr spv::ExecutionModel GetExecutionModel(Xenon::Backend::ShaderType type) noexcept
	{
		switch (type)
		{
		case Xenon::Backend::ShaderType::Vertex:
			return spv::ExecutionModel::ExecutionModelVertex;

		case Xenon::Backend::ShaderType::Fragment:
			return spv::ExecutionModel::ExecutionModelFragment;

		case Xenon::Backend::ShaderType::RayGen:
			return spv::ExecutionModel::ExecutionModelRayGenerationKHR;

		case Xenon::Backend::ShaderType::Intersection:
			return spv::ExecutionModel::ExecutionModelIntersectionKHR;

		case Xenon::Backend::ShaderType::AnyHit:
			return spv::ExecutionModel::ExecutionModelAnyHitKHR;

		case Xenon::Backend::ShaderType::ClosestHit:
			return spv::ExecutionModel::ExecutionModelClosestHitKHR;

		case Xenon::Backend::ShaderType::Miss:
			return spv::ExecutionModel::ExecutionModelMissKHR;

		case Xenon::Backend::ShaderType::Callable:
			return spv::ExecutionModel::ExecutionModelCallableKHR;

		case Xenon::Backend::ShaderType::Compute:
			return spv::ExecutionModel::ExecutionModelGLCompute;

		default:
			return spv::ExecutionModel::ExecutionModelVertex;
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12Device::DX12Device(DX12Instance* pInstance, RenderTargetType requiredRenderTargets)
			: Device(pInstance, requiredRenderTargets)
			, m_pInstance(pInstance)
		{
			// Create the factory.
			createFactory();

			// Create the device.
			createDevice();

			// Create the command structures.
			createCommandStructures();

			// Create the allocator.
			createAllocator();
		}

		DX12Device::~DX12Device()
		{
			m_pAllocator->Release();
		}

		void DX12Device::waitIdle()
		{
			OPTICK_EVENT();

			// Wait for the direct queue.
			{
				ComPtr<ID3D12Fence> fence;
				XENON_DX12_ASSERT(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the fence!");
				XENON_DX12_ASSERT(m_DirectQueue->Signal(fence.Get(), 1), "Failed to signal the fence!");

				// Setup synchronization.
				auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

				// Validate the event.
				if (fenceEvent == nullptr)
				{
					XENON_LOG_ERROR("Failed to wait till the command list execution!");
					return;
				}

				// Set the event and wait.
				XENON_DX12_ASSERT(fence->SetEventOnCompletion(1, fenceEvent), "Failed to set the fence event on completion event!");
				WaitForSingleObjectEx(fenceEvent, std::numeric_limits<DWORD>::max(), FALSE);
				CloseHandle(fenceEvent);
			}

			// Wait for the copy queue.
			{
				ComPtr<ID3D12Fence> fence;
				XENON_DX12_ASSERT(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the fence!");
				XENON_DX12_ASSERT(m_CopyQueue->Signal(fence.Get(), 1), "Failed to signal the fence!");

				// Setup synchronization.
				auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

				// Validate the event.
				if (fenceEvent == nullptr)
				{
					XENON_LOG_ERROR("Failed to wait till the command list execution!");
					return;
				}

				// Set the event and wait.
				XENON_DX12_ASSERT(fence->SetEventOnCompletion(1, fenceEvent), "Failed to set the fence event on completion event!");
				WaitForSingleObjectEx(fenceEvent, std::numeric_limits<DWORD>::max(), FALSE);
				CloseHandle(fenceEvent);
			}
		}

		DXGI_FORMAT DX12Device::convertFormat(DataFormat format) const
		{
			switch (format)
			{
			case Xenon::Backend::DataFormat::Undefined:									return DXGI_FORMAT_UNKNOWN;
			case Xenon::Backend::DataFormat::R8_SRGB:									return DXGI_FORMAT_R8_SINT;
			case Xenon::Backend::DataFormat::R8G8_SRGB:									return DXGI_FORMAT_R8G8_SINT;
			case Xenon::Backend::DataFormat::R8G8B8A8_SRGB:								return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case Xenon::Backend::DataFormat::R8_UNORMAL:								return DXGI_FORMAT_R8_UNORM;
			case Xenon::Backend::DataFormat::R8G8_UNORMAL:								return DXGI_FORMAT_R8G8_UNORM;
			case Xenon::Backend::DataFormat::R8G8B8A8_UNORMAL:							return DXGI_FORMAT_R8G8B8A8_UNORM;
			case Xenon::Backend::DataFormat::B8G8R8A8_UNORMAL:							return DXGI_FORMAT_B8G8R8A8_UNORM;
			case Xenon::Backend::DataFormat::R16_SFLOAT:								return DXGI_FORMAT_R16_FLOAT;
			case Xenon::Backend::DataFormat::R16G16_SFLOAT:								return DXGI_FORMAT_R16G16_FLOAT;
			case Xenon::Backend::DataFormat::R16G16B16A16_SFLOAT:						return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case Xenon::Backend::DataFormat::R32_SFLOAT:								return DXGI_FORMAT_R32_FLOAT;
			case Xenon::Backend::DataFormat::R32G32_SFLOAT:								return DXGI_FORMAT_R32G32_FLOAT;
			case Xenon::Backend::DataFormat::R32G32B32_SFLOAT:							return DXGI_FORMAT_R32G32B32_FLOAT;
			case Xenon::Backend::DataFormat::R32G32B32A32_SFLOAT:						return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case Xenon::Backend::DataFormat::D16_SINT:									return DXGI_FORMAT_D16_UNORM;
			case Xenon::Backend::DataFormat::D32_SFLOAT:								return DXGI_FORMAT_D32_FLOAT;
			case Xenon::Backend::DataFormat::D24_UNORMAL_S8_UINT:						return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case Xenon::Backend::DataFormat::D32_SFLOAT_S8_UINT:						return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			default:																	XENON_LOG_ERROR("Invalid or unsupported data format! Defaulting to Undefined.");
			}

			return DXGI_FORMAT_UNKNOWN;
		}

		std::pair<bool, bool> DX12Device::getFormatSupport(DXGI_FORMAT format, D3D12_FORMAT_SUPPORT1 support1 /*= D3D12_FORMAT_SUPPORT1_NONE*/, D3D12_FORMAT_SUPPORT2 support2 /*= D3D12_FORMAT_SUPPORT2_NONE*/) const
		{
			D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = { format, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE };
			XENON_DX12_ASSERT(m_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport)), "Failed to check for format support!");

			return std::make_pair(
				(formatSupport.Support1 & support1) == support1,
				(formatSupport.Support2 & support2) == support2
			);
		}

		ComPtr<ID3DBlob> DX12Device::CompileShader(const ShaderSource& shader, ShaderType type, const std::string_view& newEntryPoint /*= ""*/)
		{
			OPTICK_EVENT();

			// Remove the end padding and create the compiler.
			auto compiler = spirv_cross::CompilerHLSL(shader.getBinaryWithoutPadding());

			// Set the options.
			spirv_cross::CompilerHLSL::Options options;
			options.shader_model = 50;
			compiler.set_hlsl_options(options);

			// Rename the entry point if we have to.
			std::string_view entryPoint = shader.getEntryPoint();
			if (!newEntryPoint.empty())
			{
				compiler.rename_entry_point(entryPoint.data(), newEntryPoint.data(), GetExecutionModel(type));
				entryPoint = newEntryPoint;
			}

			// If we're in the vertex shader set the correct semantics.
			if (type & ShaderType::Vertex)
			{
				compiler.add_vertex_attribute_remap({ .location = Xenon::EnumToInt(Xenon::Backend::InputElement::VertexPosition), .semantic = "POSITION0" });
				compiler.add_vertex_attribute_remap({ .location = Xenon::EnumToInt(Xenon::Backend::InputElement::VertexNormal), .semantic = "NORMAL0" });
				compiler.add_vertex_attribute_remap({ .location = Xenon::EnumToInt(Xenon::Backend::InputElement::VertexTangent), .semantic = "TANGENT0" });

				for (uint32_t i = Xenon::EnumToInt(Xenon::Backend::InputElement::VertexColor_0); i <= Xenon::EnumToInt(Xenon::Backend::InputElement::VertexColor_7); i++)
					compiler.add_vertex_attribute_remap({ .location = i, .semantic = fmt::format("COLOR{}", i - Xenon::EnumToInt(Xenon::Backend::InputElement::VertexColor_0)) });

				for (uint32_t i = Xenon::EnumToInt(Xenon::Backend::InputElement::VertexTextureCoordinate_0); i <= Xenon::EnumToInt(Xenon::Backend::InputElement::VertexTextureCoordinate_7); i++)
					compiler.add_vertex_attribute_remap({ .location = i, .semantic = fmt::format("TEXCOORD{}", i - Xenon::EnumToInt(Xenon::Backend::InputElement::VertexTextureCoordinate_0)) });

				compiler.add_vertex_attribute_remap({ .location = Xenon::EnumToInt(Xenon::Backend::InputElement::InstancePosition), .semantic = "POSITION1" });
				compiler.add_vertex_attribute_remap({ .location = Xenon::EnumToInt(Xenon::Backend::InputElement::InstanceRotation), .semantic = "POSITION2" });
				compiler.add_vertex_attribute_remap({ .location = Xenon::EnumToInt(Xenon::Backend::InputElement::InstanceScale), .semantic = "POSITION3" });
				compiler.add_vertex_attribute_remap({ .location = Xenon::EnumToInt(Xenon::Backend::InputElement::InstanceID), .semantic = "PSIZE1" });
			}

			// Cross-compile the binary.
			const auto hlsl = compiler.compile();

			// Resolve the target.
			std::string_view target;
			switch (type)
			{
			case Xenon::Backend::ShaderType::Vertex:
				target = "vs_5_0";
				break;
			case Xenon::Backend::ShaderType::Fragment:
				target = "ps_5_0";
				break;
			case Xenon::Backend::ShaderType::Compute:
				target = "cs_5_0";
				break;

			default:
				break;
			}

			// Compile the shader.
			ComPtr<ID3DBlob> shaderBlob;
			constexpr std::array<D3D_SHADER_MACRO, 2> macros = { D3D_SHADER_MACRO("SPIRV_CROSS_CONSTANT_ID_0", "1u"), D3D_SHADER_MACRO() };
			XENON_DX12_ASSERT(D3DCompile(hlsl.data(), hlsl.size(), nullptr, macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.data(), target.data(), 0, 0, &shaderBlob, nullptr), "Failed to compile the shader!");

			return shaderBlob;
		}

		void DX12Device::createFactory()
		{
			XENON_DX12_ASSERT(CreateDXGIFactory2(m_pInstance->getFactoryFlags(), IID_PPV_ARGS(&m_Factory)), "Failed to create the DXGI factory!");
		}

		void DX12Device::createDevice()
		{
			// Setup the test feature levels if a device was not found.
			constexpr std::array<D3D_FEATURE_LEVEL, 3> testFeatureLevels = {
				D3D_FEATURE_LEVEL_12_2,
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0
			};

			// Iterate over the features and check if the best feature is available.
			for (uint8_t i = 0; i < testFeatureLevels.size() && !m_Device; i++)
			{
				const auto featureLevel = testFeatureLevels[i];

				// Create the hardware adapter.
				ComPtr<IDXGIAdapter1> hardwareAdapter;
				getHardwareAdapter(m_Factory.Get(), &hardwareAdapter, featureLevel);

				// Try to create the device.
				const auto result = D3D12CreateDevice(
					hardwareAdapter.Get(),
					featureLevel,
					IID_PPV_ARGS(&m_Device)
				);

				m_Adapter = hardwareAdapter;

				// If it failed, try making a software rasterizing device.
				if (FAILED(result))
				{
					ComPtr<IDXGIAdapter> warpAdapter;
					XENON_DX12_ASSERT(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)), "Failed to wrap the adapter!");

					D3D12CreateDevice(
						warpAdapter.Get(),
						featureLevel,
						IID_PPV_ARGS(&m_Device)
					);

					m_Adapter = warpAdapter;
				}
			}

			// Check if we were able to create a device.
			if (!m_Device)
			{
				XENON_LOG_FATAL("Failed to create a DirectX device!");
				return;
			}

			XENON_DX12_NAME_OBJECT(m_Device, "Device");
		}

		void DX12Device::createCommandStructures()
		{
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			XENON_DX12_ASSERT(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_DirectQueue)), "Failed to create the direct queue!");

			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			XENON_DX12_ASSERT(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_BundleQueue)), "Failed to create the bundle queue!");

			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			XENON_DX12_ASSERT(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CopyQueue)), "Failed to create the copy queue!");
		}

		void DX12Device::createAllocator()
		{
			D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
			allocatorDesc.pDevice = m_Device.Get();
			allocatorDesc.pAdapter = m_Adapter.Get();

			XENON_DX12_ASSERT(D3D12MA::CreateAllocator(&allocatorDesc, &m_pAllocator), "Failed to create the memory allocator!");
		}

		_Use_decl_annotations_ void DX12Device::getHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, D3D_FEATURE_LEVEL featureLevel)
		{
			*ppAdapter = nullptr;

			ComPtr<IDXGIAdapter1> adapter;

			// Create the factory to get the adapter.
			ComPtr<IDXGIFactory6> factory6;
			if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
			{
				for (UINT i = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter))); i++)
				{
					DXGI_ADAPTER_DESC1 desc;
					adapter->GetDesc1(&desc);

					// Skip the software renderer.
					if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
						continue;

					// Check to see whether the adapter supports the feature level.
					if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), featureLevel, _uuidof(ID3D12Device5), nullptr)))
						break;
				}
			}

			// If we don't have an adapter, enumerate all the adapters and check.
			if (adapter.Get() == nullptr)
			{
				for (UINT i = 0; SUCCEEDED(pFactory->EnumAdapters1(i, &adapter)); i++)
				{
					DXGI_ADAPTER_DESC1 desc;
					adapter->GetDesc1(&desc);

					// Skip the software renderer.
					if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
						continue;

					// Check to see whether the adapter supports the feature level.
					if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), featureLevel, _uuidof(ID3D12Device5), nullptr)))
						break;
				}
			}

			*ppAdapter = adapter.Detach();
		}
	}
}