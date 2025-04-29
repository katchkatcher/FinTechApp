#include "DataManager.h"

DataManager::DataManager(AssetManager& manager, DataConnector& connector) : assetManager(manager), dataConnector(connector) {
}

// ����� �������� ��� ������� 
void DataManager::choosePortfolio()
{
    allPortfolios = assetManager.getPortfolios();

    assetManager.displayPortfolios(allPortfolios);

    std::cout << "�������� �������� ��� �������: ";
    int portfolioId;
    std::cin >> portfolioId;

    allStocks = assetManager.getStocks(portfolioId);

    std::cout << "������ ����� � ��������� ��������:\n";
    assetManager.displayStocks(allStocks);

    for (const auto& stock : allStocks) {
        // �������� ������������ ������ ������ ���� ���
        std::vector<StockData> stockData = dataConnector.fetchHistoricalData(stock.ticker, "1d", "1y");
        if (stockData.empty()) continue; // �������, ���� ������ ���

        // ��������� ������� ���� ��� ��������� � ������
        double currentPrice = stockData.back().closeprice;
        currentPrices[stock.ticker] = currentPrice;

        // ��������� ������������ ����
        std::vector<double> closePrices;
        closePrices.reserve(stockData.size());
        for (const auto& data : stockData) {
            closePrices.push_back(data.closeprice);
        }
        historicalPrices[stock.ticker] = closePrices;
    }
}

// ��������� ���������
const std::vector<Portfolio>& DataManager::getAllPortfolios()
{
	return allPortfolios;
}

// ��������� �����
const std::vector<Stock>& DataManager::getAllStocks()
{
	return allStocks;
}

// ��������� ������� ���
const std::map<std::string, double>& DataManager::getCurrentPrices()
{
	return currentPrices;
}

// ��������� ������������ ��� ������� ��������
const std::map<std::string, std::vector<double>>& DataManager::getHistoricalPrices()
{
	return historicalPrices;
}

