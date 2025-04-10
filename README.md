# GoQuantC-

A lightweight quantitative finance library for C++ applications.

## Requirements

- C++17 compiler
- CMake 3.14+
- Boost 1.75+

## Installation


git clone https://github.com/ajrou4/GoQuantC-.git
cd GoQuantC-
mkdir build && cd build
cmake ..
make
sudo make install

Basic Usage
cpp
Copy

#include <GoQuant/QuantCore.h>

int main() {
    // Calculate simple moving average
    std::vector<double> prices = {45.3, 46.2, 47.8, 48.5};
    double sma = GoQuant::calculate_sma(prices, 3);
    
    std::cout << "3-period SMA: " << sma << std::endl;
    return 0;
}

Key Features

    Financial calculations

    Statistical functions

    Basic trading strategies

    Data processing utilities

Dependencies

    nlohmann/json (included)

    WebSocket++ (included)

License

MIT License - See LICENSE file
