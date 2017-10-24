-- $ID$
-- TPC-H/TPC-R Order Priority Checking Query (Q4)
-- Functional Query Definition
-- Approved February 1998
:x
:o

select
	o_orderpriority,
	count(*) as order_count
from
	orders
where
o_orderdate >= '1993-07-01'
and o_orderdate < '1993-10-01'
	and exists (
		select
			*
		from
			lineitem
		where
			l_orderkey = o_orderkey
			and l_commitdate < l_receiptdate --remove this and get the exact print o
	)
group by
	o_orderpriority
order by
	o_orderpriority;


	var exist = [];
	db.lineitem.aggregate(
	    [{
	        "$project": {
	            "orderkey": 1,
	            "eq": {
	                "$cond": [{
	                        "$lt": ["$commitdate", "$receiptdate"]
	                    },
	                    1,
	                    0
	                ]
	            }
	        }
	    }, {
	        "$match": {
	            "eq": {
	                "$eq": 1
	            }
	        }
	    }]
	).forEach(function(u) {
	    exist.push(u.orderkey)
	})


	db.orders.aggregate(
	    [{
	        "$project": {
	            "orderpriority": 1,
	            "orderdate": 1,
	            "orderkey": 1,

	        }
	    }, {
	        "$match": {
	            "orderdate": {
	                "$gte": "1993-07-01",
	                "$lt": "1993-10-01"
	            },
	            "orderkey": {
	                "$in": exist
	            }
	        }
	    }, {
	        "$group": {
	            "_id": {
	                "orderpriority": "$orderpriority",

	            },
	            "order_count": {
	                "$sum": 1
	            }
	        }
	    }, {
	        "$sort": {
	            "orderpriority": 1
	        }
	    }]
	).pretty()










	//timer code
	function insert3() {
		var start = new Date().getTime();

		//insert mongo query here

		var end = new Date().getTime();
		var time = end - start;
		printjson(time);
	}
	//since mongo embeding struct have dupplicates i used both collection lineitem and orders
