#include "tcp_server.h"

#include <tbox/base/log.h>

namespace tbox {
namespace network {

TcpServer::TcpServer(event::Loop *wp_loop)
{
    LogUndo();
}

TcpServer::~TcpServer()
{
    LogUndo();
}

}
}
