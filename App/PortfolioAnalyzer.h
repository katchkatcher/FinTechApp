#pragma once
#include "DataConnector.h"
#include "DataManager.h"

class PortfolioAnalyzer {
private:
	// ������
	DataManager& dataManager;
	DataConnector& dataConnector; 

	// ������
	const std::vector<Stock>& allStocks;
	const std::vector<Portfolio>& allPortfolios;
	const std::map<std::string, double>& currentPrices;

public:

	PortfolioAnalyzer(DataConnector& connector, DataManager& dmanager);

	double calculateTotalValue();														// ������� ������� ���� �������� 
	double calculateAssetShare(const std::string& assetName);							// ������� ���� ������ � ��������
	double calculatePortfolioReturn();													// ������� ���������� ��������
	double calculateAssetVolatility(const std::string& assetName);						// ������� ������������� �����
	double calculatePortfolioVolatility();												// ������� ���������� ������������� ��������
	double calculateAssetReturn(const std::string& assetName);							// ������� ���������� �����
	double calculateCovariance(const std::string& asset1, const std::string& asset2);	// ����������
};

