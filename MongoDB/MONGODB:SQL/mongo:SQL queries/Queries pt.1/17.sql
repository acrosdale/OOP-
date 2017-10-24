-- $ID$
-- TPC-H/TPC-R Small-Quantity-Order Revenue Query (Q17)
-- Functional Query Definition
-- Approved February 1998
:x
:o
select
	sum(l_extendedprice) / 7.0 as avg_yearly
from
	lineitem,
	part
where
	p_partkey = l_partkey
	and p_brand = 'Brand#53'
	and p_container = 'MED BOX'
	and l_quantity < (
		select
			0.2 * avg(l_quantity)
		from
			lineitem
		where
			l_partkey = p_partkey
	);
:n -1

--run both Query

var l_quantity_check = db.lineitem.aggregate(
    [{
        "$match": {
            "partsupp.part.brand": "Brand#53",
			"partsupp.part.container": "MED BOX"
        }
    },
	 {
        "$project": {
			"quantity":1,
			"l_quantity":{
				"$multiply":[
					"$quantity",
					0.2
				]
			}
        }
    },
	{
        "$group": {
            "_id": {},
			"quantity":{
				"$avg": "$l_quantity"
			}
        }
    }]
).toArray();

db.lineitem.aggregate(
    [{
        "$match": {
            "partsupp.part.brand": "Brand#53",
			"partsupp.part.container": "MED BOX",
			"quantity": {
				"$lt":l_quantity_check[0].quantity
			}
        }
    },
	 {
        "$project": {
			"extendedprice":1,
			"extend_price":{
				"$divide":["$extendedprice", 7.0]
			}
        }
    }, {
        "$group": {
            "_id": {

            },
			"avg_yearly":{
				"$sum": "$extend_price"
			}
        }
    }]
).pretty()
