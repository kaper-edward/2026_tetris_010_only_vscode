#include "SfmlTcpTransport.hpp"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Time.hpp>

namespace tetris::net {

SfmlTcpTransport::SfmlTcpTransport() : socket_(std::make_unique<sf::TcpSocket>()) {
    socket_->setBlocking(false);
}

bool SfmlTcpTransport::connect(const std::string& host, unsigned short port, int timeout_ms) {
    socket_->setBlocking(true);
    const auto address = sf::IpAddress::resolve(host);
    if (!address) {
        socket_->setBlocking(false);
        disconnected_ = true;
        return false;
    }
    const sf::Socket::Status status = socket_->connect(*address, port, sf::milliseconds(timeout_ms));
    socket_->setBlocking(false);
    disconnected_ = status != sf::Socket::Status::Done;
    return !disconnected_;
}

void SfmlTcpTransport::send(std::string line) {
    const std::string framed = frame_line(std::move(line));
    std::size_t sent = 0;
    while (sent < framed.size()) {
        std::size_t just_sent = 0;
        const sf::Socket::Status status = socket_->send(framed.data() + sent, framed.size() - sent, just_sent);
        if (status == sf::Socket::Status::Done || status == sf::Socket::Status::Partial) {
            sent += just_sent;
        } else if (status != sf::Socket::Status::NotReady) {
            disconnected_ = true;
            return;
        }
    }
}

std::optional<std::string> SfmlTcpTransport::receive() {
    if (auto line = framer_.popLine()) {
        return line;
    }
    char buffer[2048];
    std::size_t received = 0;
    const sf::Socket::Status status = socket_->receive(buffer, sizeof(buffer), received);
    if (status == sf::Socket::Status::Done && received > 0) {
        framer_.pushBytes(std::string_view(buffer, received));
        if (framer_.failed()) {
            disconnected_ = true;
            return std::nullopt;
        }
        return framer_.popLine();
    }
    if (status == sf::Socket::Status::Disconnected || status == sf::Socket::Status::Error) {
        disconnected_ = true;
    }
    return std::nullopt;
}

bool SfmlTcpTransport::disconnected() const noexcept {
    return disconnected_;
}

}  // namespace tetris::net
