#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sqlite3.h>


struct Stock {

	int id;					// id актива
	std::string ticker;		// название акции
	double purchasePrice;	// цена покупки
	int quantity;			// колличество акций

};

struct Portfolio {
	int id;					// id портфеля
	std::string name;		// название портфеля
};

class AssetManager {
private:
	std::string dbPath;		// Название БД
public:
	AssetManager(const std::string& databasePath);

	bool createDatabase();			// Создать базу данных
	bool createPortfolioTable();	// Создать таблицу портфеля
	bool createStockTable();		// Создать таблицу акций

	bool addPortfolio(const std::string& portfolioName);											// Добавить портфель
	bool addStock(int portfolioId, const std::string& ticker, double purchasePrice, int quantity);	// Добавление акции в портфель
	std::vector<Stock> getStocks(int portfolioId);													// Получение всех акций из портфеля
	std::vector<Portfolio> getPortfolios();															// Получение списка портфелей
	bool deleteAssetByName(int portfolioId, const std::string& stockName);							// Удаление актива по его имени
	bool reindexStocks();																			// Переиндексация активов
	bool deleteDatabase();																			// Удалить базу данных

	void displayPortfolios(const std::vector<Portfolio>& portfolios);	// Отобразить портфели
	void displayStocks(const std::vector<Stock>& stocks);				// Отобразить Акции
};
