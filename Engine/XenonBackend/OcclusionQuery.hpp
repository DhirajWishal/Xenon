// Copyright 2022-2023 Dhiraj Wishal
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
			explicit OcclusionQuery(const Device* pDevice, uint64_t sampleCount) : Query(pDevice), m_Samples(sampleCount) {}

			/**
			 * Get the samples.
			 * This will not query the samples from the backend.
			 *
			 * @return The samples.
			 */
			[[nodiscard]] const std::vector<uint64_t>& getSamples() const noexcept { return m_Samples; }

			/**
			 * Get the samples data pointer.
			 *
			 * @return The pointer.
			 */
			[[nodiscard]] uint64_t* getSamplesPointer() noexcept { return m_Samples.data(); }

			/**
			 * Get the samples data pointer.
			 *
			 * @return The pointer.
			 */
			[[nodiscard]] const uint64_t* getSamplesPointer() const noexcept { return m_Samples.data(); }

			/**
			 * Get the sample count.
			 *
			 * @return The sample count.
			 */
			[[nodiscard]] uint64_t getSampleCount() const noexcept { return m_Samples.size(); }

		protected:
			std::vector<uint64_t> m_Samples;
		};
	}
}