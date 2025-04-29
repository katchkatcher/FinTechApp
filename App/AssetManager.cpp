#include "AssetManager.h"
#include <iostream>
#include <sqlite3.h>

// Конструктор
AssetManager::AssetManager(const std::string& databasePath) : dbPath(databasePath){}

// Создание базы данных
bool AssetManager::createDatabase()
{
	sqlite3* db;
	int exit = sqlite3_open(dbPath.c_str(), &db);

	if (exit != SQLITE_OK) {
		std::cerr << "Ошибка при создании базы данных: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	sqlite3_close(db);
	return true;
}

// Создание таблицы портфелей
bool AssetManager::createPortfolioTable() {
    sqlite3* db;
    sqlite3_open(dbPath.c_str(), &db);

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS Portfolios (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE
        );
    )";

    char* errMsg = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при создании таблицы портфелей: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

// Создание таблицы акций
bool AssetManager::createStockTable() {
    sqlite3* db;
    sqlite3_open(dbPath.c_str(), &db);

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS Stocks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            portfolio_id INTEGER,
            ticker TEXT NOT NULL,
            purchase_price REAL NOT NULL,
            quantity INTEGER NOT NULL,
            FOREIGN KEY (portfolio_id) REFERENCES Portfolios(id)
        );
    )";

    char* errMsg = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка при создании таблицы акций: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

// Добавление нового портфеля
bool AssetManager::addPortfolio(const std::string& portfolioName) {
    sqlite3* db;
    sqlite3_open(dbPath.c_str(), &db);

    std::string sql = "INSERT INTO Portfolios (name) VALUES (?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, portfolioName.c_str(), -1, SQLITE_STATIC);

    int exit = sqlite3_step(stmt);
    if (exit != SQLITE_DONE) {
        std::cerr << "Ошибка при добавлении портфеля: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}

// Добавление акции в портфель
bool AssetManager::addStock(int portfolioId, const std::string& ticker, double purchasePrice, int quantity) {
    sqlite3* db;
    sqlite3_open(dbPath.c_str(), &db);

    std::string sql = R"(
        INSERT INTO Stocks (portfolio_id, ticker, purchase_price, quantity)
        VALUES (?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_int(stmt, 1, portfolioId);
    sqlite3_bind_text(stmt, 2, ticker.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, purchasePrice);
    sqlite3_bind_int(stmt, 4, quantity);

    int exit = sqlite3_step(stmt);
    if (exit != SQLITE_DONE) {
        std::cerr << "Ошибка при добавлении акции: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}

// Получение всех портфелей
std::vector<Portfolio> AssetManager::getPortfolios() {
    sqlite3* db;
    sqlite3_open(dbPath.c_str(), &db);

    std::string sql = "SELECT id, name FROM Portfolios;";
    sqlite3_stmt* stmt;

    std::vector<Portfolio> portfolios;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка при подготовке запроса: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return portfolios;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Portfolio portfolio;
        portfolio.id = sqlite3_column_int(stmt, 0);
        portfolio.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        portfolios.push_back(portfolio);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return portfolios;
}

// Получение всех акций из портфеля
std::vector<Stock> AssetManager::getStocks(int portfolioId) {
    sqlite3* db;
    sqlite3_open(dbPath.c_str(), &db);

    std::string sql = R"(
        SELECT id, ticker, purchase_price, quantity 
        FROM Stocks 
        WHERE portfolio_id = ?;
    )";

    sqlite3_stmt* stmt;

    std::vector<Stock> stocks;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Ошибка при подготовке запроса: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return stocks;
    }

    sqlite3_bind_int(stmt, 1, portfolioId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Stock stock;
        stock.id = sqlite3_column_int(stmt, 0);
        stock.ticker = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        stock.purchasePrice = sqlite3_column_double(stmt, 2);
        stock.quantity = sqlite3_column_int(stmt, 3);
        stocks.push_back(stock);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return stocks;
}

// Отображенние портфелей
void AssetManager::displayPortfolios(const std::vector<Portfolio>& portfolios){
    std::cout << "Список портфелей:\n";
    for (const auto& portfolio : portfolios) {
        std::cout << "ID: " << portfolio.id << ", Название: " << portfolio.name << '\n';
    }
}

// Отображение активов
void AssetManager::displayStocks(const std::vector<Stock>& stocks){
    std::cout << "Список акций:\n";
    for (const auto& stock : stocks) {
        std::cout << "ID: " << stock.id << ", Тикер: " << stock.ticker
            << ", Цена: " << stock.purchasePrice
            << ", Количество: " << stock.quantity << '\n';
    }
}

// Удаление актива по имени
bool AssetManager::deleteAssetByName(int portfolioId, const std::string& stockName)
{
    sqlite3* db;
    char* errMsg = nullptr;

    // Открываем базу данных
    int rc = sqlite3_open(dbPath.c_str(), & db);
    if (rc) {
        std::cerr << "Ошибка открытия базы данных: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Подготавливаем SQL-запрос для удаления акции
    std::string sql = "DELETE FROM stocks WHERE portfolio_id = ? AND ticker = ?;";

    // Подготавливаем SQL-запрос
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // Привязываем параметры к запросу
    sqlite3_bind_int(stmt, 1, portfolioId);          // Привязываем portfolio_id
    sqlite3_bind_text(stmt, 2, stockName.c_str(), -1, SQLITE_STATIC); // Привязываем stock_name

    // Выполняем запрос
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Ошибка выполнения запроса: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    // Освобождаем ресурсы
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::cout << "Акция '" << stockName << "' успешно удалена из портфеля с ID " << portfolioId << "." << std::endl;
    return true;
}

// Функция переиндексации активов портфеля
bool AssetManager::reindexStocks() {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Ошибка открытия базы данных: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    char* errMsg = nullptr;

    // Создаём временную таблицу без id
    const char* createTempTableSQL = R"(
        CREATE TEMPORARY TABLE temp_stocks AS 
        SELECT portfolio_id, ticker, purchase_price, quantity FROM stocks;
    )";
    rc = sqlite3_exec(db, createTempTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка создания временной таблицы: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    // Очищаем основную таблицу и сбрасываем автоинкремент
    const char* clearTableSQL = "DELETE FROM stocks; DELETE FROM sqlite_sequence WHERE name='stocks';";
    rc = sqlite3_exec(db, clearTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка очистки таблицы: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    // Вставляем данные обратно с автоинкрементом
    const char* insertDataSQL = R"(
        INSERT INTO stocks (portfolio_id, ticker, purchase_price, quantity) 
        SELECT portfolio_id, ticker, purchase_price, quantity FROM temp_stocks;
    )";
    rc = sqlite3_exec(db, insertDataSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка восстановления данных: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

// Удаление базы данных
bool AssetManager::deleteDatabase()
{
    // Открываем базу данных
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Ошибка при открытии базы данных: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Удаляем все записи из таблицы Stocks
    const char* deleteStocksSQL = "DELETE FROM Stocks;";
    char* errMsg = nullptr;
    rc = sqlite3_exec(db, deleteStocksSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка при удалении акций: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    // Удаляем все записи из таблицы Portfolios
    const char* deletePortfoliosSQL = "DELETE FROM Portfolios;";
    rc = sqlite3_exec(db, deletePortfoliosSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка при удалении портфелей: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    // Очищаем последовательности автоинкремента для обеих таблиц
    const char* resetSequenceSQL = "DELETE FROM sqlite_sequence WHERE name='Stocks'; DELETE FROM sqlite_sequence WHERE name='Portfolios';";
    rc = sqlite3_exec(db, resetSequenceSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка при сбросе автоинкремента: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    // Закрываем соединение с базой данных
    sqlite3_close(db);

    // Удаляем сам файл базы данных (предположим, что база данных на диске)
    if (remove(dbPath.c_str()) != 0) {
        char buffer[1024];
        strerror_s(buffer, sizeof(buffer), errno);
        std::cerr << "Ошибка: " << buffer << std::endl;
        return false;
    }

    std::cout << "База данных и все портфели успешно удалены." << std::endl;
    return true;
}
