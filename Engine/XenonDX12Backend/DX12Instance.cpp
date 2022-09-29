// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Instance.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Instance::DX12Instance(const std::string& applicationName, uint32_t applicationVersion)
			: Instance(applicationName, applicationVersion)
		{
			UINT dxgiFactoryFlags = 0;

#ifdef XENON_DEBUG
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debugger))))
			{
				m_Debugger->EnableDebugLayer();

				// Enable additional debug layers.
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
#endif // XENON_DEBUG
		}
	}
}