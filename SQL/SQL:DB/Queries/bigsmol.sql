SELECT c_name
FROM customer,nation
WHERE c_nationkey = n_nationkey AND n_nationkey > 0