-- $ID$
-- TPC-H/TPC-R Shipping Priority Query (Q3)
-- Functional Query Definition
-- Approved February 1998
:x
:o


select
	l_orderkey,
	sum(l_extendedprice * (1 - l_discount)) as revenue,
	o_orderdate,
	o_shippriority
from
	customer,
	orders,
	lineitem
where
	c_mktsegment = 'AUTOMOBILE'
	and c_custkey = o_custkey
	and l_orderkey = o_orderkey
	and o_orderdate <  '1995-03-15'
	and l_shipdate > '1995-03-31'
group by
	l_orderkey,
	o_orderdate,
	o_shippriority
order by
	revenue desc,
	o_orderdate;



:n 10



db.lineitem.aggregate(
    [

	{
        "$match": {
            "order.customer.mktsegment": "AUTOMOBILE",
            "order.orderdate": {
                "$lt": "1995-03-15"
            },
            "shipdate": {
                "$gt": "1995-03-31"
            }
        }
    },


	{
        "$project": {
            "order.orderkey":1,
            "order.orderdate": 1,
            "order.shippriority": 1,
            "extendedprice": 1,
            "l_dis_min_1": {
                "$subtract": [
                    1,
                    "$discount"
                ]
            }
        }
    },


	{
        "$group": {
            "_id": {

                "orderkey": "$order.orderkey",
                "orderdate": "$order.orderdate",
                "shippriority": "$order.shippriority"
            },
            "revenue": {
                "$sum": {
                    "$multiply": [
                        "$extendedprice",
                        "$l_dis_min_1"
                    ]
                }
            }

        }
    },


	{
        "$sort": {
			"revenue": -1,
            "order.orderdate":1
        }
    }


	]
).pretty()




function Start() {
	var start = new Date().getTime();

	db.lineitem.aggregate(
	    [{
	        "$match": {
	            "order.customer.mktsegment": "AUTOMOBILE",
	            "order.orderdate": {
	                "$lt": "1995-03-15"
	            },
	            "shipdate": {
	                "$gt": "1995-03-31"
	            }
	        }
	    },
		 {
	        "$project": {
	            "order.orderkey":1,
	            "order.orderdate": 1,
	            "order.shippriority": 1,
	            "extendedprice": 1,
	            "l_dis_min_1": {
	                "$subtract": [
	                    1,
	                    "$discount"
	                ]
	            }
	        }
	    }, {
	        "$group": {
	            "_id": {

	                "orderkey": "$order.orderkey",
	                "orderdate": "$order.orderdate",
	                "shippriority": "$order.shippriority"
	            },
	            "revenue": {
	                "$sum": {
	                    "$multiply": [
	                        "$extendedprice",
	                        "$l_dis_min_1"
	                    ]
	                }
	            }

	        }
	    }, {
	        "$sort": {
				"revenue": -1,
	            "order.orderdate":1
	        }
	    }]
	);

	var stop = new Date().getTime();

	var total_time = stop - start;

	printjson(total_time);
}
