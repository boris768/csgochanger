#include "ISteamHTTP.h"
#include "CSGOStructs.hpp"

CHTTPRequest::CHTTPRequest(): 
m_szEndpoint(nullptr), 
m_HTTPMethod(HTTP_METHOD_INVALID),
m_Callback(nullptr), 
m_HTTPRequestHandle(INVALID_HTTP_REQUEST_HANDLE),
m_SteamAPICallHandle(INVALID_STEAM_API_CALL_HANDLE),
m_bLocked(false)
{
}

CHTTPRequest::CHTTPRequest(HTTPMethod Method, const char* Endpoint): 
m_szEndpoint(Endpoint), 
m_HTTPMethod(Method),
m_Callback(nullptr),   
m_HTTPRequestHandle(INVALID_HTTP_REQUEST_HANDLE),      
m_SteamAPICallHandle(INVALID_STEAM_API_CALL_HANDLE),
m_bLocked(false)
{
}

CHTTPRequest::~CHTTPRequest()
{
	if (this->m_HTTPRequestHandle != INVALID_HTTP_REQUEST_HANDLE)
		SourceEngine::interfaces.SteamHTTP()->ReleaseHTTPRequest(this->m_HTTPRequestHandle);
}

void CHTTPRequest::Run(HTTPRequestCompleted* pvParam)
{
	SourceEngine::interfaces.SteamHTTP()->ReleaseHTTPRequest(pvParam->m_hRequest);

	m_HTTPRequestHandle = INVALID_HTTP_REQUEST_HANDLE;
	m_bLocked = false;

	if (m_Callback != nullptr)
		m_Callback(this, pvParam);
	
}

void CHTTPRequest::Run(HTTPRequestCompleted* pvParam, bool /*bIOFailure*/, SteamAPICallHandle /*hSteamAPICall*/)
{
	this->Run(pvParam);
}

bool CHTTPRequest::Execute()
{
	if (m_bLocked)
		return false;

	if (m_HTTPRequestHandle == INVALID_HTTP_REQUEST_HANDLE)
		Register();

	this->m_bLocked = true;

	const bool ret = SourceEngine::interfaces.SteamHTTP()->SendHTTPRequest(this->m_HTTPRequestHandle, &this->m_SteamAPICallHandle);

	if (ret)
		this->SteamAPI_RegisterCallResult();

	return ret;
}

const char* CHTTPRequest::GetEndpoint() const
{
	return m_szEndpoint;
}

HTTPMethod CHTTPRequest::GetMethod() const
{
	return m_HTTPMethod;
}

CHTTPRequest* CHTTPRequest::SetEndpoint(const char* Endpoint)
{
	if (!m_bLocked)
		m_szEndpoint = Endpoint;

	return this;
}

CHTTPRequest* CHTTPRequest::SetMethod(HTTPMethod Method)
{
	if (!m_bLocked)
		m_HTTPMethod = Method;

	return this;
}

CHTTPRequest* CHTTPRequest::SetCallback(HTTPRequestCallbackFn Callback)
{
	if (!m_bLocked)
		m_Callback = Callback;

	return this;
}

CHTTPRequest* CHTTPRequest::ClearCallback()
{
	if (!m_bLocked)
		m_Callback = nullptr;

	return this;
}

bool CHTTPRequest::SetBody(const char* ContentType, const std::string& body) const
{
	if (m_bLocked || m_HTTPRequestHandle == INVALID_HTTP_REQUEST_HANDLE || m_HTTPMethod != HTTP_METHOD_POST)
		return false;

	return SourceEngine::interfaces.SteamHTTP()->SetHTTPRequestRawPostBody(this->m_HTTPRequestHandle, ContentType, (uint8_t*)body.data(), body.size());
}

bool CHTTPRequest::SetParam(const char* key, const char* value) const
{
	if (m_bLocked || m_HTTPRequestHandle == INVALID_HTTP_REQUEST_HANDLE)
		return false;

	return SourceEngine::interfaces.SteamHTTP()->SetHTTPRequestGetOrPostParameter(this->m_HTTPRequestHandle, key, value);
}

bool CHTTPRequest::SetHeader(const char* key, const char* value) const
{
	if (m_bLocked || m_HTTPRequestHandle == INVALID_HTTP_REQUEST_HANDLE)
		return false;

	return SourceEngine::interfaces.SteamHTTP()->SetHTTPRequestHeaderValue(this->m_HTTPRequestHandle, key, value);
}

using SteamAPI_RegisterCallResult_t = void(__cdecl*)(ISteamHTTPRequestCallback*, SteamAPICallHandle);
SteamAPI_RegisterCallResult_t SteamAPI_RegisterCallResultFn = nullptr;

void CHTTPRequest::SteamAPI_RegisterCallResult()
{
	if (!SteamAPI_RegisterCallResultFn)
	{
		SteamAPI_RegisterCallResultFn = reinterpret_cast<SteamAPI_RegisterCallResult_t>(GetProcAddress(GetModuleHandleW(L"steam_api.dll"),
		                                                                                               "SteamAPI_RegisterCallResult"));
	}
	SteamAPI_RegisterCallResultFn(this, this->m_SteamAPICallHandle);
}

void CHTTPRequest::Register()
{
	if (m_bLocked || !m_szEndpoint || m_HTTPMethod == HTTP_METHOD_INVALID)
		return;

	if (this->m_HTTPRequestHandle != INVALID_HTTP_REQUEST_HANDLE)
		SourceEngine::interfaces.SteamHTTP()->ReleaseHTTPRequest(m_HTTPRequestHandle);

	this->m_HTTPRequestHandle = SourceEngine::interfaces.SteamHTTP()->CreateHTTPRequest(this->m_HTTPMethod, this->m_szEndpoint);
}
