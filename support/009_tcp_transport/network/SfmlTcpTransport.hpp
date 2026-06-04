#ifndef TETRIS_ASSIGNMENT_009_SFML_TCP_TRANSPORT_HPP
#define TETRIS_ASSIGNMENT_009_SFML_TCP_TRANSPORT_HPP

#include "ITransport.hpp"
#include "LineFramer.hpp"

#include <SFML/Network/TcpSocket.hpp>

#include <memory>
#include <string>

namespace tetris::net {

class SfmlTcpTransport final : public ITransport {
public:
    SfmlTcpTransport();
    explicit SfmlTcpTransport(std::unique_ptr<sf::TcpSocket> socket);

    bool connect(const std::string& host, unsigned short port, int timeout_ms);
    void send(std::string line) override;
    std::optional<std::string> receive() override;
    bool disconnected() const noexcept override;

private:
    std::unique_ptr<sf::TcpSocket> socket_;
    LineFramer framer_;
    bool disconnected_ = false;
};

}  // namespace tetris::net

#endif
