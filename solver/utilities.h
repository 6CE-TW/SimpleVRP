#include <iostream>

#ifndef CRP_CRP_H_
#include <cpr/cpr.h>
#define CRP_CRP_H_
#endif
#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#ifndef UTILITIES_H_
#define UTILITIES_H_

const bool TEST_CPR_WITH_HTTPBIN = false;
const bool TEST_CPR_WITH_OSRM = false;

inline int VerifyCprCondition()
{
  if (TEST_CPR_WITH_HTTPBIN)
  {
    std::cout << "[INFO] Sending request to https://httpbin.org/get" << std::endl;
    cpr::Response r = cpr::Get(cpr::Url{"https://httpbin.org/get"});
    std::cout << "[INFO] Request done." << std::endl;

    if (r.error)
    {
      std::cout << "r.error" << std::endl;
      std::cerr << "[ERROR] CPR failed:\n"
                << "  code    = " << static_cast<int>(r.error.code) << "\n"
                << "  message = " << r.error.message << "\n";
      return 1;
    }
    else
    {
      std::cout << "NOT r.error" << std::endl;
      std::cout << "Status code: " << r.status_code << "\n";
      try
      {
        // transform returned text to json object
        nlohmann::json j = nlohmann::json::parse(r.text);

        std::cout << "Response JSON:\n"
                  << j.dump(2) << "\n";

        // example: get certain data
        if (j.contains("headers") && j["headers"].contains("Host"))
        {
          std::cout << "Host header: " << j["headers"]["Host"] << "\n";
        }
      }
      catch (nlohmann::json::parse_error &e)
      {
        std::cerr << "[ERROR] JSON parse failed: " << e.what() << "\n";
        return 1;
      }
    }
  }
  else if (TEST_CPR_WITH_OSRM)
  {
    std::cout << "[INFO] Sending request to 35.194.198.57:8000/test" << std::endl;
    cpr::Response r1 = cpr::Get(cpr::Url{"http://35.194.198.57:8000/test"});
    std::cout << "[INFO] Request done." << std::endl;

    if (r1.status_code != 200)
    {
      std::cerr << "Failed to get data from server: " << r1.status_code << "\n";
      return 1;
    }

    std::cout << "Status code: " << r1.status_code << "\n";
    std::cout << "Response body:\n"
              << r1.text << "\n";
    try
    {
      // transform returned text to json object
      nlohmann::json j1 = nlohmann::json::parse(r1.text);

      std::cout << "Response JSON:\n"
                << j1.dump(2) << "\n";

      // example: get certain data
      if (j1.contains("message"))
      {
        std::cout << "message: " << j1.at("message").get<std::string>() << "\n";
      }
    }
    catch (nlohmann::json::parse_error &e)
    {
      std::cerr << "[ERROR] JSON parse failed: " << e.what() << "\n";
      return 1;
    }

    std::string url = "http://35.194.198.57:8000/distance-matrix";

    // make JSON body to pass
    std::string body = R"([[121.517,25.0477],[121.5168,25.0443]])";

    cpr::Response r2 = cpr::Post(
        cpr::Url{url},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{body});

    if (r2.status_code == 200)
    {
      auto json = nlohmann::json::parse(r2.text);
      std::cout << json.dump(2) << std::endl;
    }
    else
    {
      std::cerr << "Error: " << r2.status_code << std::endl;
      return 1;
    }
  }

  return 0;
}

#endif