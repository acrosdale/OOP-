-- $ID$
-- TPC-H/TPC-R Product Type Profit Measure Query (Q9)
-- Functional Query Definition
-- Approved February 1998
:x
:o

select
	nation,
	o_year,
	sum(amount) as sum_profit
from
	(
		select
			n_name as nation,
			--strftime('%Y', o_orderdate) as o_year,
            o_orderdate as o_year,
			(l_extendedprice * (1 - l_discount)) - (ps_supplycost * l_quantity) as amount
		from
			part,
			supplier,
			lineitem,
			partsupp,
			orders,
			nation
		where
			s_suppkey = l_suppkey
			and ps_suppkey = l_suppkey
			and ps_partkey = l_partkey
			and p_partkey = l_partkey
			and o_orderkey = l_orderkey
			and s_nationkey = n_nationkey
			and p_name like '%GREEN'
	) as profit
group by
	nation,
	o_year
order by
	nation,
	o_year desc;


"$regex": 'GREEN',
'%GREEN%'

db.lineitem.aggregate(
	[  {
	        "$match": {
	            "partsupp.part.name": {
					"$regex": 'GREEN$',
					"$options": 'i'
	            }
	        }
	    },
		{
			"$project": {
                "order.orderdate": 1,
				"partsupp.supplier.nation.name": 1,
				"extendedprice": 1,
				"discount": 1,
				"quantity": 1,
				"l_dis_min_1": {
	                "$subtract": [
	                    1,
	                    "$discount"
	                ]
	            },
				"amount_2":{
					"$multiply": [
						"$partsupp.supplycost",
						"$quantity"
					]
				}
			}
		},
		{
			"$project": {
                "order.orderdate": 1,
				"partsupp.supplier.nation.name": 1,
				"l_dis_min_1": 1,
				"extendedprice": 1,
				"amount_2": 1,
				"amount_1":{
					"$multiply": [
						"$l_dis_min_1",
						"$extendedprice"
					]
				}
			}
		},
		{
			"$project": {
                "order.orderdate": 1,
				"partsupp.supplier.nation.name": 1,
				"amount_final":{
					"$subtract": [
						"$amount_1",
						"$amount_2"
					]
				}

			}
		},
        {
	        "$group": {
	            "_id": {
                    "o_year": "$order.orderdate",
                    "nation": "$partsupp.supplier.nation.name"
	            },
                "sum_total":{
                    "$sum": "$amount_final"
                }
	        }
	    },
        {
            "$sort": {
                "partsupp.supplier.nation.name" :1,
                "order.orderdate": -1

            }
        }
    ]
).pretty()










//"o_year": "$order.orderdate",
"nation": "$partsupp.supplier.nation.name"\
,
{
    "$match": {
        "_id.o_year": {
            "$regex": '1992',
            "$options": 'i'

        },
        "_id.nation": {
            "$eq": "ARGENTINA"
        }
    }
},

.forEach(function(u){
    year.push(u.order.orderdate);  //[0] + u.order.orderdate[1]+ u.order.orderdate[2]+ u.order.orderdate[3]
    profit.push(u.amount_final);
    nation.push(u.partsupp.supplier.nation.name);
 });


	db.lineitem.aggregate(
	    [
		 {
	        "$project": {
                "partsupp.supplier.nation.name": 1,
                "order.orderdate": 1,

	        }
	    },
        {
            "$match": {

                "partsupp.supplier.nation.name":{
                    "$in": nation
                },

                "order.orderdate":{
                    "$in": year
                }

            }
        },
        {
	        "$group": {
	            "_id": {
                    "o_year": "$order.orderdate",
                    "nation": "$partsupp.supplier.nation.name"
	            }//,
                //"sum_profit":{
                    //"$sum": profit
                //}
	        }
	    }, {
	        "$sort": {
                "order.orderdate": -1,
                "partsupp.supplier.nation.name": 1
	        }
	    }]
	).pretty()




    db.lineitem.aggregate(
	    [
		 {
	        "$project": {
                nation,
                year

	        }
	    }])
