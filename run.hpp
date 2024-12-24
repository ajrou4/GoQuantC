
#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json-develop/single_include/nlohmann/json.hpp"
#pragma once

using json = nlohmann::json;

class OrderAPI {
private:
    std::string clientId;
    std::string clientSecret;
    std::string accessToken;
    const std::string baseUrl = "https://test.deribit.com/api/v2";

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    std::string sendRequest(const std::string& url, const json& payload, const std::string& token = "") {
        std::string readBuffer;
        CURL* curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            
            std::string jsonStr = payload.dump();
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            if (!token.empty()) {
                headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();
        return readBuffer;
    }

public:
    OrderAPI(const std::string& id, const std::string& secret) : clientId(id), clientSecret(secret) {}
    void Streaming();
    std::string getAccessToken() {
        json payload = {
            {"id", 0},
            {"method", "public/auth"},
            {"params", {
                {"grant_type", "client_credentials"},
                {"scope", "session:apiconsole"},
                {"client_id", clientId},
                {"client_secret", clientSecret}
            }},
            {"jsonrpc", "2.0"}
        };

        std::string response = sendRequest(baseUrl + "/public/auth", payload);
        
        try {
            auto responseJson = json::parse(response);
            if (responseJson.contains("result") && responseJson["result"].contains("access_token")) {
                accessToken = responseJson["result"]["access_token"];
                return accessToken;
            }
        }
        catch (const json::parse_error& e) {
            std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
        }
        return "";
    }

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

        std::string response = sendRequest(baseUrl + "/private/buy", payload, accessToken);
        std::cout << "Place Order Response: " << response << std::endl;
    }

    void cancelOrder(const std::string& orderID) {
        json payload = {
            {"jsonrpc", "2.0"},
            {"method", "private/cancel"},
            {"params", {{"order_id", orderID}}},
            {"id", 6}
        };

        std::string response = sendRequest(baseUrl + "/private/cancel", payload, accessToken);
        std::cout << "Cancel Order Response: " << response << std::endl;
    }

    void modifyOrder(const std::string& orderID, int amount, double price) {
        json payload = {
            {"jsonrpc", "2.0"},
            {"method", "private/edit"},
            {"params", {
                {"order_id", orderID},
                {"amount", amount},
                {"price", price}
            }},
            {"id", 11}
        };

        std::string response = sendRequest(baseUrl + "/private/edit", payload, accessToken);
        std::cout << "Modify Order Response: " << response << std::endl;
    }

    void getOrderBook(const std::string& instrument) {
        json payload = {
            {"jsonrpc", "2.0"},
            {"method", "public/get_order_book"},
            {"params", {{"instrument_name", instrument}}},
            {"id", 15}
        };

        std::string response = sendRequest(baseUrl + "/public/get_order_book", payload, accessToken);
        auto responseJson = json::parse(response);

        std::cout << "Order Book for " << instrument << ":\n\n";
        
        std::cout << "Best Bid Price: " << responseJson["result"]["best_bid_price"] 
                  << ", Amount: " << responseJson["result"]["best_bid_amount"] << '\n';
        std::cout << "Best Ask Price: " << responseJson["result"]["best_ask_price"] 
                  << ", Amount: " << responseJson["result"]["best_ask_amount"] << '\n';

        std::cout << "\nAsks:\n";
        for (const auto& ask : responseJson["result"]["asks"]) {
            std::cout << "Price: " << ask[0] << ", Amount: " << ask[1] << '\n';
        }

        std::cout << "\nBids:\n";
        for (const auto& bid : responseJson["result"]["bids"]) {
            std::cout << "Price: " << bid[0] << ", Amount: " << bid[1] << '\n';
        }

        std::cout << "\nMark Price: " << responseJson["result"]["mark_price"] << '\n';
        std::cout << "Open Interest: " << responseJson["result"]["open_interest"] << '\n';
        std::cout << "Timestamp: " << responseJson["result"]["timestamp"] << '\n';
    }

    void getPosition(const std::string& instrument) {
        json payload = {
            {"jsonrpc", "2.0"},
            {"method", "private/get_position"},
            {"params", {{"instrument_name", instrument}}},
            {"id", 20}
        };

        std::string response = sendRequest(baseUrl + "/private/get_position", payload, accessToken);
        auto responseJson = json::parse(response);
        
        if (responseJson.contains("result")) {
            std::cout << "Position Details for " << instrument << ":\n\n";
            auto result = responseJson["result"];
            std::cout << "Estimated Liquidation Price: " << result["estimated_liquidation_price"] << '\n';
            std::cout << "Size Currency: " << result["size_currency"] << '\n';
            std::cout << "Realized Funding: " << result["realized_funding"] << '\n';
            std::cout << "Total Profit Loss: " << result["total_profit_loss"] << '\n';
            std::cout << "Realized Profit Loss: " << result["realized_profit_loss"] << '\n';
            std::cout << "Floating Profit Loss: " << result["floating_profit_loss"] << '\n';
            std::cout << "Leverage: " << result["leverage"] << '\n';
            std::cout << "Average Price: " << result["average_price"] << '\n';
            std::cout << "Delta: " << result["delta"] << '\n';
            std::cout << "Mark Price: " << result["mark_price"] << '\n';
            std::cout << "Direction: " << result["direction"] << '\n';
            std::cout << "Initial Margin: " << result["initial_margin"] << '\n';
            std::cout << "Maintenance Margin: " << result["maintenance_margin"] << '\n';
            std::cout << "Size: " << result["size"] << '\n';
        }
    }

    void getOpenOrders() {
        json payload = {
            {"jsonrpc", "2.0"},
            {"method", "private/get_open_orders"},
            {"params", {{"kind", "future"}, {"type", "limit"}}},
            {"id", 25}
        };

        std::string response = sendRequest(baseUrl + "/private/get_open_orders", payload, accessToken);
        auto responseJson = json::parse(response);

        if (responseJson.contains("result")) {
            std::cout << "Open Orders:\n\n";
            for (const auto& order : responseJson["result"]) {
                std::cout << "Instrument: " << order["instrument_name"]
                          << ", Order ID: " << order["order_id"]
                          << ", Price: " << order["price"]
                          << ", Amount: " << order["amount"] << '\n';
            }
        }
    }
};
