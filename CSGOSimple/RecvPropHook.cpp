#include "RecvPropHook.h"

RecvPropHook::~RecvPropHook()
{
	target_property_->m_ProxyFn = this->original_proxy_fn_;
}

SourceEngine::RecvVarProxyFn RecvPropHook::GetOriginalFunction() const
{
	return this->original_proxy_fn_;
}

void RecvPropHook::SetProxyFunction(SourceEngine::RecvVarProxyFn user_proxy_fn) const
{
	target_property_->m_ProxyFn = user_proxy_fn;
}

RecvPropHook::RecvPropHook(SourceEngine::RecvProp* target_property): target_property_(target_property),
                                                                     original_proxy_fn_(target_property->m_ProxyFn)
{
}
