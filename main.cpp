#include "run.hpp"
#include "Streaming.cpp"

int main() {


    OrderAPI trading("yOIyFnVV", "drz0-qSHhFrcl77Sfbnw6zNM2gSSiY5l1ZkSZJGc0Ng");
    std::string accessToken = trading.getAccessToken();

    if (!accessToken.empty()) {
        int choice;
        do {
            std::cout << "\nChoose an option:\n";
            std::cout << "1. Get Order Book\n";
            std::cout << "2. Get Position\n";
            std::cout << "3. Place Order\n";
            std::cout << "4. Cancel Order\n";
            std::cout << "5. Modify Order\n";
            std::cout << "6. Get Open Orders\n";
            std::cout << "7.Stream price Index\n";
            std::cout << "0. Exit\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    std::string instrument;
                    std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
                    std::cin >> instrument;
                    trading.getOrderBook(instrument);
                    break;
                }
                case 2: {
                    std::string instrument;
                    std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
                    std::cin >> instrument;
                    trading.getPosition(instrument);
                    break;
                }
                case 3: {
                    std::string price, amount, instrument;
                    std::cout << "Enter price: ";
                    std::cin >> price;
                    std::cout << "Enter amount: ";
                    std::cin >> amount;
                    std::cout << "Enter instrument name (e.g., ETH-PERPETUAL): ";
                    std::cin >> instrument;
                    trading.placeOrder(price, amount, instrument);
                    break;
                }
                case 4: {
                    std::string orderID;
                    std::cout << "Enter Order ID to cancel: ";
                    std::cin >> orderID;
                    trading.cancelOrder(orderID);
                    break;
                }
                case 5: {
                    std::string orderID;
                    int amount;
                    double price;
                    std::cout << "Enter Order ID to modify: ";
                    std::cin >> orderID;
                    std::cout << "Enter new amount: ";
                    std::cin >> amount;
                    std::cout << "Enter new price: ";
                    std::cin >> price;
                    trading.modifyOrder(orderID, amount, price);
                    break;
                }
                case 6: {
                    trading.getOpenOrders();
                    break;
                }
                case 7:{
                    trading.Streaming();
                    break;
                }
                case 0:
                    std::cout << "Exiting...\n";
                    break;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 0);
    } else {
        std::cerr << "Failed to authenticate. Please check your credentials.\n";
    }

    return 0;
}