-- $ID$
-- TPC-H/TPC-R Returned Item Reporting Query (Q10)
-- Functional Query Definition
-- Approved February 1998
:x
:o

select
	c_custkey,
	c_name,
	sum(l_extendedprice * (1 - l_discount)) as revenue,
	c_acctbal,
	n_name,
	c_address,
	c_phone,
	c_comment
from
	customer,
	orders,
	lineitem,
	nation
where
	c_custkey = o_custkey
	and l_orderkey = o_orderkey
	and o_orderdate >= '1993-10-14'
	and o_orderdate <  '1994-01-14'
	and l_returnflag = 'R'
	and c_nationkey = n_nationkey
group by
	c_custkey,
	c_name,
	c_acctbal,
	c_phone,
	n_name,
	c_address,
	c_comment
order by
	revenue desc;


:n 20



db.lineitem.aggregate(
    [

	{
        "$match": {
            "order.orderdate":{
				"$gte" : "1993-10-14",
				"$lt" : "1994-01-14",
			},
			"returnflag": "R"
        }
    },

	 {
        "$project": {
			"order.customer.custkey":1,
			"order.customer.name":1,
			"order.customer.acctbal":1,
			"order.customer.phone":1,
			"order.customer.nation.name":1,
			"order.customer.address":1,
			"order.customer.comment":1,
            "extendedprice": 1,
            "discount": 1,
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
			"c_custkey" : "$order.customer.custkey",
			"c_name"	: "$order.customer.name",
			"c_acctbal" : "$order.customer.acctbal",
			"c_phone"	: "$order.customer.phone",
			"n_name"	: "$order.customer.nation.name",
			"c_address" : "$order.customer.address",
			"c_comment" : "$order.customer.comment",
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
            "revenue": -1
        }
    }



	]
).pretty()
