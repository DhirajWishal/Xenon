// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Instance.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Instance::DX12Instance(const std::string& applicationName, uint32_t applicationVersion)
			: Instance(applicationName, applicationVersion)
		{
#ifdef XENON_DEBUG
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debugger))))
			{
				m_Debugger->EnableDebugLayer();

				// Enable additional debug layers.
				m_FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
			else
			{
				XENON_LOG_ERROR("Failed to create the debug interface.");
			}

			// Enable GPU-based validation.
			ComPtr<ID3D12Debug> spDebugController0;
			ComPtr<ID3D12Debug1> spDebugController1;

			XENON_DX12_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0)), "Failed to get the debug interface!");
			XENON_DX12_ASSERT(spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1)), "Failed to query the debug interface!");
			spDebugController1->SetEnableGPUBasedValidation(true);

#endif // XENON_DEBUG
		}
	}
}