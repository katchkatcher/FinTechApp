#pragma once

#include "AssetManager.h"
#include "PortfolioAnalyzer.h"
#include "DataManager.h"
#include <random>
#include <numeric>

class RiskModeler {
private:
	// Ссылки
	AssetManager& assetManager;
	PortfolioAnalyzer& portfolioAnalyzer;
	DataConnector& dataConnector;
	DataManager& dataManager;

	// Данные
	const std::vector<Portfolio>& allPortfolios;
	const std::vector<Stock>& allStocks;
	const std::map<std::string, double>& currentPrices;
	const std::map<std::string, std::vector<double>>& historicalPrices;
	size_t numAssets;
	std::map<std::string, std::vector<double>> logReturns;
	std::vector<std::vector<double>> covMatrix;
	std::map<std::string, double> meanReturns;
	bool initializer = false;

public:
	RiskModeler(AssetManager& manager, PortfolioAnalyzer& analyzer, DataConnector& connector, DataManager& dmanager);
	
	double calculateFuturePrice(std::string assetName);										// Подсчёт будущей цены
	double calculatePortfolioRisk();														// Подсчёт риска портфеля
	double calculateFuturePortfolioReturn();												// Подсчёт будущей доходности портфеля
	double calculateVaR(double confidenceLevel, int daysToForecast, int numSimulations);	// Подсчёт значения под риском
	void initialize();																		// Инициализация ковариационной матрицы
	void calculateCovMatrix();																// Подсчёт ковариационной матрицы
	void calculateLogReturns();																// Подсчёт логарифмической доходности
	void calculateMeanReturns();															// Подсчёт долей активов
};

