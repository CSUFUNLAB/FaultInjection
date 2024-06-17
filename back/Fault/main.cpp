#include "Server.h"
#include "Log.h"

int main(int argc, char* argv[]) {
	Server* mServer = new Server;
	mServer->ServerStart();
	return 0;
}