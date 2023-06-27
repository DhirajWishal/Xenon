// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "BufferMemoryView.hpp"
#include "Buffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		BufferMemoryView::BufferMemoryView(Buffer* pBuffer)
			: m_pSourceBuffer(pBuffer)
		{
			m_pBegin = m_pSourceBuffer->beginRead();
			m_pEnd = m_pBegin + m_pSourceBuffer->getSize();
		}

		BufferMemoryView::BufferMemoryView(BufferMemoryView&& other) noexcept
			: m_pSourceBuffer(std::exchange(other.m_pSourceBuffer, nullptr))
			, m_pBegin(std::exchange(other.m_pBegin, nullptr))
			, m_pEnd(std::exchange(other.m_pEnd, nullptr))
		{
		}

		BufferMemoryView::~BufferMemoryView()
		{
			if (m_pSourceBuffer) m_pSourceBuffer->endRead();
		}

		std::byte BufferMemoryView::at(uint64_t index) const
		{
			return m_pBegin[index];
		}

		std::byte BufferMemoryView::operator[](uint64_t index) const
		{
			return m_pBegin[index];
		}

		BufferMemoryView& BufferMemoryView::operator=(BufferMemoryView&& other) noexcept
		{
			m_pSourceBuffer = std::exchange(other.m_pSourceBuffer, nullptr);
			m_pBegin = std::exchange(other.m_pBegin, nullptr);
			m_pEnd = std::exchange(other.m_pEnd, nullptr);

			return *this;
		}
	}
}