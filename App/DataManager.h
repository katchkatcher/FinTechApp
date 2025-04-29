#pragma once

#include "DataConnector.h"
#include "AssetManager.h"

class DataManager {
private:
	std::vector<Portfolio> allPortfolios;							// ��� ��������
	std::vector<Stock> allStocks;									// ����� �� ������������ ��������
	std::map<std::string, double> currentPrices;					// ����� � �������� � � ������� ����
	std::map<std::string, std::vector<double>> historicalPrices;	// ������������ ���� ���� ������� ��������

	// ������
	AssetManager& assetManager;
	DataConnector& dataConnector;

public:
	DataManager(AssetManager& assetManager, DataConnector& dataConnector);	

	void choosePortfolio();														// ����� ��������									
	const std::vector<Portfolio>& getAllPortfolios();							// ��������� ���� ���������
	const std::vector<Stock>& getAllStocks();									// ��������� ���� �����
	const std::map<std::string, double>& getCurrentPrices();					// ��������� ������� ���
	const std::map<std::string, std::vector<double>>& getHistoricalPrices();	// ��������� ������������ ���

};