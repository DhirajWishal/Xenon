// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "RasterizingLayer.hpp"
#include "Renderer.hpp"

#include "../XenonCore/Logging.hpp"

#include <optick.h>

namespace Xenon
{
	RasterizingLayer::RasterizingLayer(Renderer& renderer, Backend::Camera* pCamera, Backend::AttachmentType attachmentTypes, bool enableTripleBuffering /*= false*/, Backend::MultiSamplingCount multiSampleCount /*= Backend::MultiSamplingCount::x1*/)
		: Layer(renderer)
		, m_pRasterizer(renderer.getInstance().getFactory()->createRasterizer(renderer.getInstance().getBackendDevice(), pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount))
	{
	}

	Xenon::Backend::Image* RasterizingLayer::getColorAttachment()
	{
		OPTICK_EVENT();

		const auto attachmentTypes = m_pRasterizer->getAttachmentTypes();

		if (attachmentTypes & Backend::AttachmentType::Color)
			return m_pRasterizer->getImageAttachment(Backend::AttachmentType::Color);

		if (attachmentTypes & Backend::AttachmentType::EntityID)
			return m_pRasterizer->getImageAttachment(Backend::AttachmentType::EntityID);

		if (attachmentTypes & Backend::AttachmentType::Normal)
			return m_pRasterizer->getImageAttachment(Backend::AttachmentType::Normal);

		XENON_LOG_FATAL("The rasterizing layer does not have a color attachment!");
		return nullptr;
	}
}