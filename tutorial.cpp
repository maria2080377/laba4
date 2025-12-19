#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace pqxx;

connection connectToDatabase() {
    try {
        connection conn("host=localhost port=5432 dbname=products_db user=postgres password=123456");

        if (conn.is_open()) {
            cout << "Успешное подключение к базе данных!" << endl;
            return conn;
        }
        else {
            throw runtime_error("Не удалось подключиться к базе данных");
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка подключения: " << e.what() << endl;
        throw;
    }
}

void showMenu() {
    cout << "\n=== СИСТЕМА УПРАВЛЕНИЯ ПРОДУКТАМИ ===" << endl;
    cout << "1. Показать все категории" << endl;
    cout << "2. Показать все продукты" << endl;
    cout << "3. Показать все продажи" << endl;
    cout << "4. Добавить новую категорию" << endl;
    cout << "5. Добавить новый продукт" << endl;
    cout << "6. Добавить новую продажу" << endl;
    cout << "7. Найти продукты по категории" << endl;
    cout << "8. Посчитать общую выручку" << endl;
    cout << "9. Показать топ-5 продаваемых продуктов" << endl;
    cout << "10. Показать количество проданных единиц" << endl;
    cout << "0. Выход" << endl;
    cout << "Выберите действие: ";
}

void showCategories(connection& conn) {
    try {
        work txn(conn);
        result res = txn.exec("SELECT * FROM categories ORDER BY id");

        cout << "\n=== КАТЕГОРИИ ===" << endl;
        for (const auto& row : res) {
            cout << "ID: " << row[0].as<int>()
                << ", Название: " << row[1].as<string>() << endl;
        }
        cout << "Всего: " << res.size() << " категорий" << endl;
        txn.commit();
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void showProducts(connection& conn) {
    try {
        work txn(conn);
        result res = txn.exec("SELECT * FROM products ORDER BY id");

        cout << "\n=== ПРОДУКТЫ ===" << endl;
        for (const auto& row : res) {
            cout << "ID: " << row[0].as<int>()
                << ", Название: " << row[1].as<string>()
                << ", Цена: " << row[2].as<int>()
                << ", Количество: " << row[3].as<int>()
                << ", Категория ID: " << row[4].as<int>() << endl;
        }
        cout << "Всего: " << res.size() << " продуктов" << endl;
        txn.commit();
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void showSales(connection& conn) {
    try {
        work txn(conn);
        result res = txn.exec("SELECT * FROM sales ORDER BY sale_date");

        cout << "\n=== ПРОДАЖИ ===" << endl;
        for (const auto& row : res) {
            cout << "ID: " << row[0].as<int>()
                << ", Продукт ID: " << row[1].as<int>()
                << ", Дата: " << row[2].as<string>()
                << ", Количество: " << row[3].as<int>() << endl;
        }
        cout << "Всего: " << res.size() << " продаж" << endl;
        txn.commit();
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void addCategory(connection& conn) {
    int id;
    string name;

    cout << "\n=== ДОБАВЛЕНИЕ КАТЕГОРИИ ===" << endl;
    cout << "Введите ID категории: ";
    cin >> id;
    cin.ignore();
    cout << "Введите название категории: ";
    getline(cin, name);

    try {
        work txn(conn);
        txn.exec("INSERT INTO categories (id, category_name) VALUES (" +
            to_string(id) + ", " + txn.quote(name) + ")");
        txn.commit();
        cout << "Категория успешно добавлена!" << endl;
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void addProduct(connection& conn) {
    int id, price, quantity, category_id;
    string name;

    cout << "\n=== ДОБАВЛЕНИЕ ПРОДУКТА ===" << endl;
    cout << "Введите ID продукта: ";
    cin >> id;
    cin.ignore();
    cout << "Введите название продукта: ";
    getline(cin, name);
    cout << "Введите цену: ";
    cin >> price;
    cout << "Введите количество на складе: ";
    cin >> quantity;
    cout << "Введите ID категории: ";
    cin >> category_id;

    try {
        work txn(conn);
        txn.exec("INSERT INTO products (id, name, price, quantity, category_id) VALUES (" +
            to_string(id) + ", " + txn.quote(name) + ", " +
            to_string(price) + ", " + to_string(quantity) + ", " +
            to_string(category_id) + ")");
        txn.commit();
        cout << "Продукт успешно добавлен!" << endl;
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void addSale(connection& conn) {
    int id, product_id, quantity;
    string date;

    cout << "\n=== ДОБАВЛЕНИЕ ПРОДАЖИ ===" << endl;
    cout << "Введите ID продажи: ";
    cin >> id;
    cout << "Введите ID продукта: ";
    cin >> product_id;
    cout << "Введите дату (гггг-мм-дд): ";
    cin >> date;
    cout << "Введите количество: ";
    cin >> quantity;

    try {
        work txn(conn);

        result check = txn.exec(
            "SELECT quantity FROM products WHERE id = " + to_string(product_id)
        );

        if (!check.empty()) {
            int available = check[0][0].as<int>();
            if (quantity <= available) {
                txn.exec("INSERT INTO sales (id, product_id, sale_date, quantity_sold) VALUES (" +
                    to_string(id) + ", " + to_string(product_id) + ", " +
                    txn.quote(date) + ", " + to_string(quantity) + ")");

                txn.exec("UPDATE products SET quantity = quantity - " + to_string(quantity) +
                    " WHERE id = " + to_string(product_id));

                txn.commit();
                cout << "Продажа успешно добавлена!" << endl;
            }
            else {
                cout << "Ошибка: недостаточно товара на складе!" << endl;
                txn.abort();
            }
        }
        else {
            cout << "Ошибка: продукт не найден!" << endl;
            txn.abort();
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void findProductsByCategory(connection& conn) {
    int category_id;

    cout << "\n=== ПОИСК ПРОДУКТОВ ПО КАТЕГОРИИ ===" << endl;
    cout << "Введите ID категории: ";
    cin >> category_id;

    try {
        work txn(conn);
        result res = txn.exec(
            "SELECT p.*, c.category_name as category_name "
            "FROM products p "
            "JOIN categories c ON p.category_id = c.id "
            "WHERE p.category_id = " + to_string(category_id)
        );

        if (!res.empty()) {
            cout << "\nПродукты категории " << category_id << ":" << endl;
            for (const auto& row : res) {
                cout << "ID: " << row[0].as<int>()
                    << ", Название: " << row[1].as<string>()
                    << ", Цена: " << row[2].as<int>()
                    << ", Количество: " << row[3].as<int>()
                    << ", Категория: " << row[5].as<string>() << endl;
            }
        }
        else {
            cout << "В этой категории нет продуктов или категория не существует." << endl;
        }
        txn.commit();
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void calculateTotalRevenue(connection& conn) {
    try {
        work txn(conn);
        result res = txn.exec(
            "SELECT p.name, SUM(s.quantity_sold * p.price) as total_revenue "
            "FROM products p "
            "JOIN sales s ON p.id = s.product_id "
            "GROUP BY p.id, p.name "
            "ORDER BY total_revenue DESC"
        );

        cout << "\n=== ОБЩАЯ ВЫРУЧКА ===" << endl;
        long long grand_total = 0;
        for (const auto& row : res) {
            long long revenue = row[1].as<long long>();
            cout << row[0].as<string>() << ": " << revenue << " руб." << endl;
            grand_total += revenue;
        }
        cout << "------------------------" << endl;
        cout << "ИТОГО: " << grand_total << " руб." << endl;
        txn.commit();
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void showTop5Products(connection& conn) {
    try {
        work txn(conn);
        result res = txn.exec(
            "SELECT p.name, SUM(s.quantity_sold) as total_sold "
            "FROM products p "
            "JOIN sales s ON p.id = s.product_id "
            "GROUP BY p.id, p.name "
            "ORDER BY total_sold DESC "
            "LIMIT 5"
        );

        cout << "\n=== ТОП-5 ПРОДАВАЕМЫХ ПРОДУКТОВ ===" << endl;
        int rank = 1;
        for (const auto& row : res) {
            cout << rank++ << ". " << row[0].as<string>()
                << " - " << row[1].as<long long>() << " шт." << endl;
        }
        txn.commit();
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

void showSoldQuantities(connection& conn) {
    try {
        work txn(conn);
        result res = txn.exec(
            "SELECT p.name, SUM(s.quantity_sold) as total_sold "
            "FROM products p "
            "JOIN sales s ON p.id = s.product_id "
            "GROUP BY p.id, p.name "
            "ORDER BY total_sold DESC"
        );

        cout << "\n=== КОЛИЧЕСТВО ПРОДАННЫХ ЕДИНИЦ ===" << endl;
        for (const auto& row : res) {
            cout << row[0].as<string>() << ": " << row[1].as<long long>() << " шт." << endl;
        }
        txn.commit();
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    cout << "СИСТЕМА УПРАВЛЕНИЯ ПРОДУКТАМИ" << endl;

    try {
        connection conn = connectToDatabase();

        {
            work txn(conn);
            result r = txn.exec("SELECT version();");
            cout << "Версия PostgreSQL: " << r[0][0].c_str() << endl;
            txn.commit();
        }

        int choice;
        do {
            showMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                showCategories(conn);
                break;
            case 2:
                showProducts(conn);
                break;
            case 3:
                showSales(conn);
                break;
            case 4:
                addCategory(conn);
                break;
            case 5:
                addProduct(conn);
                break;
            case 6:
                addSale(conn);
                break;
            case 7:
                findProductsByCategory(conn);
                break;
            case 8:
                calculateTotalRevenue(conn);
                break;
            case 9:
                showTop5Products(conn);
                break;
            case 10:
                showSoldQuantities(conn);
                break;
            case 0:
                cout << "Выход из программы..." << endl;
                break;
            default:
                cout << "Неверный выбор! Попробуйте снова." << endl;
            }
        } while (choice != 0);

        conn.close();
    }
    catch (const exception& e) {
        cerr << "Критическая ошибка: " << e.what() << endl;
        return 1;
    }

    cout << "Программа завершена." << endl;
    return 0;
}
