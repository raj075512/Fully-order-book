#include <winsock2.h>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>  

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "boost_system.lib")  

using namespace std;
using namespace boost::asio;
using json = nlohmann::json; 


unordered_map<string, vector<SOCKET>> symbolSubscriptions;
unordered_map<SOCKET, vector<string>> clientSubscriptions;
mutex mtx;  


string apiKey = "YOUR_API_KEY";
string apiSecret = "YOUR_API_SECRET";
string apiUrl = "wss://test.deribit.com/ws/api/v2/"; 


bool initializeWinsock() {
   WSADATA wsaData;
   int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
   return (result == 0);
}


bool authenticateWithDeribit(boost::asio::io_context& io_context, const string& apiKey, const string& apiSecret) {
   
   boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("test.deribit.com"), 443);
   boost::asio::ip::tcp::socket socket(io_context);
   socket.connect(endpoint);

   
   json authMessage = {
       {"jsonrpc", "2.0"},
       {"method", "public/authorize"},
       {"params", {{"apiKey", apiKey}, {"apiSecret", apiSecret}}},
       {"id", 1}
   };


   boost::asio::write(socket, boost::asio::buffer(authMessage.dump()));


   char responseBuffer[1024];
   size_t bytesRead = socket.read_some(boost::asio::buffer(responseBuffer));
   string response(responseBuffer, bytesRead);
   cout << "Auth response: " << response << endl;

   
   if (response.find("authorized") != string::npos) {
       return true;
   }

   return false;
}


void subscribeToDeribitSymbols(boost::asio::io_context& io_context, const string& symbol) {

   boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("test.deribit.com"), 443);
   boost::asio::ip::tcp::socket socket(io_context);
   socket.connect(endpoint);

   
   json subscribeMessage = {
       {"jsonrpc", "2.0"},
       {"method", "public/subscribe"},
       {"params", {{"channels", { "order_book." + symbol }}}},
       {"id", 2}
   };

   
   boost::asio::write(socket, boost::asio::buffer(subscribeMessage.dump()));

   
   char responseBuffer[1024];
   while (true) {
       size_t bytesRead = socket.read_some(boost::asio::buffer(responseBuffer));
       string response(responseBuffer, bytesRead);

       
       json responseJson = json::parse(response);
       cout << "Market data update for symbol " << symbol << ": " << responseJson.dump() << endl;

       
       mtx.lock();
       if (symbolSubscriptions.find(symbol) != symbolSubscriptions.end()) {
           for (SOCKET clientSocket : symbolSubscriptions[symbol]) {
               send(clientSocket, response.c_str(), response.length(), 0);
           }
       }
       mtx.unlock();
   }
}


void handleClient(SOCKET clientSocket) {
   char buffer[1024];

   while (true) {
       int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
       if (bytesRead <= 0) {
           break; 
       }

       buffer[bytesRead] = '\0';
       string message(buffer);

       
       if (message.find("subscribe") != string::npos) {
           string symbol = "BTC-USD";  
           mtx.lock();
           symbolSubscriptions[symbol].push_back(clientSocket);
           clientSubscriptions[clientSocket].push_back(symbol);
           cout << "Client subscribed to symbol: " << symbol << endl;
           mtx.unlock();

           subscribeToDeribitSymbols(io_context, symbol);
       }
   }

   closesocket(clientSocket);
}


int main() {
 
   if (!initializeWinsock()) {
       cerr << "Failed to initialize Winsock" << endl;
       return 1;
   }

   boost::asio::io_context io_context;

   
   if (!authenticateWithDeribit(io_context, apiKey, apiSecret)) {
       cerr << "Failed to authenticate with Deribit API" << endl;
       return 1;
   }

  
   SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (serverSocket == INVALID_SOCKET) {
       cerr << "Socket creation failed" << endl;
       WSACleanup();
       return 1;
   }

   sockaddr_in serverAddr;
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_port = htons(9001);  
   serverAddr.sin_addr.s_addr = INADDR_ANY;

   // Bind the server socket
   if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
       cerr << "Binding failed" << endl;
       closesocket(serverSocket);
       WSACleanup();
       return 1;
   }

   // Listen for incoming connections
   if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
       cerr << "Listening failed" << endl;
       closesocket(serverSocket);
       WSACleanup();
       return 1;
   }

   cout << "Server listening on port 9001..." << endl;

   // Accept and handle client connections
   while (true) {
       SOCKET clientSocket = accept(serverSocket, NULL, NULL);
       if (clientSocket == INVALID_SOCKET) {
           cerr << "Client accept failed" << endl;
           continue;
       }

       // Handle the client connection in a separate thread
       thread clientThread(handleClient, clientSocket);
       clientThread.detach();  // Detach to handle multiple clients
   }

   closesocket(serverSocket);
   WSACleanup();
   return 0;
}


#include "Orderbook.h"
#include<iostream>

int main()
{
   std::cout << "this is main file after orderbook execution\n";
   Orderbook orderbook;
  
   std::cout << "this is main file after orderbook execution\n";
   
   return 0;
}