#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "includes/httplib.h"
#include "includes/json.hpp"
#include "includes/cmdparser.hpp"
using json = nlohmann::json;

void configure_parser(cli::Parser& parser) {
    parser.set_optional<std::string>("t", "target", "localhost", "Sets the target url.");
    parser.set_optional<int>("p", "port", 8080, "Sets the target port.");
};

std::string process(httplib::Client& cli, std::string url) {
    for (int attempts = 1; attempts <= 3; ++attempts) {
        if (auto res = cli.Get(url)) {
            if (res->status == 200) {
                json myData = json::parse(res->body);
                if (!myData.empty())
                    return res->body;
                else
                    // Error Case: JSON Parsing failure
                    std::cout << "JSON Parse Error" << std::endl << "Trying again: Attempt " << attempts << " of 3" << std::endl;
            } else {
                // Error Case: Unsucessful response code.
                std::cout << "Response Code: " << res->status << std::endl << "Trying again: Attempt " << attempts << " of 3" << std::endl;
            }
        } else {
            // Error Case: Could not connect to given url.
            std::cout << "HTTP error: " << httplib::to_string(res.error()) << std::endl << "Trying again: Attempt " << attempts << " of 3" << std::endl;
        }

        if (attempts == 3) {
            std::cout << "Could not connect: Exiting program." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return "";
};

int main(int argc, char** argv) {
	cli::Parser parser(argc, argv);
    configure_parser(parser);
    parser.run_and_exit_if_error();

    httplib::Client cli(parser.get<std::string>("t"), parser.get<int>("p"));
    std::cout << "Attempting to Connect to " << parser.get<std::string>("t") << ":" << parser.get<int>("p") << std::endl;

    json data;
    std::string temp = "";

    // Initialize
    temp = process(cli, "/lights");
    if (temp == "")
        return -1;

    data = json::parse(temp);
    for (int i = 0; i < data.size(); i++){
        std::string url = "/lights/";
        url.append(data[i]["id"]);
        temp = process(cli, url);
        if (temp == "")
            return -1; 
        data[i] = json::parse(temp);
        int tempI = data[i]["brightness"];
        tempI /= 2.55;
        data[i]["brightness"] = tempI;
    }
    std::cout << data.dump(4) << std::endl;

    // Polls the server every 5 seconds for changes in data
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Fetch updated data
        temp = process(cli, "/lights");
        if (temp == "")
            return -1; 
        json newData = json::parse(temp);
        
        // Update existing lights
         for (int i = 0; i < data.size(); i++){
            bool found = false;
            for (int j = 0; j < newData.size(); j++){
                if (data[i]["id"] == newData[j]["id"]) {
                    std::string url = "/lights/";
                    url.append(data[i]["id"]);
                    std::string temp2 = process(cli, url);
                    if (temp2 == "")
                        return -1;
                    json tempData = json::parse(temp2);
                    if (data[i]["on"] != tempData["on"]){
                        data[i]["on"] = tempData["on"];
                        json dataOut = { {"id", data[i]["id"]},
                                         {"on", data[i]["on"]}};
                        std::cout << dataOut.dump(4) << std::endl;
                    }
                    int tempI = tempData["brightness"];
                    tempI /= 2.55;
                    if (data[i]["brightness"] != tempI){
                        data[i]["brightness"] = tempI;
                        json dataOut = { {"id", data[i]["id"]},
                                         {"brightness", data[i]["brightness"]}};
                        std::cout << dataOut.dump(4) << std::endl;
                    }

                    found = true;
                    newData.erase(j);
                    break;
                }
            }

            // Removes any lights not found within the system
            if (!found)
            {
                std::cout << data[i]["name"]  << " (" << data[i]["id"] << ") has been removed." << std::endl;
                data.erase(i);
                i--;
            }
        }

        // Add any new lights
        for (int j = 0; j < newData.size(); j++){
            std::string url = "/lights/";
            url.append(newData[j]["id"]);
            std::string temp2 = process(cli, url);
            if (temp2 == "")
                return -1;
            json tempData = json::parse(temp2);
            int tempI = tempData["brightness"];
            tempI /= 2.55;
            tempData["brightness"] = tempI;
            std::cout << tempData.dump(4) << std::endl;
            data[data.size()] = tempData;
        }
    }
}
