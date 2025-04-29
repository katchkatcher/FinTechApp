#pragma once
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

struct StockData {
    std::string timestamp;   // Временная метка
    double closeprice;       // Цена закрытия
};

class DataConnector {
public:
    DataConnector(const std::string& userAgent); // Конструктор

    std::vector<StockData> fetchHistoricalData(const std::string& symbol, const std::string& interval, const std::string& range);                // Получение данных
    std::string formatTimestamp(std::time_t timestamp);                                                                                          // Преобразование времени
    void fetchAndDisplayDataWithRetries(const std::string& symbol, const std::string& interval, const std::string& range, int retryCount);       // Функция для отображения 
private:
    std::string userAgent; // User-Agent

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userData);                                               // Функция обработчик
    std::vector<StockData> parseJsonData(const std::string& jsonData);                                                                           // Парсер json

  
};
