// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Query class.
		 * This class is used for different querying operations, like occlusion queries.
		 */
		class Query : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit Query(XENON_MAYBE_UNUSED const Device* pDevice) {}
		};
	}
}