#pragma once
#include <string>

struct sessionInfo
{
	std::string project_name;
	std::string client;
	std::string session;
	double os_version;
	double app_build;

	sessionInfo(
		const std::string& _project_name, 
		const std::string& _client, 
		const std::string& _session, 
		const double& _os_version, 
		const double& _app_build)
		:
		project_name(_project_name),
		client(_client),
		session(_session),
		os_version(_os_version),
		app_build(_app_build)
	{
	}
};


class EventLogger
{
public:
	static void send_event_to_server(const std::string& event_name, const std::string& sender_name = "", const std::string& value = "");
};

