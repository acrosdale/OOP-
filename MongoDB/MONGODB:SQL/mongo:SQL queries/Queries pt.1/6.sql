-- $ID$
-- TPC-H/TPC-R Forecasting Revenue Change Query (Q6)
-- Functional Query Definition
-- Approved February 1998
:x
:o
select
	sum(l_extendedprice * l_discount) as revenue
from
	lineitem
where
	l_shipdate >=  '1994-01-01'
	and l_shipdate < '1995-01-01'
	and l_discount between 0.06 - 0.01 and 0.06 + 0.01
	and l_quantity <  24
:n -1


db.lineitem.aggregate(
	    [{
			"$match": {
				"shipdate":{
					"$gte": "1994-01-01",
					"$lt": "1995-01-01"
				},
				"quantity":{
					"$lt": 24

				},
				"discount":{
					"$gte": 0.06 - 0.01,
					"$lte": 0.06 + 0.01
				}
			}
	    },
		 {
	        "$project": {
				"extendedprice": 1,
				"discount": 1

	        }
	    }, {
	        "$group": {
	            "_id": {

	            },
				"revenue": {
	                "$sum": {
	                    "$multiply": [
	                        "$extendedprice",
	                        "$discount"
	                    ]
	                }
	            }

	        }
	    }, {
	        "$sort": {
				"revenue": 1
	        }
	    }]
	).pretty()
