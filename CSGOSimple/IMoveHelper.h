#pragma once

namespace SourceEngine
{
	class IMoveHelper
	{
		virtual void UnknownVirtual() = 0;
	public:
		virtual void SetHost(void* host) = 0;
	};
}