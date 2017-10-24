
select
  ps_partkey,
   ps_supplycost * ps_availqty as value
from
    partsupp,
    supplier,
    nation
where
    ps_suppkey = s_suppkey
    and s_nationkey = n_nationkey
    and n_name = 'ARGENTINA'


db.lineitem.aggregate(
    [{
        "$match": {

            "partsupp.supplier.nation.name": "ARGENTINA",
        }
    },
	 {
        "$project": {
			"partsupp.partkey": 1,
            "partsupp.supplycost":1,
            "partsupp.availqty":1,

        }
    }, {
        "$group": {
            "_id": {
				"p_partkey":"$partsupp.partkey"
            },
            "value": {
                "$sum":{
                    "$multiply": [
					    "$partsupp.supplycost",
					    "$partsupp.availqty"
                    ]
                }
            }
        }
    }]
).pretty()
