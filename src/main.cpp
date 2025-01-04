#include <iostream>
#include <string>
#include <curl/curl.h>
#include "./json/single_include/nlohmann/json.hpp"
#include <sstream>
#include"deribit.hpp"
// Callback function for handling the response
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// Function to get the access token
void getPositions(const std::string& access_token) {
    CURL* curl;
    CURLcode res;
    std::string response_string;

    // Prompt user for currency and kind
    std::string currency;
    std::cout << "Enter Currency: ";
    std::cin >> currency;
    std::string kind;
    std::cout << "Enter kind (future, spot, ...): ";
    std::cin >> kind;

    // Construct the URL with query parameters
    std::ostringstream url_stream;
    url_stream << "https://test.deribit.com/api/v2/private/get_positions"
               << "?currency=" << currency
               << "&kind=" << kind;
    std::string url = url_stream.str();

    // Initialize CURL
    curl = curl_easy_init();
    if (curl) {
        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set headers
        struct curl_slist* headers = nullptr;
        std::string auth_header = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, auth_header.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up the response handling
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            try {
                // Parse the JSON response
                auto json_response = nlohmann::json::parse(response_string);

                // Check if "result" exists and display it
                if (json_response.contains("result")) {
                    std::cout << "Result: " << json_response["result"].dump(4) << std::endl;
                    std::cerr << std::endl;

                } else {
                    std::cerr << "No 'result' field found in response." << std::endl;
                }
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
    }
}




void getOpenOrders(const std::string &access_token)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return;
    }

    std::string url = "https://test.deribit.com/api/v2/private/get_open_orders";
    std::string auth_header = "Authorization: Bearer " + access_token;
    const std::string content_type_header = "Content-Type: application/json";

    // Add headers
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, content_type_header.c_str());

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
    }
    else
    {
        try
        {
            // Parse the JSON response
            nlohmann::json json_response = nlohmann::json::parse(response_data);

            // Pretty print the JSON response
            std::cout << "Formatted Server Response:\n"
                      << json_response.dump(4) // 4 spaces for indentation
                      << std::endl;
            std::cout << std::endl;
        }
        catch (const nlohmann::json::parse_error &e)
        {
            std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
            std::cerr << "Raw Response: " << response_data << std::endl;
        }
    }

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void cancelOrderGet(const std::string &bearer_token)
{
    CURL *curl;
    CURLcode res;
    std::string order_id;
    std::cout << "order id: ";
    std::cin >> order_id;
    std::string url = "https://test.deribit.com/api/v2/private/cancel?order_id=" + order_id;
    struct curl_slist *headers = NULL;
    std::string auth_header = "Authorization: Bearer " + bearer_token;
    std::string content_type_header = "Content-Type: application/json";

    curl = curl_easy_init();
    if (curl)
    {
        // Add headers
        headers = curl_slist_append(headers, auth_header.c_str());
        headers = curl_slist_append(headers, content_type_header.c_str());

        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Add headers
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            std::cout << "Order cancellation request sent successfully." << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    else
    {
        std::cerr << "Failed to initialize CURL." << std::endl;
    }
}

// Function to get the access token
void editOrder(const std::string& access_token) {
    CURL* curl;
    CURLcode res;
    std::string response_string;

    // Prompt user for new price and amount
    double price, amount;
    std::string order_id;
    std::cout << "Enter Order Id: ";
    std::cin >> order_id;
    std::cout << "Enter the new price: ";
    std::cin >> price;
    std::cout << "Enter the new amount: ";
    std::cin >> amount;

    // Construct the URL with query parameters
    std::ostringstream url_stream;
    url_stream << "https://test.deribit.com/api/v2/private/edit"
           << "?amount=" << amount
           << "&order_id=" << order_id
           << "&price=" << price;
    std::string url = url_stream.str();

    // Initialize CURL
    curl = curl_easy_init();
    if (curl) {
        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set headers
        struct curl_slist* headers = nullptr;
        std::string auth_header = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, auth_header.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up the response handling
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // Perform the request
        res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
            std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            try {
                // Parse the JSON response
                auto json_response = nlohmann::json::parse(response_string);

                // Check if "result" exists and display it
                if (json_response.contains("result")) {
                    std::cout << "Result: " << json_response["result"].dump(4) << std::endl;
                    std::cerr << std::endl;
                } else {
                    std::cerr << "No 'result' field found in response." << std::endl;
                }
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
    }
}

std::string getAccessToken()
{
    CURL *curl;
    CURLcode res;
    std::string response_string;
    std::string post_data = R"({
        "jsonrpc": "2.0",
        "method": "public/auth",
        "params": {
            "grant_type": "client_credentials",
            "scope": "session:apiconsole-5kq5ea8upw2 expires:2592000",
            "client_id": "3lLxSV83",
            "client_secret": "VRadKIoIikig5W8jflfQPbH05yRzwXBI9QIZZmqY0w4"
        },
        "id": 0
    })";

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://test.deribit.com/api/v2/public/auth");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "CURL failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    
    return response_string;
}

// Function to place an order
void placeOrder(const std::string &access_token)
{
    CURL *curl;
    CURLcode res;
    std::string response_string;
    std::string url;

    // Get order details from the user
    std::string instrument;
    double quantity, price, amount;
    std::string side, order_type;

    std::cout << "Enter instrument (e.g., BTC-USD): ";
    std::cin >> instrument;
    std::cout << "Enter quantity: ";
    std::cin >> quantity;
    std::cout << "Enter price: ";
    std::cin >> amount;
    std::cout << "Enter amount: ";
    std::cin >> price;
    std::cout << "Enter side (buy/sell): ";
    std::cin >> side;
    std::cout << "Enter order type (limit/market): ";
    std::cin >> order_type;

    // Decide the correct endpoint based on the side (buy/sell)
    if (side == "buy")
    {
        url = "https://test.deribit.com/api/v2/private/buy"; // Buy order endpoint
    }
    else if (side == "sell")
    {
        url = "https://test.deribit.com/api/v2/private/sell"; // Sell order endpoint
    }
    else
    {
        std::cerr << "Invalid side. Use 'buy' or 'sell'.\n";
        return;
    }

    // Prepare the JSON body for the order using nlohmann::json
    nlohmann::json root;
    root["jsonrpc"] = "2.0";
    root["method"] = (side == "buy") ? "private/buy" : "private/sell";
    root["params"]["instrument_name"] = instrument;
    root["params"]["quantity"] = quantity;
    root["params"]["amount"] = amount;
    root["params"]["price"] = price;
    root["params"]["type"] = order_type;
    root["params"]["access_token"] = access_token;

    std::string requestBody = root.dump(); // Serializing the JSON object to string

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
            std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            try {
                // Parse the JSON response
                auto json_response = nlohmann::json::parse(response_string);

                // Check if "result" exists and display it
                if (json_response.contains("result")) {
                    std::cout << "Result: " << json_response["result"].dump(4) << std::endl;
                    std::cerr << std::endl;

                } else {
                    std::cerr << "No 'result' field found in response." << std::endl;
                }
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main()
{
    // Retrieve access token from the API
    std::string response = getAccessToken();

    // Parse the JSON response with nlohmann::json
    try
    {
        nlohmann::json responseJson = nlohmann::json::parse(response);
        std::string access_token = responseJson["result"]["access_token"];

        // std::cout << "Access Token: " << access_token << std::endl;

        // Display menu and let user choose to place an order
        while (true)
        {
            std::cout << "\nMenu:\n";
            std::cout << "1. Place an Order\n";
            std::cout << "2. Modify an Order\n";
            std::cout << "3. Get Open Orders\n";
            std::cout << "4. View Current Positions\n";
            std::cout << "5. Cancel an Order\n";
            std::cout << "6. Real-time market data streaming\n";
            std::cout << "7. Exit\n";
            std::cout << "Choose an option: ";

            int choice;
            std::cin >> choice;
            if (std::cin.fail())
            {
                std::cin.clear();                                                   // Clear error flags
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                std::cout << "Invalid input. Please enter a number between 1 and 5.\n";
                continue;
            }
            switch (choice)
            {
            case 1:
                placeOrder(access_token);
                break;
            case 2:
                editOrder(access_token);
                break;
            case 3:
                getOpenOrders(access_token);
                break;
            case 4:
                getPositions(access_token);
                break;
            case 5:
                cancelOrderGet(access_token);
                break;
            case 6:
                webSock();
                break;
            case 7:
                std::cout << "Exiting program...\n";
                return 0;
            default:
                std::cout << "Invalid option. Please try again.\n";
            }
        }
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "Failed to parse the response: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

