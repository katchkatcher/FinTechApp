#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sqlite3.h>


struct Stock {

	int id;					// id ������
	std::string ticker;		// �������� �����
	double purchasePrice;	// ���� �������
	int quantity;			// ����������� �����

};

struct Portfolio {
	int id;					// id ��������
	std::string name;		// �������� ��������
};

class AssetManager {
private:
	std::string dbPath;		// �������� ��
public:
	AssetManager(const std::string& databasePath);

	bool createDatabase();			// ������� ���� ������
	bool createPortfolioTable();	// ������� ������� ��������
	bool createStockTable();		// ������� ������� �����

	bool addPortfolio(const std::string& portfolioName);											// �������� ��������
	bool addStock(int portfolioId, const std::string& ticker, double purchasePrice, int quantity);	// ���������� ����� � ��������
	std::vector<Stock> getStocks(int portfolioId);													// ��������� ���� ����� �� ��������
	std::vector<Portfolio> getPortfolios();															// ��������� ������ ���������
	bool deleteAssetByName(int portfolioId, const std::string& stockName);							// �������� ������ �� ��� �����
	bool reindexStocks();																			// �������������� �������
	bool deleteDatabase();																			// ������� ���� ������

	void displayPortfolios(const std::vector<Portfolio>& portfolios);	// ���������� ��������
	void displayStocks(const std::vector<Stock>& stocks);				// ���������� �����
};
