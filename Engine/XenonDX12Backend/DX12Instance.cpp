// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Instance.hpp"
#include "../XenonCore/Logging.hpp"

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

#endif // XENON_DEBUG
		}
	}
}