-- $ID$
-- TPC-H/TPC-R Volume Shipping Query (Q7)
-- Functional Query Definition
-- Approved February 1998
:x
:o
select
	supp_nation,
	cust_nation,
	l_year,
	sum(volume) as revenue
from
	(
		select
			n1.n_name as supp_nation,
			n2.n_name as cust_nation,
			--strftime('%Y', l_shipdate) as l_year,
			l_shipdate as l_year,
			l_extendedprice * (1 - l_discount) as volume
		from
			supplier,
			lineitem,
			orders,
			customer,
			nation n1,
			nation n2
		where
			s_suppkey = l_suppkey
			and o_orderkey = l_orderkey
			and c_custkey = o_custkey
			and s_nationkey = n1.n_nationkey
			and c_nationkey = n2.n_nationkey
			and (
				(n1.n_name = 'ARGENTINA' and n2.n_name = 'BRAZIL')
				or (n1.n_name = 'BRAZIL' and n2.n_name = 'ARGENTINA')
			)
			and l_shipdate between '1996-01-01' and '1997-12-31'
	) as shipping
group by
	supp_nation,
	cust_nation,
	l_year
order by
	supp_nation,
	cust_nation,
	l_year;


	db.lineitem.aggregate(
		    [{
				"$match": {
					"shipdate":{
						"$gte": "1996-01-01",
						"$lte": "1997-12-31"
					},
					"$or": [{
						"$and": [{
							"order.customer.nation.name": "BRAZIL"
						}, {
							"partsupp.supplier.nation.name": "ARGENTINA"
						}]
					}, {
						"$and": [{
							"order.customer.nation.name": "ARGENTINA"
						}, {
							"partsupp.supplier.nation.name": "BRAZIL"
						}]
					}]
				}
			},
			{
		        "$project": {
		            "extendedprice": 1,
					"order.customer.nation.name": 1,
					"partsupp.supplier.nation.name": 1,
					"shipdate":1,
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
		        "$project": {
		            "extendedprice": 1,
					"order.customer.nation.name": 1,
					"partsupp.supplier.nation.name": 1,
					"shipdate":1,
		            "discount": 1,
					"l_dis_min_1": {
		                "$subtract": [
		                    1,
		                    "$discount"
		                ]
		            },
					"volume": {
						"$multiply": [
							"$extendedprice",
							"$l_dis_min_1"
						]
					}
		        }
		    },
			{
		        "$group": {
		            "_id": {
						"supp_nation" :"$partsupp.supplier.nation.name",
						"cust_nation" :"$order.customer.nation.name",
		                "l_shipdate": "$shipdate"
		            },

					"revenue": {
		                "$sum": "$volume"
		            }
		        }
		    }
			]).pretty()





//// this case below is also valid
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



	var line_id=[];

	db.lineitem.find({
		$or :[ { $and : [ { "order.customer.nation.name": "BRAZIL" },{ "partsupp.supplier.nation.name": "ARGENTINA"} ] },
		{ $and : [ {  "order.customer.nation.name": "ARGENTINA" }, { "partsupp.supplier.nation.name": "BRAZIL" } ] }]},
		{"_id": 1, "order.customer.nation.name": 1,"partsupp.supplier.nation.name": 1}).forEach(function(u){ line_id.push(u._id) }
	)


db.lineitem.aggregate(
	    [{
			"$match": {
				"shipdate":{
					"$gte": "1996-01-01",
					"$lte": "1997-12-31"
				},
				"_id":{

					"$in": line_id

				}
			}
		},
		{
	        "$project": {
	            "extendedprice": 1,
				"order.customer.nation.name": 1,
				"partsupp.supplier.nation.name": 1,
				"shipdate":1,
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
	        "$project": {
	            "extendedprice": 1,
				"order.customer.nation.name": 1,
				"partsupp.supplier.nation.name": 1,
				"shipdate":1,
	            "discount": 1,
				"l_dis_min_1": {
	                "$subtract": [
	                    1,
	                    "$discount"
	                ]
	            },
				"volume": {
					"$multiply": [
						"$extendedprice",
						"$l_dis_min_1"
					]
				}
	        }
	    },
		{
	        "$group": {
	            "_id": {
					"supp_nation" :"$partsupp.supplier.nation.name",
					"cust_nation" :"$order.customer.nation.name",
	                "l_shipdate": "$shipdate"
	            },

				"revenue": {
	                "$sum": "$volume"
	            }
	        }
	    }
		]).pretty()
