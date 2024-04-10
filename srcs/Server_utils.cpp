#include <Server.hpp>

int Server::sfd_non_blocking(int sfd)
{
    int flags;

	// Gets the flags of the fd
    flags = fcntl(sfd, F_GETFL, 0);
    if(flags == -1)
        return -1;

	// sets the flags of the fd
    flags |= O_NONBLOCK;
    if(fcntl(sfd, F_SETFL, flags))
        return -1;
    return 0;
}
