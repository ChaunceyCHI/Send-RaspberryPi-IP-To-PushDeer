#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/asio.hpp> // for IP address handling
#include <httplib.h>      // for HTTP requests
#include <nlohmann/json.hpp> // for JSON parsing
#include <thread>         // for std::this_thread::sleep_for
#include <chrono>         // for std::chrono::seconds

// Function to get the local IPs
std::string get_all_ips() {
    FILE* fp = popen("ifconfig", "r");
    if (!fp) {
        std::cerr << "Failed to run ifconfig command\n";
        return "";
    }

    char buffer[256];  // Buffer for reading each line
    std::string line;
    std::string interface_name;
    std::ostringstream result;  // Stores the final string

    // Read ifconfig output line by line
    while (fgets(buffer, sizeof(buffer), fp)) {
        line = buffer;

        // Extract interface name
        if (line.find(':') != std::string::npos && line.find("flags") != std::string::npos) {
            interface_name = line.substr(0, line.find(':'));
        }

        // Extract IPv4 address (exclude IPv6)
        if (line.find("inet ") != std::string::npos && line.find("inet6") == std::string::npos) {
            std::istringstream iss(line);
            std::string ignore, ip_address;
            iss >> ignore >> ip_address;  // Ignore "inet", capture IP address
            result << interface_name << ": " << ip_address << "\n";  // Add to result
        }
    }

    pclose(fp);
    return result.str();  // Return the final result string
}

int main() {
    std::string ipAddress = get_all_ips();
    std::cout << "IP Address Info:\n" << ipAddress << std::endl;

    // Setup SSLClient for HTTPS requests
    httplib::SSLClient cli("api2.pushdeer.com");
    cli.enable_server_certificate_verification(false);

    // Encode URL and prepare the push request
    std::string encoded_text = httplib::detail::encode_url(ipAddress);
    std::string url = "/message/push?pushkey=[insert your key of pushdeer]&text=" + encoded_text; // insert your key of pushdeer

    std::string success_value = "none";
    int max_attempts = 50;  // Max number of retry attempts
    int attempts = 0;       // Current attempt count

    // Retry the push request until success or max attempts reached
    while (success_value != "ok" && attempts < max_attempts) {
        auto res = cli.Get(url.c_str());
        if (res && res->status == 200) {
            std::cout << "Response body: " << res->body << std::endl; 
            auto parsed_data = nlohmann::json::parse(res->body);

            // Validate JSON structure
            if (parsed_data.contains("content") && parsed_data["content"].contains("result")) {
                auto result_data = parsed_data["content"]["result"];

                // Check if result is an array and contains at least one element
                if (result_data.is_array() && !result_data.empty()) {
                    auto first_result_string = result_data[0];

                    // Ensure first_result_string is a string and parse it as JSON
                    if (first_result_string.is_string()) {
                        auto first_result = nlohmann::json::parse(first_result_string.get<std::string>());

                        // Check if "success" is a valid string
                        if (first_result.contains("success") && first_result["success"].is_string()) {
                            success_value = first_result["success"];
                            std::cout << "Success field value: " << success_value << std::endl;

                            // Exit the loop if the request is successful
                            if (success_value == "ok") {
                                break;
                            }
                        } else {
                            std::cerr << "Error: 'success' field is not a string or missing." << std::endl;
                        }
                    }
                } else {
                    std::cerr << "Error: 'result' field is not an array or is empty." << std::endl;
                }
            } else {
                std::cerr << "Error: 'content' or 'result' field is missing." << std::endl;
            }
        } else {
            std::cerr << "Error in request: " << res.error() << std::endl;
        }

        attempts++;
        if (attempts < max_attempts) {
            std::cout << "Retrying in 1 second... (" << attempts << "/" << max_attempts << ")" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Wait for 1 second before retrying
        }
    }

    if (success_value != "ok") {
        std::cerr << "Failed to push message after " << max_attempts << " attempts." << std::endl;
    }

    return 0;
}
