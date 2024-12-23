#include <iostream>
#include <string>
#include <curl/curl.h>

#include "json.hpp"
using json = nlohmann::json;
using namespace std;
class Order
{
private:
    string clinetID;
    string clientSecret;
public:
    Order():clinetID("yOIyFnVV"),clientSecret("drz0-qSHhFrcl77Sfbnw6zNM2gSSiY5l1ZkSZJGc0Ng"){}
    void* genToken()
    {
        string url = "https://test.deribit.com/api/v2/public/auth";
        string params = "?client_id=" + clinetID + "&client_secret=" + clientSecret + "&grant_type=client_credentials";

        string fullPath = url + params;
        CURL *curl = curl_easy_init();
        if(curl) 
        {
            CURLcode res;
            curl_easy_setopt(curl, CURLOPT_URL, fullPath.c_str());
            res = curl_easy_perform(curl);
            // cout << res << endl;
            curl_easy_cleanup(curl);
        }
        return curl;
    }
    void placeOrder(void *accessToken){
        accessToken = genToken();

    }
    void cancelOrder();
    void modifyOrder();
    void getOrder();
    void viewCurrentPositions ();
    ~Order(){}
};

int main()
{
    Order a;
    a.genToken();
    // return 0;
}