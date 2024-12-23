#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <set>
#include <mutex>

typedef websocketpp::server<websocketpp::config::asio> server;

class MarketDataWebSocketServer {
private:
    server wsServer;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;
    std::mutex connectionMutex;

public:
    MarketDataWebSocketServer() {
        wsServer.init_asio();

        // Set message handler
        wsServer.set_message_handler([this](websocketpp::connection_hdl hdl, server::message_ptr msg) {
            onMessage(hdl, msg);
        });

        // Set open handler
        wsServer.set_open_handler([this](websocketpp::connection_hdl hdl) {
            std::lock_guard<std::mutex> lock(connectionMutex);
            connections.insert(hdl);
        });

        // Set close handler
        wsServer.set_close_handler([this](websocketpp::connection_hdl hdl) {
            std::lock_guard<std::mutex> lock(connectionMutex);
            connections.erase(hdl);
        });
    }

    void onMessage(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        std::string payload = msg->get_payload();
        std::cout << "Received message: " << payload << std::endl;

        // Handle subscription message
        if (payload.find("subscribe") != std::string::npos) {
            // Example subscription handling
            std::cout << "Client subscribed to market data." << std::endl;
        }
    }

    void broadcastMessage(const std::string& message) {
        std::lock_guard<std::mutex> lock(connectionMutex);
        for (const auto& conn : connections) {
            wsServer.send(conn, message, websocketpp::frame::opcode::text);
        }
    }

    void run(uint16_t port) {
        wsServer.listen(port);
        wsServer.start_accept();
        wsServer.run();
    }
};

int main() {
    MarketDataWebSocketServer wsServer;

    // Run WebSocket server on port 9002
    std::thread serverThread([&]() {
        wsServer.run(9002);
    });

    // Simulate real-time data streaming
    OrderAPI trading("yOIyFnVV", "drz0-qSHhFrcl77Sfbnw6zNM2gSSiY5l1ZkSZJGc0Ng");
    std::string accessToken = trading.getAccessToken();
    if (!accessToken.empty()) {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::string orderBookData = trading.getOrderBook("BTC-PERPETUAL");
            wsServer.broadcastMessage(orderBookData);
        }
    }

    serverThread.join();
    return 0;
}
