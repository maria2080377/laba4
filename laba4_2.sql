INSERT INTO categories (id, category_name) VALUES 
(1, 'Electronics'),
(2, 'Clothing'),
(3, 'Books'),
(4, 'Software');


INSERT INTO products (id, name, price, quantity, category_id) VALUES 
(1, 'Smartphone', 30000, 50, 1),
(2, 'Laptop', 80000, 30, 1),
(3, 'T-Shirt', 1500, 100, 2),
(4, 'Shool Book', 2000, 50, 3),
(5, 'Microsoft Office', 5000, 200, 4);


INSERT INTO sales (id, product_id, sale_date, quantity_sold) VALUES 
(1, 1, '2024-01-10', 2),
(2, 1, '2024-01-15', 1),
(3, 3, '2024-01-12', 5),
(4, 4, '2024-01-20', 3),
(5, 5, '2024-01-25', 10);