// Copyright 2022 Dhiraj Wishal
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
			explicit Query([[maybe_unused]] const Device* pDevice) {}
		};
	}
}