```#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include "run.hpp"
#include <curl/curl.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "./json/single_include/nlohmann/json.hpp"

using websocketpp::connection_hdl;

using json = nlohmann::json;

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;

void GoQuant::Streaming() {
    int outWitoutEntrer = 0;
    string channel;
    string symbolName;
    cout << "Subscribe to Symbol:";
    cin >> symbolName;
    channel = "deribit_price_index." + symbolName;
    string subscriptionRequest = R"({
        "jsonrpc": "2.0",
        "method": "public/subscribe",
        "params": {
            "channels": [")" + channel + R"("]
        }
    })";
    cout << subscriptionRequest << endl;
    try {
        asio::io_context ioc;
        asio::ssl::context ssl_ctx(asio::ssl::context::tlsv12_client);

        websocket::stream<asio::ssl::stream<asio::ip::tcp::socket>> ws(ioc, ssl_ctx);

        asio::ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("test.deribit.com", "443");

        asio::connect(ws.next_layer().next_layer(), results.begin(), results.end());

        ws.next_layer().handshake(asio::ssl::stream_base::client);

        ws.handshake("test.deribit.com", "/ws/api/v2");

        cout << "WebSocket connection established.\n";


        ws.write(asio::buffer(subscriptionRequest));
        cout << "Subscription request sent.\n";

        atomic<bool> stopReading(false);

        thread reader([&ws, &stopReading]() {
            try {
                beast::flat_buffer buffer;
                while (!stopReading) {
                    ws.read(buffer);
                    cout << "Received data: " << beast::make_printable(buffer.data()) << endl;
                    buffer.consume(buffer.size());
                }
            } catch (const exception& e) {
                cerr << "Error reading: " << e.what() << endl;
            }
        });

        cout << "Press Enter to stop streaming..." << endl;

        string ss;
        cin.ignore();
        getline(cin, ss);
        stopReading = true;

        reader.join();

        ws.close(websocket::close_code::normal);
        cout << "WebSocket connection closed.\n";

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}```