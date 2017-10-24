-- $ID$
-- TPC-H/TPC-R Important Stock Identification Query (Q11)
-- Functional Query Definition
-- Approved February 1998



select
	ps_partkey,
	sum(ps_supplycost * ps_availqty) as value
from
	partsupp,
	supplier,
	nation
where
	ps_suppkey = s_suppkey
	and s_nationkey = n_nationkey
	and n_name = 'ARGENTINA'
group by
	ps_partkey having
		sum(ps_supplycost * ps_availqty) > (
			select
				sum(ps_supplycost * ps_availqty) *  0.0001
			from
				partsupp,
				supplier,
				nation
			where
				ps_suppkey = s_suppkey
				and s_nationkey = n_nationkey
				and n_name = 'ARGENTINA'
		)
order by
	value desc;
:n -1

var inner_cmp = db.partsupp.aggregate(
    [{
        "$match": {

            "supplier.nation.name": "ARGENTINA",
        }
    },
	 {
        "$project": {
			"supplycost":1,
			"availqty":1,

        }
    }, {
        "$group": {
            "_id": {

            },
			"value": {
			   "$sum": {
				   "$multiply": [
					   "$supplycost",
					   "$availqty",0.0001
				   ]
			   }
		   }
        }
    }]
).toArray();

db.lineitem.aggregate(
    [{
	   "$project": {

		   "partsupp.partkey": 1,
		   "partsupp.supplycost":1,
		   "partsupp.availqty":1,
		   "partsupp.supplier.nation.name":1,
		   "value": {
			   "$sum": {
				   "$multiply": [
					   "$partsupp.supplycost",
					   "$partsupp.availqty"
				   ]
			   }
		   }
	   }
   },
	 {
		"$match":{
			"partsupp.supplier.nation.name": "ARGENTINA",
			"value": {
				"$gt": inner_cmp[0].value
			}

		}
    },
	{
        "$group": {
            "_id": {
				"ps_partkey": "$partsupp.partkey",
				"value": "$value"
            }
        }
    },
	{
        "$sort": {
			"value": -1
        }
    }]
).pretty()
