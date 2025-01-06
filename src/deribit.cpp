#include"deribit.hpp"

std::atomic<bool> is_streaming(false);
std::map<int, std::string> symbols;

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::connection_hdl connection_hdl;

std::string formatTimestamp(long long timestamp);
std::shared_ptr<asio::ssl::context> on_tls_init(std::weak_ptr<void> hdl) {
    std::shared_ptr<asio::ssl::context> ctx =
        std::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);
    ctx->set_options(asio::ssl::context::sslv23 | asio::ssl::context::no_sslv2);
    return ctx;
}

using json = nlohmann::json;

std::string formatTimestamp(long long timestamp) {
    std::time_t time = timestamp / 1000;  // Convert to seconds
    std::tm* localTime = std::localtime(&time);  // Convert to local time
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

class DeribitStreamer {
public:
    DeribitStreamer();
    ~DeribitStreamer();
    void start_streaming(int symbol);
    void stop_streaming();

private:
    void run_streaming(int symbol);
    void subscribe_to_symbols(websocketpp::connection_hdl hdl, const std::vector<std::string>& symbols);
    void handle_message(const std::string& message);

    std::thread streaming_thread;
    websocketpp::client<websocketpp::config::asio_tls_client> deribit_client;  // Use the correct client
};

DeribitStreamer::DeribitStreamer() {}

void DeribitStreamer::start_streaming(int symbol) {
    if (is_streaming.load()) {
        std::cout << "Streaming is already running.\n";
        return;
    }

    is_streaming.store(true);
    streaming_thread = std::thread([this, symbol]() { run_streaming(symbol); });
}

void DeribitStreamer::stop_streaming() {
    if (!is_streaming.load()) return;
    is_streaming.store(false);

    if (streaming_thread.joinable()) {
        streaming_thread.join();
    }
    std::cout << "Stopped streaming.\n";
}

DeribitStreamer::~DeribitStreamer() {
    stop_streaming();
}

void DeribitStreamer::run_streaming(int symbol) {
    deribit_client.init_asio();
    deribit_client.set_tls_init_handler(&on_tls_init);

    deribit_client.set_message_handler([this](connection_hdl hdl, client::message_ptr msg) {
        handle_message(msg->get_payload());
    });

    deribit_client.set_open_handler([this, symbol](connection_hdl hdl) {
        subscribe_to_symbols(hdl, {symbols[symbol]});
    });

    websocketpp::lib::error_code ec;
    client::connection_ptr con = deribit_client.get_connection("wss://test.deribit.com/ws/api/v2", ec);

    if (ec) {
        std::cerr << "Error connecting to Deribit: " << ec.message() << std::endl;
        return;
    }

    deribit_client.connect(con);
    deribit_client.run();
}

void DeribitStreamer::subscribe_to_symbols(websocketpp::connection_hdl hdl, const std::vector<std::string>& symbols) {
    for (const auto& symbol : symbols) {
        json subscription = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "public/subscribe"},
            {"params", {{"channels", {"deribit_price_index." + symbol}}}}
        };

        try {
            deribit_client.send(hdl, subscription.dump(), websocketpp::frame::opcode::text);
            std::cout << "Subscribed to " << symbol << " price updates." << std::endl;
        } catch (const websocketpp::exception& e) {
            std::cerr << "Error sending subscription: " << e.what() << std::endl;
        }
    }
}

void DeribitStreamer::handle_message(const std::string& message) {
    try {
        json parsed = json::parse(message);
        std::string channel = parsed["params"]["channel"];
        std::string index_name = parsed["params"]["data"]["index_name"];
        auto priceJson = parsed["params"]["data"]["price"];
        long long timestamp = parsed["params"]["data"]["timestamp"];
        std::string price = priceJson.is_null() ? "N/A" : std::to_string(priceJson.get<double>());
        std::string formattedTimestamp = formatTimestamp(timestamp);

        std::cout << "\033[2J\033[H";  // Clear console
        std::cout << "---------------------------------------------\n";
        std::cout << "Channel: " << channel << "\n";
        std::cout << "Index Name: " << index_name << "\n";
        std::cout << "Price: " << price << "\n";
        std::cout << "Timestamp: " << formattedTimestamp << "\n";
        std::cout << "---------------------------------------------\n";
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << "\n";
    }
}

void webSock() {
    DeribitStreamer streamer;
    int choice;

    while (true) {
        std::cout << "\nMenu:\n";
        std::cout << "1. Subscribe to a new symbol\n";
        std::cout << "2. Start streaming\n";
        std::cout << "3. Stop streaming\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            int id;
            std::string symbol;
            std::cout << "Enter an ID for the symbol: ";
            std::cin >> id;
            std::cout << "Enter the symbol (e.g., btc_usd): ";
            std::cin >> symbol;

            symbols[id] = symbol;
            std::cout << "Symbol added successfully.\n";
        } else if (choice == 2) {
            int id;
            std::cout << "Available symbols:\n";
            for (const auto& [id, symbol] : symbols) {
                std::cout << id << ": " << symbol << "\n";
            }

            std::cout << "Enter the ID of the symbol to stream: ";
            std::cin >> id;

            if (symbols.find(id) != symbols.end()) {
                streamer.start_streaming(id);
            } else {
                std::cout << "Invalid ID. Please try again.\n";
            }
        } else if (choice == 3) {
            streamer.stop_streaming();
        } else if (choice == 4) {
            streamer.stop_streaming();
            break;
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

   
}
