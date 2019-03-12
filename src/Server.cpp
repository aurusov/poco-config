#include "Server.h"

#include "handlers/Factory.h"

#include <iostream>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocketImpl.h>

namespace
{

class ServerSocketImpl: public Poco::Net::ServerSocketImpl
{
public:
	using Poco::Net::SocketImpl::init;
};

class Socket: public Poco::Net::Socket
{
public:
	explicit Socket(const std::string & address)
		: Poco::Net::Socket(new ServerSocketImpl())
	{
		const Poco::Net::SocketAddress socket_address(address);
		ServerSocketImpl * socket = static_cast<ServerSocketImpl*>(impl());
		socket->init(socket_address.af());
		socket->setReuseAddress(true);
		socket->setReusePort(false);
		socket->bind(socket_address, false);
		socket->listen();
	}
};

} // anonymous namespace

void Server::defineOptions(Poco::Util::OptionSet & options)
{
	Poco::Util::ServerApplication::defineOptions(options);
	options.addOption(
        Poco::Util::Option("config", "c", "load configuration data from a file")
		    .required(true)
			.argument("file")
			.callback(Poco::Util::OptionCallback<Server>(this, &Server::handleConfig)));
}

void Server::handleConfig(const std::string& /*name*/, const std::string& file_name)
{
	loadConfiguration(file_name);
}

int Server::main(const std::vector<std::string>& args)
{
	Poco::Net::HTTPServerParams::Ptr parameters = new Poco::Net::HTTPServerParams();
	parameters->setTimeout(10000);
	parameters->setMaxQueued(100);
	parameters->setMaxThreads(4);

	const std::string address = config().getString("server.host") + ":" + config().getString("server.port");
	std::cout << "address = " << address << std::endl;
	const Socket socket(address);
	const Poco::Net::ServerSocket server_socket(socket);
	Poco::Net::HTTPServer server(new handlers::Factory(), server_socket, parameters);

	server.start();
	waitForTerminationRequest();
	server.stopAll();

	return 0;
}
