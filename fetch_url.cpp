#include <iostream>
#include <string>
#include <curl/curl.h>

// Структура для хранения ответа
struct CurlResponse
{
    std::string data;
};

// Callback-функция: вызывается libcurl при получении данных
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    CurlResponse *response = static_cast<CurlResponse *>(userp);
    response->data.append(static_cast<char *>(contents), realsize);
    return realsize;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Использование: " << argv[0] << " <URL>\n";
        return 1;
    }

    const char *url = argv[1];

    // Инициализация libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if (!curl)
    {
        std::cerr << "Не удалось инициализировать curl\n";
        curl_global_cleanup();
        return 1;
    }

    CurlResponse response;

    // Настройка запроса
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Выполнение запроса
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "Ошибка curl: " << curl_easy_strerror(res) << '\n';
    }
    else
    {
        std::cout << response.data;
    }

    // Очистка
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return (res == CURLE_OK) ? 0 : 1;
}