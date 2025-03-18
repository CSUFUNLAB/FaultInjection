#include "Server.h"
#include "Log.h"

std::ofstream logfilesave("log.txt");

std::ofstream &logfile(void)
{
	return logfilesave;
}

int main(int argc, char* argv[])
{
	Server* mServer = new Server;
	mServer->ServerStart();
	return 0;
}