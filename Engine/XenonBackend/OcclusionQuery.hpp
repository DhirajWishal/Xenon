// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Query.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Occlusion query class.
		 * This class can be used for occlusion querying.
		 */
		class OcclusionQuery : public Query
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param sampleCount The maximum number of possible samples that could be written to.
			 */
			explicit OcclusionQuery(const Device* pDevice, uint64_t sampleCount) : Query(pDevice), m_SampleCount(sampleCount) {}

			/**
			 * Get the samples.
			 * This will query the samples from the backend.
			 *
			 * @return The samples.
			 */
			[[nodiscard]] virtual std::vector<uint64_t> getSamples() = 0;

			/**
			 * Get the sample count.
			 *
			 * @return The sample count.
			 */
			[[nodiscard]] uint64_t getSampleCount() const noexcept { return m_SampleCount; }

		protected:
			uint64_t m_SampleCount = 0;
		};
	}
}