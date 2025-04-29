#include "DataConnector.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <thread> // Для паузы
#include <chrono>

// Псевдоним для типа 
using json = nlohmann::json;

// Конструктор
DataConnector::DataConnector(const std::string& userAgent) : userAgent(userAgent) {}

// Функция-обработчик ответа сервера
size_t DataConnector::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userData) {

	size_t totalSize = size * nmemb;                // Получаем общий размер данных, полученных в результате запроса на сервер

	userData->append((char*)contents, totalSize);   // Преобразуем указатель так как записываем символы, добавляет totalSize байт из массива contents в строку userData

	return totalSize;
}

// Функция получения исторических данных с сайта
std::vector<StockData> DataConnector::fetchHistoricalData(const std::string& symbol, const std::string& interval, const std::string& range) {
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	std::string url = "https://query2.finance.yahoo.com/v8/finance/chart/" + symbol + "?interval=" + interval + "&range=" + range;

	curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("Failed to initialize CURL.");
	}

	for (int attempt = 0; attempt < 3; ++attempt) { // Максимум 3 попытки
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());               // формируем url для запроса
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);   // указываем функцию обработчик запроса 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);         // записывем в строку данные из запроса
		curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());   // указываем UserAgent

		res = curl_easy_perform(curl);  // совершаем запрос на сервер
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		else {
			// Если обнаружена ошибка
			long responseCode = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

			if (responseCode == 429) { // Too Many Requests
				std::cerr << "429 Too Many Requests: Retrying after delay..." << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(5));
				continue; // Повторить запрос
			}
			else if (responseCode >= 400) { // Другие ошибки
				throw std::runtime_error("Server returned error: " + std::to_string(responseCode));
			}

			break; // Успешный запрос
		}
	}

	curl_easy_cleanup(curl);    // очищаем объект

	if (readBuffer.empty()) {
		throw std::runtime_error("Empty response from server.");
	}

	return parseJsonData(readBuffer);
}


// JSON Парсер
std::vector<StockData> DataConnector::parseJsonData(const std::string& jsonData) {
	std::vector<StockData> stockDataList;

	try {
		auto jsonResponse = json::parse(jsonData);
		auto timestamps = jsonResponse["chart"]["result"][0]["timestamp"];                          // парсим временные метки
		auto closeValues = jsonResponse["chart"]["result"][0]["indicators"]["quote"][0]["close"];   // парсим цены закрытия

		for (size_t i = 0; i < timestamps.size(); i++) {
			if (!closeValues[i].is_null()) {
				StockData data;
				data.timestamp = formatTimestamp(timestamps[i]);		// записываем временную метку в объект класса StockData
				data.closeprice = closeValues[i];						// записываем цену закрытия
				stockDataList.push_back(data);
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "JSON parsing failed: " << e.what() << std::endl;
	}

	return stockDataList;
}

// Функция преобразования времени
std::string DataConnector::formatTimestamp(std::time_t timestamp)
{
	std::tm timeInfo; // Локальная структура для времени

	// Безопасное преобразование временной метки в локальное время(время преобразуется из кол-ва секунд, прошедших с 01.01.1970 00:00:00 UTC)
	if (localtime_s(&timeInfo, &timestamp) != 0) {
		throw std::runtime_error("Failed to convert timestamp to local time.");
	}

	// Форматирование даты
	std::ostringstream oss;                                 // выходной поток для преобразования даты в строку
	oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");   // записываем преобразованное время в буффер
	return oss.str();                                       // преобразовываем в строку
}

// Получение и отображение данных с интервалом
void DataConnector::fetchAndDisplayDataWithRetries(const std::string& symbol, const std::string& interval, const std::string& range, int retryCount)
{
	for (int i = 0; i < retryCount; ++i) {
		try {
			auto data = fetchHistoricalData(symbol, interval, range);

			std::cout << "Дата              \tЦена закрытия" << std::endl;
			for (const auto& entry : data) {
				std::cout << entry.timestamp << "\t" << std::fixed << std::setprecision(2) << entry.closeprice << std::endl;
			}
			return; // Успешное выполнение
		}
		catch (const std::exception& e) {
			std::cerr << "Attempt " << (i + 1) << " failed: " << e.what() << std::endl;

			if (i + 1 < retryCount) {
				std::this_thread::sleep_for(std::chrono::seconds(5));
			}
			else {
				std::cerr << "All attempts failed. Exiting." << std::endl;
				throw; // Проброс ошибки, если все попытки провалились
			}
		}
	}
}
