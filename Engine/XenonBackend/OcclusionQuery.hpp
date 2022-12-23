// Copyright 2022 Dhiraj Wishal
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
			 * Get the results form the query.
			 *
			 * @return The samples
			 */
			[[nodiscard]] virtual const std::vector<uint64_t>& getResults() = 0;

			/**
			 * Get the samples.
			 * This will not query the samples from the backend.
			 *
			 * @return The samples.
			 */
			[[nodiscard]] const std::vector<uint64_t>& getSamples() const noexcept { return m_Samples; }

		protected:
			std::vector<uint64_t> m_Samples;
		};
	}
}