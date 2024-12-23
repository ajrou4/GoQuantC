#include <iostream>
#include <string>
#include <curl/curl.h>
#include "include/json.hpp"

using json = nlohmann::json;

class Order {
private:
    std::string clientId;
    std::string clientSecret;
    std::string accessToken;

    // Callback function for handling cURL responses
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    // Function to send a cURL request
    std::string sendRequest(const std::string& url, const json& payload, const std::string& token = "") {
        std::string readBuffer;
        CURL* curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);

            // Set the request payload
            std::string jsonStr = payload.dump();
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

            // Set headers
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            if (!token.empty()) {
                headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // Set up the write callback
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            // Perform the request
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
            }

            // Free resources
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();
        return readBuffer;
    }

public:
    Order(const std::string& id, const std::string& secret) : clientId(id), clientSecret(secret) {}

    // Retrieve access token
    void authenticate() {
        json payload = {
            {"id", 0},
            {"method", "public/auth"},
            {"params", {
                {"grant_type", "client_credentials"},
                {"scope", "session:apiconsole-c5i26ds6dsr expires:2592000"},
                {"client_id", clientId},
                {"client_secret", clientSecret}
            }},
            {"jsonrpc", "2.0"}
        };

        std::string response = sendRequest("https://test.deribit.com/api/v2/public/auth", payload);
        auto responseJson = json::parse(response);

        if (responseJson.contains("result") && responseJson["result"].contains("access_token")) {
            accessToken = responseJson["result"]["access_token"];
        } else {
            std::cerr << "Failed to retrieve access token." << std::endl;
        }
    }

    // Place an order
    void placeOrder(const std::string& price, const std::string& amount, const std::string& instrument) {
        json payload = {
            {"jsonrpc", "2.0"},
            {"method", "private/buy"},
            {"params", {
                {"instrument_name", instrument},
                {"type", "limit"},
                {"price", price},
                {"amount", amount}
            }},
            {"id", 1}
        };

        std::string response = sendRequest("https://test.deribit.com/api/v2/private/buy", payload, accessToken);
        std::cout << "Place Order Response: " << response << std::endl;
    }

    // Cancel an order
    void cancelOrder(const std::string& orderID) {
        json payload = {
            {"jsonrpc", "2.0"},
            {"method", "private/cancel"},
            {"params", {"order_id", orderID}},
            {"id", 6}
        };

        std::string response = sendRequest("https://test.deribit.com/api/v2/private/cancel", payload, accessToken);
        std::cout << "Cancel Order Response: " << response << std::endl;
    }

    // Retrieve open orders
    void getOpenOrders() {
        json payload = {
            {"jsonrpc", "2.0"},
            {"method", "private/get_open_orders"},
            {"params", {"kind", "future", "type", "limit"}},
            {"id", 25}
        };

        std::string response = sendRequest("https://test.deribit.com/api/v2/private/get_open_orders", payload, accessToken);
        auto responseJson = json::parse(response);

        if (responseJson.contains("result")) {
            std::cout << "Open Orders:\n\n";
            for (const auto& order : responseJson["result"]) {
                std::cout << "Instrument: " << order["instrument_name"]
                          << ", Order ID: " << order["order_id"]
                          << ", Price: " << order["price"]
                          << ", Amount: " << order["amount"] << '\n';
            }
        } else {
            std::cerr << "Error: Could not retrieve open orders." << std::endl;
        }
    }
};

int main() {
    Order order("BVUmeOyt", "d6g5hD9vStqevqX7q75qM3WsuEPKSllwLl7PIpYaMlk");

    order.authenticate();

    if (!order.accessToken.empty()) {
        // Uncomment to perform actions
        // order.placeOrder("50000", "10", "ETH-PERPETUAL");
        // order.cancelOrder("29257473891");
        order.getOpenOrders();
    } else {
        std::cerr << "Authentication failed. Exiting." << std::endl;
    }

    return 0;
}
