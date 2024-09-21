#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/asio.hpp> // for IP address handling
#include <boost/asio/ip/host_name.hpp>
#include <httplib.h>      // for HTTP requests
#include <nlohmann/json.hpp> // for JSON parsing

// Function to get the local IP
std::string get_local_ip() {
    boost::asio::io_context context;
    boost::asio::ip::tcp::resolver resolver(context);
    auto results = resolver.resolve(boost::asio::ip::host_name(), "");
    return results.begin()->endpoint().address().to_string();
}

// Function to get all interfaces (limited to boost::asio context)
std::unordered_map<std::string, std::vector<std::string>> get_all_interfaces() {
    std::unordered_map<std::string, std::vector<std::string>> interface_info;
    
    // Here, you would need to use OS-specific ways to get the network interfaces
    // Example on Linux: using ioctl and SIOCGIFCONF for interface discovery
    // For simplicity, let's assume we fetch interfaces manually or use boost functions
    
    // TODO: Implement OS-specific code to fetch interfaces and add to `interface_info`
    
    return interface_info;
}

int main() {
    // Fetch network interface information
    auto all_interfaces = get_all_interfaces();
    std::string sourceString;

    for (const auto& [interface, ips] : all_interfaces) {
        for (const auto& ip : ips) {
            sourceString += interface + ":" + ip + "------";
        }
    }

    std::cout << "Source String: " << sourceString << std::endl;

    // 使用 SSLClient 代替 Client 以支持 HTTPS
    #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    httplib::SSLClient cli("api2.pushdeer.com");
    #else
    httplib::Client cli("api2.pushdeer.com");
    #endif

    std::string url = "/message/push?pushkey=[输入pushdeer的KEY]&text=" + httplib::detail::encode_url(sourceString); // 输入PUSHDEER的key

    std::string success_value = "none";

    // Retry until "ok" is received
    while (success_value != "ok") {
        auto res = cli.Get(url.c_str());
        if (res && res->status == 200) {
            auto parsed_data = nlohmann::json::parse(res->body);
            auto result_data = parsed_data["content"]["result"][0];
            success_value = result_data["success"];
            std::cout << "success字段的值: " << success_value << std::endl;
        } else {
            std::cerr << "Error in request: " << res.error() << std::endl;
        }
    }

    return 0;
}