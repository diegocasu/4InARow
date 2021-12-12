#ifndef INC_4INAROW_TCPSOCKETHASHER_H
#define INC_4INAROW_TCPSOCKETHASHER_H

#include "TcpSocket.h"

namespace fourinarow {

/**
 * Class representing a custom hasher for <code>TcpSocket</code>.
 * It allows to use <code>TcpSocket</code> as key in containers
 * like <code>std::unordered_map</code>.
 */
class TcpSocketHasher {
    public:
        std::size_t operator()(const fourinarow::TcpSocket &socket) const;
};

}

#endif //INC_4INAROW_TCPSOCKETHASHER_H
