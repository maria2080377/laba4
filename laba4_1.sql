CREATE TABLE categories (
    id SERIAL PRIMARY KEY,
    category_name VARCHAR(100) NOT NULL);


CREATE TABLE products (
    id SERIAL PRIMARY KEY,
    name VARCHAR(200) NOT NULL,
    price INT NOT NULL,
    quantity INTEGER NOT NULL,
    category_id INTEGER REFERENCES categories(id));

CREATE TABLE sales (
    id SERIAL PRIMARY KEY,
    product_id INTEGER REFERENCES products(id),
    sale_date DATE NOT NULL DEFAULT CURRENT_DATE,
    quantity_sold INTEGER NOT NULL);


CREATE INDEX idx_category_name ON categories(category_name);
CREATE INDEX idx_product_name ON products(name);
CREATE INDEX idx_sale_date ON sales(sale_date);