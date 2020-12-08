#pragma once
#include "SourceEngine/Recv.hpp"


class RecvPropHook {
	SourceEngine::RecvProp* target_property_;
	SourceEngine::RecvVarProxyFn original_proxy_fn_;
public:
	RecvPropHook(SourceEngine::RecvProp* target_property);
	~RecvPropHook();
	[[nodiscard]] SourceEngine::RecvVarProxyFn GetOriginalFunction() const;
	void SetProxyFunction(SourceEngine::RecvVarProxyFn user_proxy_fn) const;
};