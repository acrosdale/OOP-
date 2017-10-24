-- $ID$
-- TPC-H/TPC-R Parts/Supplier Relationship Query (Q16)
-- Functional Query Definition
-- Approved February 1998
:x
:o
select
	p_brand,
	p_type,
	p_size,
	count(distinct ps_suppkey) as supplier_cnt
from
	partsupp,
	part
where
	p_partkey = ps_partkey
	and p_brand <> 'Brand#45' -- <> means not equal to--
	and p_type not like 'MEDIUM POLISHED%'--
	and p_size in (49, 14, 23, 45, 19, 3, 36, 9)--
	and ps_suppkey not in (
		select
			s_suppkey
		from
			supplier
		where
			s_comment like '%Customer%Complaints%'
	)
group by
	p_brand,
	p_type,
	p_size
order by
	supplier_cnt desc,
	p_brand,
	p_type,
	p_size;


	var supp_key = [];

    db.partsupp.aggregate(
        [{
            "$match": {

                "supplier.comment": {
                    "$regex": 'Customer Complaints',
                    "$options": 'i'
                }
            }
        }, {
            "$project": {
                "supplier.suppkey": 1
            }
        }]).forEach(function(u) {
        	supp_key.push(u.suppkey)
    	});


    db.partsupp.aggregate(
        [{
            "$project": {
                "part.brand": 1,
                "suppkey": 1,
                "part.type": 1,
                "part.size": 1,
                "brand_check": {
                    "$ne": ["$part.brand", "Brand#45"]
                }

            }
        }, {
            "$match": {
                "brand_check": true,
                "part.type": {
                    "$regex": '^(?!MEDIUM POLISHED)',
                    "$options": 'i'
                },
                "part.size": {
                    "$in": [49, 14, 23, 45, 19, 3, 36, 9]
                },
                "suppkey": {
                    "$nin": supp_key
                }

            }
        }, {
            "$group": {
                "_id": {
                    "p_type": "$part.type",
                    "p_brand": "$part.brand",
                    "p_size": "$part.size"
                },
                "supplier_cnt_distinct": {
                    "$addToSet": "$suppkey"
                }
            }
        }, {
            "$unwind": "$supplier_cnt_distinct"
        }, {
            "$group": {
                "_id": {
                    "_id": "$_id"
                },
                "supplier_cnt": {
                    "$sum": 1
                }
            }
        }, {
            "$sort": {
                "part.brand": 1,
                "part.type": 1,
                "part.size": 1,
                "supplier_cnt": -1
            }
        }]).pretty()
