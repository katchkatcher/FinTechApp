#pragma once

#include "AssetManager.h"
#include "PortfolioAnalyzer.h"
#include "DataManager.h"
#include <random>
#include <numeric>

class RiskModeler {
private:
	// ������
	AssetManager& assetManager;
	PortfolioAnalyzer& portfolioAnalyzer;
	DataConnector& dataConnector;
	DataManager& dataManager;

	// ������
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
	
	double calculateFuturePrice(std::string assetName);										// ������� ������� ����
	double calculatePortfolioRisk();														// ������� ����� ��������
	double calculateFuturePortfolioReturn();												// ������� ������� ���������� ��������
	double calculateVaR(double confidenceLevel, int daysToForecast, int numSimulations);	// ������� �������� ��� ������
	void initialize();																		// ������������� �������������� �������
	void calculateCovMatrix();																// ������� �������������� �������
	void calculateLogReturns();																// ������� ��������������� ����������
	void calculateMeanReturns();															// ������� ����� �������
};

