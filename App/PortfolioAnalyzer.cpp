
#include "PortfolioAnalyzer.h"
#include <iostream>
#include <vector>

// Конструктор
PortfolioAnalyzer::PortfolioAnalyzer( DataConnector& connector, DataManager& dmanager) :  
	dataConnector(connector), 
	dataManager(dmanager),
	allPortfolios(dmanager.getAllPortfolios()),
	allStocks(dmanager.getAllStocks()),
	currentPrices(dmanager.getCurrentPrices()){

}

// Подсчёт общей стоимости портфеля
double PortfolioAnalyzer::calculateTotalValue()
{
	double totalValue = 0.0;
	int i = 0;

	for (const auto& stock : allStocks) {
		auto it = currentPrices.find(stock.ticker);
		if (it != currentPrices.end()) {
			totalValue += it->second * stock.quantity;
		}
	}
	
	return totalValue;
} 

// Подсчёт доли портфеля
double PortfolioAnalyzer::calculateAssetShare(const std::string& assetName)
{

	double totalValue = calculateTotalValue();

	if (totalValue == 0.0)	return 0.0;

	for (const auto& stock : allStocks) {

		if (stock.ticker == assetName) {
			auto it = currentPrices.find(assetName);
			double stockValue = it->second * stock.quantity;
			return (stockValue / totalValue);
		}
	}

	return 0.0;
}

// Подсчёт доходности портфеля
double PortfolioAnalyzer::calculatePortfolioReturn()
{
	double currentPortfolioValue = calculateTotalValue();
	if (currentPortfolioValue == 0.0) return 0.0;

	double portfolioReturnValue = 0.0;

	auto it = currentPrices.begin();
	for (const auto& stock : allStocks) {
		double stockReturn = ((it->second - stock.purchasePrice) / stock.purchasePrice) * 100.0;
		double stockShare = calculateAssetShare(stock.ticker);
		portfolioReturnValue += stockReturn * stockShare;
	}

	return portfolioReturnValue;
}

// Волатильность акции в портфеле
double PortfolioAnalyzer::calculateAssetVolatility(const std::string& assetName)
{
	std::vector<StockData> prices;

	for (const auto& stock : allStocks) {
		if (stock.ticker == assetName) {
			prices = dataConnector.fetchHistoricalData(stock.ticker, "1d", "1y");
		}
	}

	std::vector<double> logReturns;
	for (auto i = 1; i < prices.size(); i++) {
		logReturns.push_back(std::log(prices[i].closeprice / prices[i - 1].closeprice));
	}

	double meanReturn = std::accumulate(logReturns.begin(), logReturns.end(), 0.0) / logReturns.size();

	double variance = 0.0;
	for (double r : logReturns) {
		variance += (r - meanReturn) * (r - meanReturn);
	}
	variance /= (logReturns.size() - 1);

	double standartDeviation = std::sqrt(variance);

	return  standartDeviation * std::sqrt(252);
}

// Волатильность акции в портфеле
double PortfolioAnalyzer::calculatePortfolioVolatility()
{
	double totalValue = calculateTotalValue();
	double weightedVariance = 0.0;

	std::vector<std::vector<double>> covarianceMatrix(allStocks.size(), std::vector<double>(allStocks.size(), 0.0));

	for (size_t i = 0; i < allStocks.size(); ++i) {
		for (size_t j = i; j < allStocks.size(); ++j) {
			double cov = calculateCovariance(allStocks[i].ticker, allStocks[j].ticker);
			covarianceMatrix[i][j] = cov;
			covarianceMatrix[j][i] = cov;
		}
	}

	for (size_t i = 0; i < allStocks.size(); ++i) {
		double weight_i = calculateAssetShare(allStocks[i].ticker); // Доля актива в портфеле
		for (size_t j = 0; j < allStocks.size(); ++j) {
			double weight_j = calculateAssetShare(allStocks[j].ticker);
			weightedVariance += weight_i * weight_j * covarianceMatrix[i][j];
		}
	}

	return std::sqrt(weightedVariance);
}

// Доходность актива
double PortfolioAnalyzer::calculateAssetReturn(const std::string& assetName)
{
	std::vector<StockData> prices;

	for (const auto& stock : allStocks) {
		if (stock.ticker == assetName) {
			prices = dataConnector.fetchHistoricalData(assetName, "1d", "1y");
		}
	}

	std::vector<double> logReturns;
	for (auto i = 1; i < prices.size(); i++) {
		logReturns.push_back(std::log(prices[i].closeprice / prices[i - 1].closeprice));
	}

	double meanReturn = std::accumulate(logReturns.begin(), logReturns.end(), 0.0) / logReturns.size();

	double annualReturn = meanReturn * 252;

	return annualReturn;
}

//!!!BUG!!!
// Ковариационная матрица
double PortfolioAnalyzer::calculateCovariance(const std::string& asset1, const std::string& asset2)
{
	auto prices1 = dataConnector.fetchHistoricalData(asset1, "1d", "1y");
	auto prices2 = dataConnector.fetchHistoricalData(asset2, "1d", "1y");

	if (prices1.size() != prices2.size() || prices1.empty()) {
		std::cerr << "Размеры исторических данных для акций " << asset1 << " и " << asset2 << " не совпадают." << std::endl;
		return 0.0;
	}

	std::vector<double> returns1, returns2;
	for (size_t i = 1; i < prices1.size(); ++i) {
		returns1.push_back(std::log(prices1[i].closeprice / prices1[i - 1].closeprice));
		returns2.push_back(std::log(prices2[i].closeprice / prices2[i - 1].closeprice));
	}

	double mean1 = std::accumulate(returns1.begin(), returns1.end(), 0.0) / returns1.size();
	double mean2 = std::accumulate(returns2.begin(), returns2.end(), 0.0) / returns2.size();

	double covariance = 0.0;
	for (size_t i = 0; i < returns1.size(); ++i) {
		covariance += (returns1[i] - mean1) * (returns2[i] - mean2);
	}

	return covariance / (returns1.size() - 1);
}



