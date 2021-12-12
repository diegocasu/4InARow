#include "TcpSocketHasher.h"

namespace fourinarow {

std::size_t TcpSocketHasher::operator()(const TcpSocket &socket) const {
    return std::hash<int>()(socket.getDescriptor());
}

}
