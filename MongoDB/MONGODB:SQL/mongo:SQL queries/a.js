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
