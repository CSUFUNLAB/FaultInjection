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
/*
    std::string cmd_server = std::string("nohup timeout ") + std::to_string(20) +
    std::string(" \\\"iperf3 -s -i 1 -J --logfile iperf_data.json\\\"");
    std::string cmd_client = std::string("nohup timeout ") + std::to_string(10) +
    std::string(" \\\"iperf3 -c 192.168.12.167 -i 1 -t 5 -J --logfile iperf_data.json\\\"");

	std::string cmd_all_server = std::string("orangepi 192.168.12.167 orangepi \"") + cmd_server + string("\"");
	std::string cmd_all_client = std::string("orangepi 192.168.12.169 orangepi \"") + cmd_client + string("\"");
	SshSession::python_ssh(cmd_all_server);
	SshSession::python_ssh(cmd_all_client);

	NodeManager::m_node_info_list[11].ip = string("192.168.12.169");
	NodeManager::m_node_info_list[11].detected= true;
	SshSession* ssh = new SshSession(&NodeManager::m_node_info_list[11]);
	ssh->m_send_type = SshSession::EXEC_CMD;
	string cmd = "nohup timeout 20 \\\" iperf3 -s -i 1 -p 8081 -J --logfile 10_11_1.csv\\\"";
	ssh->python_ssh(cmd);
*/

	Server* mServer = new Server;
	mServer->ServerStart();

	return 0;
}