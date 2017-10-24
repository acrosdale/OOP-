SELECT c_name
FROM customer,supplier
WHERE  c_nationkey = s_nationkey AND s_nationkey > 0
