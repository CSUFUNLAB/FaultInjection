#include "Server.h"
#include "Log.h"
#include "NodeManager.h"

std::ofstream logfilesave("log.txt");

std::ofstream &logfile(void)
{
	return logfilesave;
}

int main(int argc, char* argv[])
{
	NodeManager::get_instance()->init_m_node_info_list();
	Server* mServer = new Server;
	mServer->ServerStart();
	return 0;
}