#include <Server.hpp>

int Server::sfd_non_blocking(int sfd)
{
	if(fcntl(sfd, F_SETFL, O_NONBLOCK | FD_CLOEXEC))
		return -1;
	return 0;
}
