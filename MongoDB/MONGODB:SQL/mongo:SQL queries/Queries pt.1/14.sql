-- $ID$
-- TPC-H/TPC-R Promotion Effect Query (Q14)
-- Functional Query Definition
-- Approved February 1998
:x
:o
select
	100.00 * sum(case
		when p_type like 'PROMO%'
			then l_extendedprice * (1 - l_discount)
		else 0
	end) / sum(l_extendedprice * (1 - l_discount)) as promo_revenue
from
	lineitem,
	part
where
	l_partkey = p_partkey
	and l_shipdate >= '1995-09-01'
	and l_shipdate < '1995-10-01';
:n -1

var rev_divider = db.lineitem.aggregate(
    [{
        "$project": {
            "extendedprice": 1,
            "shipdate": 1,
            "partsupp.part.type": 1,
            "l_dis_min_1": {
                "$subtract": [
                    1,
                    "$discount"
                ]
            }
        }
    }, {
        "$match": {
            "shipdate": {
                "$gte": "1995-09-01",
                "$lt": "1995-10-01"
            },

        }
    }, {
        "$group": {
            "_id": {},
            "rev": {
                "$sum": {
                    "$multiply": ["$extendedprice", "$l_dis_min_1"]
                },

            }

        }
    }]).toArray();

db.lineitem.aggregate(
    [{
        "$match": {
            "shipdate": {
                "$gte": "1995-09-01",
                "$lt": "1995-10-01"
            },
            "partsupp.part.type": {
                "$regex": 'PROMO',
                "$options": 'i'
            }
        }
    }, {
        "$project": {
            "extendedprice": 1,
            "l_dis_min_1": {
                "$subtract": [
                    1,
                    "$discount"
                ]
            }
        }
    }, {
        "$group": {
            "_id": {},
            "revenue": {
                "$sum": {
                    "$multiply": ["$extendedprice", "$l_dis_min_1", 100.00]
                }
            }
        }
    }, {
        "$project": {

            "promo_revenue": {
                "$divide": ["$revenue", rev_divider[0].rev]
            }

        }
    }, {
        "$sort": {
            "promo_revenue": 1
        }
    }]).pretty()
