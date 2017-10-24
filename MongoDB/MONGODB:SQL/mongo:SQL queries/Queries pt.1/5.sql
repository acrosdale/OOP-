-- $ID$
-- TPC-H/TPC-R Local Supplier Volume Query (Q5)
-- Functional Query Definition
-- Approved February 1998
:x
:o
select
	n_name,
	sum(l_extendedprice * (1 - l_discount)) as revenue
from
	customer,
	orders,
	lineitem,
	supplier,
	nation,
	region
where
	c_custkey = o_custkey//
	and l_orderkey = o_orderkey//
	and l_suppkey = s_suppkey//
	and c_nationkey = s_nationkey
	and s_nationkey = n_nationkey//
	and n_regionkey = r_regionkey//
	and r_name = "AFRICA"
	and o_orderdate >= "1992-01-02"
	and o_orderdate < "1993-01-02"
group by
	n_name
order by
	revenue desc
:n -1


	db.lineitem.aggregate(
		    [{
				"$project": {
					"partsupp.supplier.nation.name" : 1,
					"partsupp.supplier.nation.region.name":1,
					"extendedprice":1,
					"discount":1,
					"order.orderdate": 1,

					"l_dis_min_1": {
						"$subtract": [
							1,
							"$discount"
						]
					},
					"c_nkTOs_nk":{
						"$cmp": [ "$order.customer.nationkey", "$partsupp.supplier.nationkey" ]

					}
				}
		    },
			{
				"$match": {
					"partsupp.supplier.nation.region.name" : "AFRICA",

					"c_nkTOs_nk":{
						"$eq": 0
					},

					"order.orderdate": {
						"$gte": "1992-01-02",
						"$lt":"1993-01-02"
					}
			 }
			}, {
		        "$group": {
		            "_id": {
						"n_name": "$partsupp.supplier.nation.name"
		            },
					"revenue":{
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
					"revenue": -1
		        }
		    }
			]).pretty()
