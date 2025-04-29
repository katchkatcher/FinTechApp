#include "RiskModeler.h"

// Конструктор
RiskModeler::RiskModeler(AssetManager& manager, PortfolioAnalyzer& analyzer, DataConnector& connector, DataManager& dmanager) :
	assetManager(manager),
	portfolioAnalyzer(analyzer),
	dataConnector(connector),
	dataManager(dmanager),
	allPortfolios(dmanager.getAllPortfolios()),
	currentPrices(dmanager.getCurrentPrices()),
	allStocks(dmanager.getAllStocks()),
	historicalPrices(dmanager.getHistoricalPrices())
{
	numAssets = allStocks.size();
}

// Функция-нициализатор матрицы
void RiskModeler::initialize()
{
	numAssets = allStocks.size();
	covMatrix = std::vector<std::vector<double>>(numAssets, std::vector<double>(numAssets, 0.0));
	calculateLogReturns();
	calculateMeanReturns();
	calculateCovMatrix();
}

// Подсчёт ковариационной матрицы
void RiskModeler::calculateCovMatrix()
{
	for (size_t i = 0; i < numAssets; i++) {
		for (size_t j = 0; j < numAssets; j++) {
			const std::string& tickerI = allStocks[i].ticker;
			const std::string& tickerJ = allStocks[j].ticker;

			const std::vector<double>& returnsI = logReturns[tickerI];
			const std::vector<double>& returnsJ = logReturns[tickerJ];

			size_t n = returnsI.size();
			if (n == 0 || n != returnsJ.size()) continue;

			double meanI = meanReturns[tickerI];
			double meanJ = meanReturns[tickerJ];

			double sum = 0.0;
			for (size_t k = 0; k < n; k++) {
				sum += (returnsI[k] - meanI) * (returnsJ[k] - meanJ);
			}
			sum /= (n - 1);
			covMatrix[i][j] = sum;
		}
	}
}

// Подсчёт логарифмической доходности
void RiskModeler::calculateLogReturns()
{
	for (auto it = historicalPrices.begin(); it != historicalPrices.end(); ++it) {
		const std::string& ticker = it->first;
		const std::vector<double>& prices = it->second;

		std::vector<double> returns;
		returns.reserve(prices.size() - 1);
		for (size_t i = 1; i < prices.size(); i++) {
			returns.push_back(std::log(prices[i] / prices[i - 1]));
		}
		logReturns[ticker] = returns;
	}
}

// Подсчёт долей активов
void RiskModeler::calculateMeanReturns()
{
	for (const auto& stock : allStocks) {
		meanReturns[stock.ticker] = portfolioAnalyzer.calculateAssetReturn(stock.ticker);
	}
}

// Подсчёт будущей цены портфеля
double RiskModeler::calculateFuturePrice( std::string assetName)
{
	double volatility = portfolioAnalyzer.calculateAssetVolatility(assetName);
	double currentPrice = currentPrices.at(assetName);
	int daysToForecast{ 252 };
	double meanReturn = portfolioAnalyzer.calculateAssetReturn(assetName);

	std::vector<double> futurePrices;

	futurePrices.push_back(currentPrice);

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0.0, 1.0);

	for (int i = 0; i < daysToForecast; i++) {
		double Z = distribution(generator);
		double newPrice = futurePrices.back() * exp((meanReturn - 0.5 * volatility * volatility) / 252
			+ (volatility * Z / sqrt(252)));
		futurePrices.push_back(newPrice);
	}

	double sum = std::accumulate(futurePrices.begin(), futurePrices.end(), 0.0);
	return sum / futurePrices.size();
}

// Подсчёт риска портфеля
double RiskModeler::calculatePortfolioRisk()
{
	if (!initializer) {
		initialize();
		initializer = true;
	}

	std::vector<double> weights;
	double totalValue = portfolioAnalyzer.calculateTotalValue();
	for (const auto& stock : allStocks) {
		double weight = portfolioAnalyzer.calculateAssetShare(stock.ticker);
		weights.push_back(weight);
	}

	double portfolioVariance = 0.0;
	for (size_t i = 0; i < numAssets; i++) {
		for (size_t j = 0; j < numAssets; j++) {
			portfolioVariance += weights[i] * covMatrix[i][j] * weights[j];
		}
	}
	double portfolioRisk = std::sqrt(portfolioVariance);

	return portfolioRisk;
}

//Подсчёт будущей доходности портфеля
double RiskModeler::calculateFuturePortfolioReturn()
{
	// Параметры моделирования
	int daysToForecast = 252;
	int numSimulations = 10000;

	// Рассчитываем веса активов в портфеле
	std::vector<double> weights;
	for (const auto& stock : allStocks) {
		double weight = portfolioAnalyzer.calculateAssetShare(stock.ticker);
		weights.push_back(weight);
	}
	double totalPortfolioValue = portfolioAnalyzer.calculateTotalValue();

	// Моделируем доходность портфеля методом Монте-Карло
	std::vector<double> simulatedReturns;
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0.0, 1.0);

	for (int sim = 0; sim < numSimulations; sim++) {
		std::vector<double> simulatedPrices(numAssets, 0.0);

		for (size_t i = 0; i < numAssets; i++) {
			double futurePrice = currentPrices.at(allStocks[i].ticker);

			for (int d = 0; d < daysToForecast; d++) {
				double randomShock = distribution(generator);
				double drift = meanReturns.at(allStocks[i].ticker) / 252 - 0.5 * covMatrix[i][i];
				double diffusion = std::sqrt(covMatrix[i][i]) * randomShock;
				futurePrice *= std::exp(drift + diffusion);
			}
			simulatedPrices[i] = futurePrice;
		}

		// Рассчитываем стоимость портфеля в конце периода
		double futurePortfolioValue = 0.0;
		for (size_t i = 0; i < numAssets; i++) {
			futurePortfolioValue += simulatedPrices[i] * weights[i];
		}

		// Вычисляем доходность портфеля
		double portfolioReturn = (futurePortfolioValue - totalPortfolioValue) / totalPortfolioValue;
		simulatedReturns.push_back(portfolioReturn);
	}

	// Средняя доходность портфеля
	double avgReturn = std::accumulate(simulatedReturns.begin(), simulatedReturns.end(), 0.0) / simulatedReturns.size();
	return avgReturn;

}

// VaR методом Монте-Карло
double RiskModeler::calculateVaR(double confidenceLevel, int daysToForecast, int numSimulations)
{
	std::vector<double> simulatedReturns;

	std::random_device rd;
	std::mt19937 generator(rd());
	std::normal_distribution<> distribution(0.0, 1.0);

	double totalPortfolioValue = portfolioAnalyzer.calculateTotalValue();

	// Получаем веса активов
	std::vector<double> weights;
	for (const auto& stock : allStocks) {
		double weight = portfolioAnalyzer.calculateAssetShare(stock.ticker);
		weights.push_back(weight);
	}

	// Запуск Монте-Карло симуляций
	for (int sim = 0; sim < numSimulations; sim++) {
		std::vector<double> simulatedPrices(numAssets, 0.0);

		for (size_t i = 0; i < numAssets; i++) {
			double futurePrice = currentPrices.at(allStocks[i].ticker);

			for (int d = 0; d < daysToForecast; d++) {
				double randomShock = distribution(generator);
				double drift = (meanReturns.at(allStocks[i].ticker) / 252) * daysToForecast - 0.5 * covMatrix[i][i] * daysToForecast;
				double diffusion = std::sqrt(covMatrix[i][i] * daysToForecast) * randomShock;
				futurePrice *= std::exp(drift + diffusion);
			}
			simulatedPrices[i] = futurePrice;
		}

		// Рассчитываем стоимость портфеля в конце периода
		double futurePortfolioValue = 0.0;
		for (size_t i = 0; i < numAssets; i++) {
			futurePortfolioValue += simulatedPrices[i] * weights[i];
		}

		// Вычисляем доходность портфеля
		double portfolioReturn = (futurePortfolioValue - totalPortfolioValue) / totalPortfolioValue;

		simulatedReturns.push_back(portfolioReturn);
	}

	// Сортируем результаты симуляций
	std::sort(simulatedReturns.begin(), simulatedReturns.end());

	// Определяем индекс для доверительного уровня
	size_t varIndex = static_cast<size_t>((1.0 - confidenceLevel) * numSimulations);

	// Получаем VaR как отрицательную величину (потери)
	double VaR = -simulatedReturns[varIndex] * totalPortfolioValue;
	return VaR;
}

