#pragma once
#include "DataConnector.h"
#include "DataManager.h"

class PortfolioAnalyzer {
private:
	// Ссылки
	DataManager& dataManager;
	DataConnector& dataConnector; 

	// Данные
	const std::vector<Stock>& allStocks;
	const std::vector<Portfolio>& allPortfolios;
	const std::map<std::string, double>& currentPrices;

public:

	PortfolioAnalyzer(DataConnector& connector, DataManager& dmanager);

	double calculateTotalValue();														// Подсчёт текущей цены портфеля 
	double calculateAssetShare(const std::string& assetName);							// Подсчёт доли актива в портфеле
	double calculatePortfolioReturn();													// Подсчёт доходности портфеля
	double calculateAssetVolatility(const std::string& assetName);						// Подсчёт волатильности акции
	double calculatePortfolioVolatility();												// Подсчёт взвешенной волатильности портфеля
	double calculateAssetReturn(const std::string& assetName);							// Подсчёт доходности акции
	double calculateCovariance(const std::string& asset1, const std::string& asset2);	// Ковариация
};

