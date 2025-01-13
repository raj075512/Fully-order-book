
# Orderbook Library for Deribit Test Exchange

This repository contains a high-performance C++ library for managing order execution and orderbook data from the Deribit Test Exchange. It is designed to provide low-latency operations and real-time updates with support for WebSocket communication and JSON parsing.

## Features

### Core Functionalities
- **Low-Latency Order Management:**
  - Place new orders for spot, futures, and options.
  - Modify existing orders.
  - Cancel active orders.

- **Orderbook Management:**
  - Fetch and process the live orderbook.
  - View positions across all symbols.
  - Maintain a snapshot of current market conditions.

- **WebSocket Communication:**
  - Subscribe to symbols for continuous orderbook updates.
  - Efficiently handle real-time data streams.

- **JSON Support:**
  - Parse and handle JSON payloads using [JSON for Modern C++](https://github.com/nlohmann/json).
  - Serialize and deserialize orders and positions for ease of integration.

### Additional Capabilities
- Thread-safe operations for multithreaded environments.
- Support for advanced market strategies.
- Modular and extensible design.

## Prerequisites

Ensure that you have the following installed:
- C++17 or later
- [Boost Libraries](https://www.boost.org/)
- [JSON for Modern C++](https://github.com/nlohmann/json)

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/orderbook-library.git
   cd orderbook-library
   ```

2. Build the library:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

3. Run the tests (optional):
   ```bash
   make test
   ```

## Usage

### Example: Initializing the Orderbook

```cpp
#include "Orderbook.h"
#include "WebSocketClient.h"

int main() {
    Orderbook orderbook;
    WebSocketClient wsClient("wss://test.deribit.com/ws/api/v2");

    wsClient.connect();
    wsClient.subscribe("BTC-PERPETUAL");

    while (true) {
        std::string message = wsClient.receive();
        orderbook.update(message);
        orderbook.print();
    }

    return 0;
}
```

### Example: Placing an Order

```cpp
#include "OrderManager.h"

int main() {
    OrderManager orderManager("api_key", "api_secret");
    
    orderManager.placeOrder("BTC-PERPETUAL", 1, 45000, "buy");
    return 0;
}
```

## API Documentation

### Orderbook
- **update(const std::string &message):** Updates the orderbook with new data from WebSocket.
- **print():** Prints the current state of the orderbook.

### WebSocketClient
- **connect():** Establishes a WebSocket connection.
- **subscribe(const std::string &symbol):** Subscribes to updates for a specific symbol.
- **receive():** Receives the next message from the WebSocket.

### OrderManager
- **placeOrder(const std::string &symbol, int quantity, double price, const std::string &side):** Places a new order.
- **modifyOrder(int orderId, double newPrice):** Modifies an existing order.
- **cancelOrder(int orderId):** Cancels an existing order.

## Contributing

1. Fork the repository.
2. Create your feature branch (`git checkout -b feature/AmazingFeature`).
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`).
4. Push to the branch (`git push origin feature/AmazingFeature`).
5. Open a pull request.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Contact

For questions or suggestions, feel free to contact:
- **Author:** Ashutosh raj
- **Email:** raj075512@gmail.com
  
