#include "Server.h"
#include "Log.h"

#include "SshSession.h"
#include "NodeManager.h"

std::ofstream logfilesave("log.txt");

std::ofstream &logfile(void)
{
	return logfilesave;
}

int main(int argc, char* argv[]) {
	
	/*
	SshSession* ssh = new SshSession(&NodeManager::m_node_info_list[10]);
	ssh->m_send_type = SshSession::EXEC_CMD;
	ssh->only_open();


	SshSession* ssh_2 = new SshSession(&NodeManager::m_node_info_list[3]);
	ssh_2->m_send_type = SshSession::EXEC_CMD;
	ssh_2->only_open();

	ssh_2->send_cmd("nohup timeout 10 iperf3 -s -i 1 > data.txt &");
	ssh_2->send_thread();

	ssh->send_cmd("nohup iperf3 -c 192.168.12.1 -i 1 -t 4 > data.txt &");
	ssh->send_thread();
    */

	// nohup sh -c "date +%s && iperf3 -c 192.168.4.2 -i 1 -t 4" > data.txt 
	// nohup timeout 10 iperf3 -s -i 1 > data.txt &
	// nohup iperf3 -c 192.168.12.1 -i 1 -t 4 > data.txt &

	Server* mServer = new Server;
	mServer->ServerStart();

	return 0;
}