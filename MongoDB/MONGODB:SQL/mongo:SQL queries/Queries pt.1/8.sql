-- $ID$
-- TPC-H/TPC-R National Market Share Query (Q8)
-- Functional Query Definition
-- Approved February 1998
:x
:o
select
	o_year,
	sum(case
		when nation = 'BRAZIL' then volume
		else 0
	end) / sum(volume) as mkt_share
from
	(
		select
			--strftime('%Y',o_orderdate) as o_year,
			o_orderdate as o_year,
			l_extendedprice * (1 - l_discount) as volume,
			n2.n_name as nation
		from
			part,
			supplier,
			lineitem,
			orders,
			customer,
			nation n1,
			nation n2,
			region
		where
			p_partkey = l_partkey
			and s_suppkey = l_suppkey
			and l_orderkey = o_orderkey
			and o_custkey = c_custkey
			and c_nationkey = n1.n_nationkey
			and n1.n_regionkey = r_regionkey
			and r_name = 'AMERICA'
			and s_nationkey = n2.n_nationkey
			and o_orderdate between  '1995-01-01' and  '1996-12-31'
			and p_type = 'ECONOMY ANODIZED STEEL'
	) as all_nations
group by
	o_year
order by
	o_year;





	db.lineitem.aggregate(
		[

		{
			"$match": {
				"order.orderdate": {
					"$gte": "1995-01-01",
					"$lte": "1996-12-31"
				},
				"order.customer.nation.region.name": "AMERICA",

				"partsupp.part.type": 'ECONOMY ANODIZED STEEL'
			}
		},

		{
			"$project": {
				"extendedprice": 1,
				"partsupp.supplier.nation.name": 1,
				"order.orderdate": 1,
				"l_dis_min_1": {
					"$subtract": [
						1,
						"$discount"
					]
				}
			}
		}, {
			"$project": {
				"extendedprice": 1,
				"partsupp.supplier.nation.name": 1,
				"order.orderdate": 1,
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
					"nation": "$partsupp.supplier.nation.name",
					"o_year": "$order.orderdate"
				},

				"revenue": {
					"$sum": "$volume"
				}
			}
		},


		{
			"$project": {
				"_id.nation": 1,
				"_id.o_year": 1,
				"revenue": 1,
				"mkt_share": {
					"$sum": "$revenue"
				},
				"eq": {
					"$cond": [{
							"$eq": ["$_id.nation", "BRAZIL"]
						},
						"$revenue",
						0
					]
				}
			}
		},


		{
			"$project": {
				"_id.o_year": 1,

				"mkt_share": {
					"$sum": "$revenue"
				},

				"MKT_SHARE": {
					"$divide": ["$eq", "$mkt_share"]
				}

			}
		},

		{
			"$group": {
				"_id": {
					"o_year": "$_id.o_year",
					"mkt_share": "$MKT_SHARE"
				}
			}
		},

		{
			"$sort": {
				"order.orderdate": 1
			}
		}
	]).pretty()
















	///////////this is another version///////////////////////////////////////////////////////////////


	var line_id=[];

	db.lineitem.find({ "order.customer.nation.region.name": "AMERICA" }).forEach(function(u){ line_id.push(u._id) })

	db.lineitem.aggregate(
	    [{
	        "$match": {
	            "order.orderdate": {
	                "$gte": "1995-01-01",
	                "$lte": "1996-12-31"
	            },
	            "_id": {

	                "$in": line_id
	            },

	            "partsupp.part.type": 'ECONOMY ANODIZED STEEL'
	        }
	    }, {
	        "$project": {
	            "extendedprice": 1,
	            "partsupp.supplier.nation.name": 1,
	            "order.orderdate": 1,
	            "l_dis_min_1": {
	                "$subtract": [
	                    1,
	                    "$discount"
	                ]
	            }
	        }
	    }, {
	        "$project": {
	            "extendedprice": 1,
	            "partsupp.supplier.nation.name": 1,
	            "order.orderdate": 1,
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
	    }, {
	        "$group": {
	            "_id": {
	                "nation": "$partsupp.supplier.nation.name",
	                "o_year": "$order.orderdate"
	            },

	            "revenue": {
	                "$sum": "$volume"
	            }
	        }
	    }, {
	        "$project": {
	            "_id.nation": 1,
	            "_id.o_year": 1,
	            "revenue": 1,
	            "mkt_share": {
	                "$sum": "$revenue"
	            },
	            "eq": {
	                "$cond": [{
	                        "$eq": ["$_id.nation", "BRAZIL"]
	                    },
	                    "$revenue",
	                    0
	                ]
	            }
	        }
	    }, {
	        "$project": {
	            "_id.o_year": 1,

	            "mkt_share": {
	                "$sum": "$revenue"
	            },

	            "MKT_SHARE": {
	                "$divide": ["$eq", "$mkt_share"]
	            }

	        }
	    }, {
	        "$group": {
	            "_id": {
	                "o_year": "$_id.o_year",
	                "mkt_share": "$MKT_SHARE"
	            }
	        }
	    }, {
	        "$sort": {
	            "order.orderdate": 1
	        }
	    }
	]).pretty()
