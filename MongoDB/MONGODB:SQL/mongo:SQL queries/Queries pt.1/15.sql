-- $ID$
-- TPC-H/TPC-R Top Supplier Query (Q15)
-- Functional Query Definition
-- Approved February 1998



create view revenue (supplier_no, total_revenue)
	select
		l_suppkey,
		sum(l_extendedprice * (1 - l_discount))
	from
		lineitem
	where
		l_shipdate >= '1996-01-10'
		and l_shipdate < '1996-03-10'
	group by
		l_suppkey;


select
	s_suppkey,
	s_name,
	s_address,
	s_phone,
	total_revenue
from
	supplier,
	revenue
where
	s_suppkey = supplier_no
	and total_revenue = (
		select
			max(total_revenue)
		from
			revenue
	)
order by
	s_suppkey;




	db.lineitem.aggregate(
	    [{
	        "$match": {
	            "shipdate": {
	                "$lt": "1996-03-10",
	                "$gte": '1996-01-10'
	            }
	        }
	    }, {
	        "$project": {
	            "suppkey": 1,
	            "quantity": 1,
	            "partsupp.supplier.name": 1,
	            "partsupp.supplier.address": 1,
	            "partsupp.supplier.phone": 1,
	            "extendedprice": 1,
	            "discount": 1,
	            "l_dis_min_1": {
	                "$subtract": [
	                    1,
	                    "$discount"
	                ]
	            }
	        }
	    }, {
	        "$group": {
	            "_id": {
	                "supplier_no": "$suppkey",
	                "s_name": "$partsupp.supplier.name",
	                "s_address": "$partsupp.supplier.address",
	                "s_phone": "$partsupp.supplier.phone"

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
	    }, {
	        "$group": {
	            "_id": {},
	            "total_revenue": {
	                $max: "$revenue"
	            },
	            "ROOT": {
	                "$push": "$$ROOT"
	            }
	        }
	    }, {
	        "$unwind": "$ROOT"
	    }, {
	        "$project": {
	            "total_revenue": 1,
	            "ROOT._id.supplier_no": 1,
	            "ROOT.revenue": 1,
	            "ROOT._id.s_name": 1,
	            "ROOT._id.s_address": 1,
	            "ROOT._id.s_phone": 1,
	            "eq": {
	                "$cond": [{
	                        "$eq": ["$ROOT.revenue", "$total_revenue"]
	                    },
	                    1,
	                    0
	                ]
	            }
	        }
	    }, {
	        "$match": {
	            "eq": {
	                "$eq": 1
	            }
	        }
	    }, {
	        "$project": {
	            "ROOT._id.supplier_no": 1,
	            "ROOT.revenue": 1,
	            "ROOT._id.s_name": 1,
	            "ROOT._id.s_address": 1,
	            "ROOT._id.s_phone": 1
	        }
	    }
	]).pretty()
