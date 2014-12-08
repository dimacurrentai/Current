#ifndef BRICKS_NET_EXCEPTIONS_H
#define BRICKS_NET_EXCEPTIONS_H

#include <exception>

namespace bricks {
namespace net {

// TODO(dkorolev): Work with Alex on shortening the names.

struct NetworkException : std::exception {};

struct SocketException : NetworkException {};
struct SocketCreateException : SocketException {};

struct ServerSocketException : SocketException {};
struct SocketBindException : ServerSocketException {};
struct SocketListenException : ServerSocketException {};
struct SocketAcceptException : ServerSocketException {};

struct ClientSocketException : SocketException {};
struct SocketConnectException : ClientSocketException {};
struct SocketResolveAddressException : ClientSocketException {};

struct SocketFcntlException : SocketException {};
struct SocketReadException : SocketException {};
struct SocketReadMultibyteRecordEndedPrematurelyException : SocketReadException {};
struct SocketWriteException : SocketException {};
struct SocketCouldNotWriteEverythingException : SocketWriteException {};

struct HTTPException : NetworkException {};
struct HTTPConnectionClosedByPeerBeforeHeadersWereSentInException : HTTPException {};
struct HTTPNoBodyProvidedException : HTTPException {};
struct HTTPAttemptedToRespondTwiceException : HTTPException {};

}  // namespace net
}  // namespace bricks

#endif  // BRICKS_NET_EXCEPTIONS_H
