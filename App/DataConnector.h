#pragma once
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

struct StockData {
    std::string timestamp;   // ��������� �����
    double closeprice;       // ���� ��������
};

class DataConnector {
public:
    DataConnector(const std::string& userAgent); // �����������

    std::vector<StockData> fetchHistoricalData(const std::string& symbol, const std::string& interval, const std::string& range);                // ��������� ������
    std::string formatTimestamp(std::time_t timestamp);                                                                                          // �������������� �������
    void fetchAndDisplayDataWithRetries(const std::string& symbol, const std::string& interval, const std::string& range, int retryCount);       // ������� ��� ����������� 
private:
    std::string userAgent; // User-Agent

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userData);                                               // ������� ����������
    std::vector<StockData> parseJsonData(const std::string& jsonData);                                                                           // ������ json

  
};
