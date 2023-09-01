#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <iterator>
#include <curl.h>

const std::string BASE_URL = "https://www.sahibinden.com";
const std::string LOGIN_URL = BASE_URL + "/login";
const std::string LINK1_URL = BASE_URL + "/link1";
const std::string SEARCH_TEXT = "YourSearchTextHere";
const std::string USERNAME = "YourUsernameHere";
const std::string PASSWORD = "YourPasswordHere";

std::string httpGet(const std::string& url) {
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void *data, size_t size, size_t count, void *userp) -> size_t {
            size_t totalSize = size * count;
            static_cast<std::string*>(userp)->append(static_cast<char*>(data), totalSize);
            return totalSize;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "HTTP GET request failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    return response;
}

// Function to perform an HTTP POST request using libcurl
std::string httpPost(const std::string& url, const std::string& postdata) {
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void *data, size_t size, size_t count, void *userp) -> size_t {
            size_t totalSize = size * count;
            static_cast<std::string*>(userp)->append(static_cast<char*>(data), totalSize);
            return totalSize;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "HTTP POST request failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    return response;
}

int main() {
    std::string loginResponse = httpGet(LOGIN_URL);

    std::regex csrfRegex("<input type=\"hidden\" name=\"_csrf_token\" value=\"(.*?)\">");
    std::smatch csrfMatch;
    if (std::regex_search(loginResponse, csrfMatch, csrfRegex)) {
        std::string csrfToken = csrfMatch[1];

        std::string postdata = "_csrf_token=" + csrfToken + "&username=" + USERNAME + "&password=" + PASSWORD;

        std::string loginResponsePost = httpPost(LOGIN_URL, postdata);

        if (loginResponsePost.find("Welcome, " + USERNAME) != std::string::npos) {
            std::cout << "Login successful." << std::endl;

            std::string link1Response = httpGet(LINK1_URL);

            if (link1Response.find(SEARCH_TEXT) != std::string::npos) {
                std::cout << "Found the text: " << SEARCH_TEXT << std::endl;
            } else {
                std::cout << "Text not found." << std::endl;
            }
        } else {
            std::cout << "Login failed." << std::endl;
        }
    } else {
        std::cerr << "CSRF token not found." << std::endl;
    }

    return 0;
}
