#include "AssetManager.h"
#include "DataConnector.h"
#include "PortfolioAnalyzer.h"
#include "DataManager.h"
#include "RiskModeler.h"
#include <iostream>

// ����������� ��������� ����
void displayMenu() {

	std::cout << "**************************************************************\n";
	std::cout << "*               ��������� ���������� ���������               *\n";
	std::cout << "**************************************************************\n";
	std::cout << "*  1. ������� �������� � �������� �����                      *\n";
	std::cout << "*  2. ����� �������� ��� ������� � ������������� ������      *\n";
	std::cout << "*  3. ������ ��������                                        *\n";
	std::cout << "*  4. ������������� ������ � ���������                       *\n";
	std::cout << "*  0. �����                                                  *\n";
	std::cout << "**************************************************************\n";
	std::cout << "������� ����� �������:  ";

}

// ����������� ���� AssetManager
void displayAssetManagerMenu() {

	std::cout << "------------ �������� �������� -----------\n";
	std::cout << "  1. ������� ���� ������\n";
	std::cout << "  2. ������� ������� ������� � ���������\n";
	std::cout << "  3. �������� ��������.\n";
	std::cout << "  4. �������� ����� � ��������.\n";
	std::cout << "  5. ������� �����.\n";
	std::cout << "  6. ���������� ������������ ��������.\n";
	std::cout << "  7. ���������� ����� � ��������.\n";
	std::cout << "  8. ������� ���� ������\n";
	std::cout << "  0. ����� �� ����.\n";
	std::cout << "------------------------------------------\n";
	std::cout << "��� �����: ";

}

void displayDataManagerMenu() {

	std::cout << "------------ ����� �������� ��� ������� -----------\n";
	std::cout << "  1. ������ ��������.\n";
	std::cout << "  0. ����� �� ����.\n";
	std::cout << "------------------------------------------\n";
	std::cout << "��� �����: ";

}

void displayPrtfolioAnalyzerMenu() {

	std::cout << "------------ ������ �������� -----------\n";
	std::cout << "  1. ������� ����� ��������� ��������\n";
	std::cout << "  2. ������� ���� ������ � ��������\n";
	std::cout << "  3. ������� ���������� ��������\n";
	std::cout << "  4. ������� ������������� ������\n";
	std::cout << "  5. ������� ���������� ������������� ��������\n";
	std::cout << "  6. ������� ���������� ������\n";
	std::cout << "  0. ����� �� ����.\n";
	std::cout << "------------------------------------------\n";
	std::cout << "��� �����: ";

}

// ����������� ���� RiskModeler
void displayRiskModelerMenu() {

	std::cout << "------------ ������������� ������ -----------\n";
	std::cout << "  1. ������� ������� ���� �����\n";
	std::cout << "  2. ������� ����� ��������\n";
	std::cout << "  3. ������� ������� ���������� �������� ��������\n";
	std::cout << "  4. ������� VaR ��������\n";
	std::cout << "  0. ����� �� ����.\n";
	std::cout << "------------------------------------------\n";
	std::cout << "��� �����: ";

}

// ������� ���������� �����
bool addAsset(AssetManager& manager, DataConnector& connector) {

	std::cout << "������� id ��������, � ������� ������ "
		<< "�������� �����(���� ���������� ������ ���� ��������, ������� 1): ";

	int portfolioId;
	std::cin >> portfolioId;

	std::cout << "������� �������� �����, ������� ������ �������� � ��������: ";

	std::string ticker;
	std::cin >> ticker;

	auto data = connector.fetchHistoricalData(ticker, "1d", "2d");
	double purchasePrise = data[data.size() - 1].closeprice;

	std::cout << "������� ����������� ����� ��� ����������: ";
	int quantity;
	std::cin >> quantity;

	manager.addStock(portfolioId, ticker, purchasePrise, quantity);

	return true;

}

// �������� ��������
int assetManager(AssetManager& manager, DataConnector& connector) {
	while (true) {
		int choiseAssetManagerFunc;
		displayAssetManagerMenu();

		std::cin >> choiseAssetManagerFunc;
		switch (choiseAssetManagerFunc) {
		case 1:
		{
			if (manager.createDatabase()) {
				std::cout << "���� ������ ������� �������.\n";
			}
			break;
		}
		case 2:
		{
			if (manager.createPortfolioTable() && manager.createStockTable()) {
				std::cout << "������� ������� �������.\n";
			}
			break;
		}
		case 3:
		{
			std::cout << "������� ��� ��������: ";

			std::string portfolioName;
			std::cin >> portfolioName;

			if (manager.addPortfolio(portfolioName)) {
				std::cout << "�������� " << portfolioName << " ������� ��������\n";
			}
			break;
		}
		case 4:
		{
			if (addAsset(manager, connector)) {
				std::cout << "����� ������� ���������!\n";
			}
			break;
		}
		case 5:
		{
			std::string assetName;
			std::cout << "������� ����� ������ ��� ��������: ";
			std::cin >> assetName;

			int portfolioId;
			std::cout << "������� Id �������� ��� �������� ����� �� ����: ";
			std::cin >> portfolioId;


			if (manager.deleteAssetByName(portfolioId, assetName)) {
				std::cout << "�������� ������ �������\n";
			}
			else {
				std::cout << "������� �� �������\n";
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
			std::cout << "������� id ��������: ";

			int id;
			std::cin >> id;

			auto stocks = manager.getStocks(id);
			manager.displayStocks(stocks);

			break;
		}
		case 8:
		{
			std::cout << "�������� ���� ������.\n";
			manager.deleteDatabase();
			break;
		}
		case 0:
		{
			std::cout << "����� �� ���������.\n";
			return 0;
		}
		default:
		{
			std::cout << "�� ����� ������������ ��������, ���������� �����.\n";
			break;
		}
		}
	}
		
	return 0;
}

// ����� �������� ��� �������
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
			std::cout << "����� �� ���������.\n";
			return 0;
		}
		default:
		{
			std::cout << "�� ����� ������������ ��������, ���������� �����.\n";
			break;
		}
		}
	}
	return 0;
}

// ������ ��������
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
			std::cout << "������� ��������� ���������� ��������: " << portfolioPrice << "$" << "\n";
			break;
		}
		case 2:
		{
			std::cout << "������� ��� �����, ��� ������� �� ������ ���������� ����: ";
			
			std::string stockName;
			std::cin >> stockName;

			double assetShare = analyzer.calculateAssetShare(stockName);

			std::cout << "���� ����� � ��������: " << assetShare * 100 << "%\n";
			break;
		}
		case 3:
		{
			std::cout << "���������� ���������� ��������: ";
			double portfolioReturn = analyzer.calculatePortfolioReturn();
			std::cout << portfolioReturn << "%\n";
			break;
		}
		case 4:
		{
			std::string stockName;
			std::cout << "������� ��� �����, ��� ������� ����� ���������� �������������: ";
			std::cin >> stockName;

			double variance = analyzer.calculateAssetVolatility(stockName) * 100;
			std::cout << "������������� �������� �����: " << variance << "%\n";
			break;
		}
		case 5:
		{
			double portfolioVolatility;

			portfolioVolatility = analyzer.calculatePortfolioVolatility();

			std::cout << "���������� ������������� ������ ��������: " << portfolioVolatility * 100 << "%\n";
			break;
		}
		case 6:
		{
			std::string stockName;
			std::cout << "������� ��� �����, ��� ������� ����� ���������� ����������: ";
			std::cin >> stockName;

			double assetMeanReturn;
			assetMeanReturn = analyzer.calculateAssetReturn(stockName);
			std::cout << "���������� �������� �����: " << assetMeanReturn * 100 << "%\n";
			break;
		}
		case 0: 
		{
			std::cout << "����� �� ���������.\n";
			return 0;
		}
		default:
		{
			std::cout << "�� ����� ������������ ��������, ���������� �����.\n";
			break;
		}
		}
	}
	return 0;
}

// ������������� ������
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
			std::cout << "������� ����� �����, ��� ������� ����� ��������������� ������� ����: ";
			std::cin >> stockName;

			double futurePrice = modeler.calculateFuturePrice( stockName);
			std::cout << "��������������� ������� ���� ������: " << futurePrice << "$\n";
			break;
		}
		case 2:
		{
			double portfolioRisk;
			portfolioRisk = modeler.calculatePortfolioRisk();
			std::cout << "���� ���������� ��������: " << portfolioRisk * 100 << "%\n";
			break;
		}
		case 3:
		{
			double futurePortfolioReturn;
			futurePortfolioReturn = modeler.calculateFuturePortfolioReturn();
			std::cout << "��������������� ������� ���������� ������ ��������: " << futurePortfolioReturn << "%\n";
			break;
		}
		case 4:
		{
			double trustLevel;
			std::cout << "������� ������� �������(�������� 95): ";
			std::cin >> trustLevel;

			int daysToForecast;
			std::cout << "������� ���-�� ���� �� ������� ������ ��������������� ������: ";
			std::cin >> daysToForecast;

			int numSmulation;
			std::cout << "������� ���-�� ���������(�������� 10000): ";
			std::cin >> numSmulation;

			double totalValue = analyzer.calculateTotalValue();

			std::cout << "������� ����� ��������� ��������: " << totalValue<< "\n";
			double valueAtRisk = modeler.calculateVaR(trustLevel / 100, daysToForecast, numSmulation);
			std::cout << "�������� VaR(�����, ������� ��������, ��� � " << trustLevel << "% �������, ������ �� �������� ��� �����): "
				<< valueAtRisk << "\n";
			break;
		}
		case 0:
		{
			std::cout << "����� �� ���������.\n";
			return 0;
		}
		default:
		{
			std::cout << "�� ����� ������������ ��������, ���������� �����.\n";
			break;
		}
		}
	}
	return 0;
}

int main() {
	setlocale(LC_ALL, "rus");
	// �������� �������� ��������� ����������� ��������
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
			std::cout << "����� �� ���������.\n";
			return 0;
		}
		default:
		{
			std::cout << "�� ����� ������������ ��������, ���������� �����.\n";
			break;
		}
		}
	}

	return 0;
}
