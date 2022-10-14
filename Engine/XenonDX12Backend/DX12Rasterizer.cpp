// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Rasterizer.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Rasterizer::DX12Rasterizer(DX12Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering /*= false*/, MultiSamplingCount multiSampleCount /*= MultiSamplingCount::x1*/)
			: Rasterizer(pDevice, pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount)
		{
		}
	}
}