// install websocketpp library if it doesnt exist already
// brew install websocketpp

#include <iostream>
#include <vector>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::client<websocketpp::config::asio_client> client;

void on_server_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::vector<int> data;
    const char* payload = msg->get_payload().data();
    std::size_t payload_len = msg->get_payload().length();
    if (payload_len % sizeof(int) == 0) {
        std::size_t num_ints = payload_len / sizeof(int);
        data.resize(num_ints);
        std::memcpy(data.data(), payload, payload_len);
    }

    // process data...
    std::cout << "Server received: ";
    for (int d : data) {
        std::cout << d << " ";
    }
    std::cout << std::endl;

    // send data back to client
    s->send(hdl, msg->get_payload(), msg->get_opcode());
}

void on_client_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    std::vector<int> data;
    const char* payload = msg->get_payload().data();
    std::size_t payload_len = msg->get_payload().length();
    if (payload_len % sizeof(int) == 0) {
        std::size_t num_ints = payload_len / sizeof(int);
        data.resize(num_ints);
        std::memcpy(data.data(), payload, payload_len);
    }

    // process data...
    std::cout << "Client received: ";
    for (int d : data) {
        std::cout << d << " ";
    }
    std::cout << std::endl;
}

int main() {
    server ws_server;
    client ws_client;

    // Initialize ASIO
    ws_server.init_asio();
    ws_client.init_asio();

    // Set up WebSocket server options
    ws_server.set_reuse_addr(true);

    // Register message handlers
    ws_server.set_message_handler(&on_server_message);
    ws_client.set_message_handler(&on_client_message);

    // Listen on port 12345
    ws_server.listen(12345);

    // Start the server
    ws_server.start_accept();

    // Connect to server
    websocketpp::lib::error_code ec;
    client::connection_ptr con = ws_client.get_connection("ws://localhost:12345", ec);
    if (ec) {
        std::cerr << "Error: Failed to connect to server. " << ec.message() << std::endl;
        return 1;
    }
    ws_client.connect(con);

    // Send message from client to server
    std::vector<int> data = {1, 2, 3, 4, 5};
    ws_client.send(con->get_handle(), websocketpp::frame::opcode::binary, data.data(), data.size() * sizeof(int), ec);
    if (ec) {
        std::cerr << "Error: Failed to send message. " << ec.message() << std::endl;
        return 1;
    }

    // Run the ASIO event loop
    ws_server.run();
    ws_client.run();

    return 0;
}