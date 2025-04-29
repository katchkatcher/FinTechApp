#pragma once

#include "DataConnector.h"
#include "AssetManager.h"

class DataManager {
private:
	std::vector<Portfolio> allPortfolios;							// Все портфели
	std::vector<Stock> allStocks;									// Акции из определённого портфеля
	std::map<std::string, double> currentPrices;					// Акция в портфеле и её текущая цена
	std::map<std::string, std::vector<double>> historicalPrices;	// Исторические цены всех активов портфеля

	// Ссылки
	AssetManager& assetManager;
	DataConnector& dataConnector;

public:
	DataManager(AssetManager& assetManager, DataConnector& dataConnector);	

	void choosePortfolio();														// Выбор портфеля									
	const std::vector<Portfolio>& getAllPortfolios();							// Получение всех портфелей
	const std::vector<Stock>& getAllStocks();									// Получение всех акций
	const std::map<std::string, double>& getCurrentPrices();					// Получение текущих цен
	const std::map<std::string, std::vector<double>>& getHistoricalPrices();	// Получение исторических цен

};