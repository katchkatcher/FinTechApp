#include "DataManager.h"

DataManager::DataManager(AssetManager& manager, DataConnector& connector) : assetManager(manager), dataConnector(connector) {
}

// Выбор портфеля для анализа 
void DataManager::choosePortfolio()
{
    allPortfolios = assetManager.getPortfolios();

    assetManager.displayPortfolios(allPortfolios);

    std::cout << "Выберите портфель для анализа: ";
    int portfolioId;
    std::cin >> portfolioId;

    allStocks = assetManager.getStocks(portfolioId);

    std::cout << "Список акций в выбранном портфеле:\n";
    assetManager.displayStocks(allStocks);

    for (const auto& stock : allStocks) {
        // Получаем исторические данные только один раз
        std::vector<StockData> stockData = dataConnector.fetchHistoricalData(stock.ticker, "1d", "1y");
        if (stockData.empty()) continue; // Пропуск, если данных нет

        // Сохраняем текущую цену как последнюю в списке
        double currentPrice = stockData.back().closeprice;
        currentPrices[stock.ticker] = currentPrice;

        // Сохраняем исторические цены
        std::vector<double> closePrices;
        closePrices.reserve(stockData.size());
        for (const auto& data : stockData) {
            closePrices.push_back(data.closeprice);
        }
        historicalPrices[stock.ticker] = closePrices;
    }
}

// Получение портфелей
const std::vector<Portfolio>& DataManager::getAllPortfolios()
{
	return allPortfolios;
}

// Получение акций
const std::vector<Stock>& DataManager::getAllStocks()
{
	return allStocks;
}

// Получение текущих цен
const std::map<std::string, double>& DataManager::getCurrentPrices()
{
	return currentPrices;
}

// Получение исторических цен активов портфеля
const std::map<std::string, std::vector<double>>& DataManager::getHistoricalPrices()
{
	return historicalPrices;
}

