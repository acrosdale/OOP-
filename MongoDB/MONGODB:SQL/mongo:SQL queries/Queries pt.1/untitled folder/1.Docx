-- $ID$
-- TPC-H/TPC-R Pricing Summary Report Query (Q1)
-- Functional Query Definition
-- Approved February 1998
--done--1,2,3,4,5,6,7,8,9,10,11,14,17,16,18
--doable--12,13,19,20,21,22
--4

-- 16 is done but the matching thing recheck it
db.users.insert({name: 'paulo'})
db.users.insert({name: 'patric'})
db.users.insert({name: 'pedro'})

db.users.find({name: /a/})  //like '%a%'
out: paulo, patric

db.users.find({name: /^pa/}) //like 'pa%'
out: paulo, patric

db.users.find({name: /ro$/}) //like '%ro'
out: pedro



select
	l_returnflag,
	l_linestatus,
	sum(l_quantity) as sum_qty,
	sum(l_extendedprice) as sum_base_price,
	sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,
	sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,
	avg(l_quantity) as avg_qty,
	avg(l_extendedprice) as avg_price,
	avg(l_discount) as avg_disc,
	count(*) as count_order
from
	lineitem
where
	l_shipdate <= '1996-01-10'
group by
	l_returnflag,
	l_linestatus
order by
	l_returnflag,
	l_linestatus;





db.lineitem.aggregate(
    [{
        "$match": {
            "shipdate": {
                "$lte": "1996-01-10"
            }
        }
    },
	 {
        "$project": {
            "returnflag": 1,
            "linestatus": 1,
            "quantity": 1,
            "extendedprice": 1,
            "discount": 1,
            "l_dis_min_1": {
                "$subtract": [
                    1,
                    "$discount"
                ]
            },
            "l_tax_plus_1": {
                "$add": [
                    1,
					"$tax"

                ]
            }
        }
    }, {
        "$group": {
            "_id": {
                "RETURNFLAG": "$returnflag",
                "LINESTATUS": "$linestatus"
            },
            "sum_qty": {
                "$sum": "$quantity"
            },
            "sum_base_price": {
                "$sum": "$extendedprice"
            },
            "sum_disc_price": {
                "$sum": {
                    "$multiply": [
                        "$extendedprice",
                        "$l_dis_min_1"
                    ]
                }
            },
            "sum_charge": {
                "$sum": {
                    "$multiply": [
                        "$extendedprice",
						"$l_dis_min_1",
						"$l_tax_plus_1"
                    ]
                }
            },
            "avg_qty":{
                "$avg": "$quantity"
            },
            "avg_price": {
                "$avg": "$extendedprice"
            },

            "avg_disc": {
                "$avg": "$discount"
            },
            "count_order": {
                "$sum": 1
            }
        }
    }, {
        "$sort": {
            "returnflag": 1,
            "linestatus": 1
        }
    }]
).pretty()












function Start() {
	var start = new Date().getTime();

	db.lineitem.aggregate(
	    [{
	        "$match": {
	            "shipdate": {
	                "$lte": "1996-01-10"
	            }
	        }
	    },
		 {
	        "$project": {
	            "returnflag": 1,
	            "linestatus": 1,
	            "quantity": 1,
	            "extendedprice": 1,
	            "discount": 1,
	            "l_dis_min_1": {
	                "$subtract": [
	                    1,
	                    "$discount"
	                ]
	            },
	            "l_tax_plus_1": {
	                "$add": [
	                    1,
						"$tax"

	                ]
	            }
	        }
	    }, {
	        "$group": {
	            "_id": {
	                "RETURNFLAG": "$returnflag",
	                "LINESTATUS": "$linestatus"
	            },
	            "sum_qty": {
	                "$sum": "$quantity"
	            },
	            "sum_base_price": {
	                "$sum": "$extendedprice"
	            },
	            "sum_disc_price": {
	                "$sum": {
	                    "$multiply": [
	                        "$extendedprice",
	                        "$l_dis_min_1"
	                    ]
	                }
	            },
	            "sum_charge": {
	                "$sum": {
	                    "$multiply": [
	                        "$extendedprice",
							"$l_dis_min_1",
							"$l_tax_plus_1"
	                    ]
	                }
	            },
	            "avg_qty":{
	                "$avg": "$quantity"
	            },
	            "avg_price": {
	                "$avg": "$extendedprice"
	            },

	            "avg_disc": {
	                "$avg": "$discount"
	            },
	            "count_order": {
	                "$sum": 1
	            }
	        }
	    }, {
	        "$sort": {
	            "returnflag": 1,
	            "linestatus": 1
	        }
	    }]
	).pretty();

	var stop = new Date().getTime();

	var total_time = stop - start;
	printjson(total_time);
}
