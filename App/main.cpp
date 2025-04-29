#include "AssetManager.h"
#include "DataConnector.h"
#include "PortfolioAnalyzer.h"
#include "DataManager.h"
#include "RiskModeler.h"
#include <iostream>

// Отображение основного меню
void displayMenu() {

	std::cout << "**************************************************************\n";
	std::cout << "*               Программа управления портфелем               *\n";
	std::cout << "**************************************************************\n";
	std::cout << "*  1. Создать портфель и добавить акции                      *\n";
	std::cout << "*  2. Выбор портфеля для анализа и моделирования рисков      *\n";
	std::cout << "*  3. Анализ портфеля                                        *\n";
	std::cout << "*  4. Моделирование рисков и прогнозов                       *\n";
	std::cout << "*  0. Выход                                                  *\n";
	std::cout << "**************************************************************\n";
	std::cout << "Введите номер команды:  ";

}

// Отображение меню AssetManager
void displayAssetManagerMenu() {

	std::cout << "------------ Создание портфеля -----------\n";
	std::cout << "  1. Создать базу данных\n";
	std::cout << "  2. Создать таблицы активов и портфелей\n";
	std::cout << "  3. Добавить портфель.\n";
	std::cout << "  4. Добавить акцию в портфель.\n";
	std::cout << "  5. Удалить акцию.\n";
	std::cout << "  6. Отобразить существующие портфели.\n";
	std::cout << "  7. Отобразить акции в портфеле.\n";
	std::cout << "  8. Удалить базу данных\n";
	std::cout << "  0. Выход из меню.\n";
	std::cout << "------------------------------------------\n";
	std::cout << "Ваш выбор: ";

}

void displayDataManagerMenu() {

	std::cout << "------------ Выбор портфеля для анализа -----------\n";
	std::cout << "  1. Вырать портфель.\n";
	std::cout << "  0. Выход из меню.\n";
	std::cout << "------------------------------------------\n";
	std::cout << "Ваш выбор: ";

}

void displayPrtfolioAnalyzerMenu() {

	std::cout << "------------ Анализ портфеля -----------\n";
	std::cout << "  1. Подсчёт общей стоимости портфеля\n";
	std::cout << "  2. Подсчёт доли актива в портфеле\n";
	std::cout << "  3. Подсчёт доходности портфеля\n";
	std::cout << "  4. Подсчёт волатильности актива\n";
	std::cout << "  5. Подсчёт взвешенной волатильности портфеля\n";
	std::cout << "  6. Подсчёт доходности актива\n";
	std::cout << "  0. Выход из меню.\n";
	std::cout << "------------------------------------------\n";
	std::cout << "Ваш выбор: ";

}

// Отображение меню RiskModeler
void displayRiskModelerMenu() {

	std::cout << "------------ Моделирование Рисков -----------\n";
	std::cout << "  1. Подсчёт будущей цены акции\n";
	std::cout << "  2. Подсчёт риска портфеля\n";
	std::cout << "  3. Подсчёт будущей доходности портфеля портфеля\n";
	std::cout << "  4. Подсчёт VaR портфеля\n";
	std::cout << "  0. Выход из меню.\n";
	std::cout << "------------------------------------------\n";
	std::cout << "Ваш выбор: ";

}

// Функция добавления акции
bool addAsset(AssetManager& manager, DataConnector& connector) {

	std::cout << "Введите id портфеля, в который хотите "
		<< "добавить акцию(если существует только один портфель, введите 1): ";

	int portfolioId;
	std::cin >> portfolioId;

	std::cout << "Введите название акции, которую хотите добавить в портфель: ";

	std::string ticker;
	std::cin >> ticker;

	auto data = connector.fetchHistoricalData(ticker, "1d", "2d");
	double purchasePrise = data[data.size() - 1].closeprice;

	std::cout << "Введите колличество акций для добавления: ";
	int quantity;
	std::cin >> quantity;

	manager.addStock(portfolioId, ticker, purchasePrise, quantity);

	return true;

}

// Создание портфеля
int assetManager(AssetManager& manager, DataConnector& connector) {
	while (true) {
		int choiseAssetManagerFunc;
		displayAssetManagerMenu();

		std::cin >> choiseAssetManagerFunc;
		switch (choiseAssetManagerFunc) {
		case 1:
		{
			if (manager.createDatabase()) {
				std::cout << "База данных успешно создана.\n";
			}
			break;
		}
		case 2:
		{
			if (manager.createPortfolioTable() && manager.createStockTable()) {
				std::cout << "Таблица успешно создана.\n";
			}
			break;
		}
		case 3:
		{
			std::cout << "Введите имя портфеля: ";

			std::string portfolioName;
			std::cin >> portfolioName;

			if (manager.addPortfolio(portfolioName)) {
				std::cout << "Портфель " << portfolioName << " успешно добавлен\n";
			}
			break;
		}
		case 4:
		{
			if (addAsset(manager, connector)) {
				std::cout << "Акция успешно добавлена!\n";
			}
			break;
		}
		case 5:
		{
			std::string assetName;
			std::cout << "Введите тикер актива для удаления: ";
			std::cin >> assetName;

			int portfolioId;
			std::cout << "Введите Id портфеля для удаления акции из него: ";
			std::cin >> portfolioId;


			if (manager.deleteAssetByName(portfolioId, assetName)) {
				std::cout << "Удаление прошло успешно\n";
			}
			else {
				std::cout << "Удалить не удалось\n";
			}
			manager.reindexStocks();
			break;
		}
		case 6:
		{
			auto portfolios = manager.getPortfolios();
			manager.displayPortfolios(portfolios);
			break;
		}
		case 7:
		{
			std::cout << "Введите id портфеля: ";

			int id;
			std::cin >> id;

			auto stocks = manager.getStocks(id);
			manager.displayStocks(stocks);

			break;
		}
		case 8:
		{
			std::cout << "Удаление базы данных.\n";
			manager.deleteDatabase();
			break;
		}
		case 0:
		{
			std::cout << "Выход из программы.\n";
			return 0;
		}
		default:
		{
			std::cout << "Вы ввели некорректное значение, попробуйте снова.\n";
			break;
		}
		}
	}
		
	return 0;
}

// Выбор портфеля для анализа
int dataManager(DataManager& dmanager) {
	while (true) {
		int choosePortfolio;
		displayDataManagerMenu();

		std::cin >> choosePortfolio;
		switch (choosePortfolio) {
		case 1:
		{
			dmanager.choosePortfolio();
			break;
		}
		case 0:
		{
			std::cout << "Выход из программы.\n";
			return 0;
		}
		default:
		{
			std::cout << "Вы ввели некорректное значение, попробуйте снова.\n";
			break;
		}
		}
	}
	return 0;
}

// Анализ портфеля
int portfolioAnalyzer(DataConnector& connector, PortfolioAnalyzer& analyzer) {
	while (true) {
		int choisePortfolioAnalyzerFunction;
		displayPrtfolioAnalyzerMenu();

		std::cin >> choisePortfolioAnalyzerFunction;
		switch (choisePortfolioAnalyzerFunction)
		{

		case 1:
		{
			const auto portfolioPrice = analyzer.calculateTotalValue();
			std::cout << "Текущая стоимость выбранного портфеля: " << portfolioPrice << "$" << "\n";
			break;
		}
		case 2:
		{
			std::cout << "Введите имя акции, для которой вы хотите подсчитать долю: ";
			
			std::string stockName;
			std::cin >> stockName;

			double assetShare = analyzer.calculateAssetShare(stockName);

			std::cout << "Доля акции в портфеле: " << assetShare * 100 << "%\n";
			break;
		}
		case 3:
		{
			std::cout << "Доходность выбранного портфеля: ";
			double portfolioReturn = analyzer.calculatePortfolioReturn();
			std::cout << portfolioReturn << "%\n";
			break;
		}
		case 4:
		{
			std::string stockName;
			std::cout << "Введите имя акции, для которой нужно подсчитать волатильность: ";
			std::cin >> stockName;

			double variance = analyzer.calculateAssetVolatility(stockName) * 100;
			std::cout << "Волатильность заданной акции: " << variance << "%\n";
			break;
		}
		case 5:
		{
			double portfolioVolatility;

			portfolioVolatility = analyzer.calculatePortfolioVolatility();

			std::cout << "Взвешенная волатильность вашего портфеля: " << portfolioVolatility * 100 << "%\n";
			break;
		}
		case 6:
		{
			std::string stockName;
			std::cout << "Введите имя акции, для которой нужно подсчитать доходность: ";
			std::cin >> stockName;

			double assetMeanReturn;
			assetMeanReturn = analyzer.calculateAssetReturn(stockName);
			std::cout << "Доходность заданной акции: " << assetMeanReturn * 100 << "%\n";
			break;
		}
		case 0: 
		{
			std::cout << "Выход из программы.\n";
			return 0;
		}
		default:
		{
			std::cout << "Вы ввели некорректное значение, попробуйте снова.\n";
			break;
		}
		}
	}
	return 0;
}

// Моделирование Рисков
int riskModeler( RiskModeler& modeler, PortfolioAnalyzer&analyzer)
{
	while (true) {
		int chooseRiskModelerFunction;
		displayRiskModelerMenu();
		
		std::cin >> chooseRiskModelerFunction;
		switch (chooseRiskModelerFunction)
		{

		case 1:
		{
			std::string stockName;
			std::cout << "Введите тикер акции, для которой нужно спрогнозировать будущую цену: ";
			std::cin >> stockName;

			double futurePrice = modeler.calculateFuturePrice( stockName);
			std::cout << "Приблизительная будущая цена актива: " << futurePrice << "$\n";
			break;
		}
		case 2:
		{
			double portfolioRisk;
			portfolioRisk = modeler.calculatePortfolioRisk();
			std::cout << "Риск выбранного портфеля: " << portfolioRisk * 100 << "%\n";
			break;
		}
		case 3:
		{
			double futurePortfolioReturn;
			futurePortfolioReturn = modeler.calculateFuturePortfolioReturn();
			std::cout << "Приблизительная будущая доходность ващего портфеля: " << futurePortfolioReturn << "%\n";
			break;
		}
		case 4:
		{
			double trustLevel;
			std::cout << "Введите уровень доверия(например 95): ";
			std::cin >> trustLevel;

			int daysToForecast;
			std::cout << "Введите кол-во дней на которое хотите спрогнозировать потери: ";
			std::cin >> daysToForecast;

			int numSmulation;
			std::cout << "Введите кол-во симуляций(например 10000): ";
			std::cin >> numSmulation;

			double totalValue = analyzer.calculateTotalValue();

			std::cout << "Текущая общая стоимость портфеля: " << totalValue<< "\n";
			double valueAtRisk = modeler.calculateVaR(trustLevel / 100, daysToForecast, numSmulation);
			std::cout << "Значение VaR(сумма, которая отражает, что в " << trustLevel << "% случаях, потери не превысят эту сумму): "
				<< valueAtRisk << "\n";
			break;
		}
		case 0:
		{
			std::cout << "Выход из программы.\n";
			return 0;
		}
		default:
		{
			std::cout << "Вы ввели некорректное значение, попробуйте снова.\n";
			break;
		}
		}
	}
	return 0;
}

int main() {
	setlocale(LC_ALL, "rus");
	// создание объектов основного функционала портфеля
	AssetManager manager("portfolio.db");
	DataConnector connector("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.0.0 Safari/537.36");
	DataManager dmanager(manager, connector);
	PortfolioAnalyzer analyzer( connector, dmanager);
	RiskModeler modeler(manager, analyzer, connector, dmanager);

	while (true) {
		int choise;
		displayMenu();

		std::cin >> choise;
		switch (choise) {
		case 1:
		{
			assetManager(manager, connector);
			break;
		}
		case 2:
		{
			dataManager(dmanager);
			break;
		}
		case 3:
		{
			portfolioAnalyzer( connector, analyzer);
			break;
		}
		case 4:
		{
			riskModeler(modeler, analyzer);
			break;
		}
		case 0:
		{
			std::cout << "Выход из программы.\n";
			return 0;
		}
		default:
		{
			std::cout << "Вы ввели некорректное значение, попробуйте снова.\n";
			break;
		}
		}
	}

	return 0;
}
