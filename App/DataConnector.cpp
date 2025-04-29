#include "DataConnector.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <thread> // ��� �����
#include <chrono>

// ��������� ��� ���� 
using json = nlohmann::json;

// �����������
DataConnector::DataConnector(const std::string& userAgent) : userAgent(userAgent) {}

// �������-���������� ������ �������
size_t DataConnector::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userData) {

	size_t totalSize = size * nmemb;                // �������� ����� ������ ������, ���������� � ���������� ������� �� ������

	userData->append((char*)contents, totalSize);   // ����������� ��������� ��� ��� ���������� �������, ��������� totalSize ���� �� ������� contents � ������ userData

	return totalSize;
}

// ������� ��������� ������������ ������ � �����
std::vector<StockData> DataConnector::fetchHistoricalData(const std::string& symbol, const std::string& interval, const std::string& range) {
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	std::string url = "https://query2.finance.yahoo.com/v8/finance/chart/" + symbol + "?interval=" + interval + "&range=" + range;

	curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("Failed to initialize CURL.");
	}

	for (int attempt = 0; attempt < 3; ++attempt) { // �������� 3 �������
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());               // ��������� url ��� �������
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);   // ��������� ������� ���������� ������� 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);         // ��������� � ������ ������ �� �������
		curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());   // ��������� UserAgent

		res = curl_easy_perform(curl);  // ��������� ������ �� ������
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		else {
			// ���� ���������� ������
			long responseCode = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

			if (responseCode == 429) { // Too Many Requests
				std::cerr << "429 Too Many Requests: Retrying after delay..." << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(5));
				continue; // ��������� ������
			}
			else if (responseCode >= 400) { // ������ ������
				throw std::runtime_error("Server returned error: " + std::to_string(responseCode));
			}

			break; // �������� ������
		}
	}

	curl_easy_cleanup(curl);    // ������� ������

	if (readBuffer.empty()) {
		throw std::runtime_error("Empty response from server.");
	}

	return parseJsonData(readBuffer);
}


// JSON ������
std::vector<StockData> DataConnector::parseJsonData(const std::string& jsonData) {
	std::vector<StockData> stockDataList;

	try {
		auto jsonResponse = json::parse(jsonData);
		auto timestamps = jsonResponse["chart"]["result"][0]["timestamp"];                          // ������ ��������� �����
		auto closeValues = jsonResponse["chart"]["result"][0]["indicators"]["quote"][0]["close"];   // ������ ���� ��������

		for (size_t i = 0; i < timestamps.size(); i++) {
			if (!closeValues[i].is_null()) {
				StockData data;
				data.timestamp = formatTimestamp(timestamps[i]);		// ���������� ��������� ����� � ������ ������ StockData
				data.closeprice = closeValues[i];						// ���������� ���� ��������
				stockDataList.push_back(data);
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "JSON parsing failed: " << e.what() << std::endl;
	}

	return stockDataList;
}

// ������� �������������� �������
std::string DataConnector::formatTimestamp(std::time_t timestamp)
{
	std::tm timeInfo; // ��������� ��������� ��� �������

	// ���������� �������������� ��������� ����� � ��������� �����(����� ������������� �� ���-�� ������, ��������� � 01.01.1970 00:00:00 UTC)
	if (localtime_s(&timeInfo, &timestamp) != 0) {
		throw std::runtime_error("Failed to convert timestamp to local time.");
	}

	// �������������� ����
	std::ostringstream oss;                                 // �������� ����� ��� �������������� ���� � ������
	oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");   // ���������� ��������������� ����� � ������
	return oss.str();                                       // ��������������� � ������
}

// ��������� � ����������� ������ � ����������
void DataConnector::fetchAndDisplayDataWithRetries(const std::string& symbol, const std::string& interval, const std::string& range, int retryCount)
{
	for (int i = 0; i < retryCount; ++i) {
		try {
			auto data = fetchHistoricalData(symbol, interval, range);

			std::cout << "����              \t���� ��������" << std::endl;
			for (const auto& entry : data) {
				std::cout << entry.timestamp << "\t" << std::fixed << std::setprecision(2) << entry.closeprice << std::endl;
			}
			return; // �������� ����������
		}
		catch (const std::exception& e) {
			std::cerr << "Attempt " << (i + 1) << " failed: " << e.what() << std::endl;

			if (i + 1 < retryCount) {
				std::this_thread::sleep_for(std::chrono::seconds(5));
			}
			else {
				std::cerr << "All attempts failed. Exiting." << std::endl;
				throw; // ������� ������, ���� ��� ������� �����������
			}
		}
	}
}
