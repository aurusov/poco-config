#pragma once

#include <Poco/Util/ServerApplication.h>

class Server: public Poco::Util::ServerApplication
{
private:
	int main(const std::vector<std::string>& args) override;
	void defineOptions(Poco::Util::OptionSet & options) override;
	void handleConfig(const std::string& /*name*/, const std::string& file_name);
};
