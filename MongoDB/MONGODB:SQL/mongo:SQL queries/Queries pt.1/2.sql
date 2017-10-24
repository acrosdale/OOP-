-- $ID$
-- TPC-H/TPC-R Minimum Cost Supplier Query (Q2)
-- Functional Query Definition
-- Approved February 1998
:x
:o
select
	s_acctbal,
	s_name,
	n_name,
	p_partkey,
	p_mfgr,
	s_address,
	s_phone,
	s_comment

from
	part,supplier,partsupp,nation,region
where
	p_partkey = ps_partkey
	and s_suppkey = ps_suppkey
	and p_size = 17
	and p_type like '%BRASS'
	and s_nationkey = n_nationkey
	and n_regionkey = r_regionkey
	and r_name = 'EUROPE'
	and ps_supplycost = (
		select
			min(ps_supplycost)
		from
			partsupp,supplier,nation,region
		where
			p_partkey = ps_partkey
			and s_suppkey = ps_suppkey
			and s_nationkey = n_nationkey
			and n_regionkey = r_regionkey
			and r_name = 'EUROPE'
	)
order by
	s_acctbal desc,
	n_name,
	s_name,
	p_partkey;



db.lineitem.aggregate(
	    [{
			"$match": {
				"partsupp.part.size": 17,
				"partsupp.part.type": { "$regex": 'BRASS', "$options": 'g' },
				"partsupp.supplier.nation.region.name": "EUROPE"

			}
	    },
		 {
	        "$project": {
				"partsupp.supplier.acctbal": 1,
				"partsupp.supplier.name":1,
				"partsupp.supplier.nation.name":1,
				"partsupp.part.partkey":1,
				"partsupp.part.mfgr":1,
				"partsupp.supplier.address":1,
				"partsupp.supplier.phone":1,
				"partsupp.supplier.comment":1,
				"partsupp.supplycost" : 1,

	        }
	    }, {
	        "$group": {
	            "_id": {
				"acctbal"   : "$partsupp.supplier.acctbal",
				"s_name"    : "$partsupp.supplier.name",
				"n_name"    : "$partsupp.supplier.nation.name",
				"p_partkey" : "$partsupp.part.partkey",
				"p_mfgr"    : "$partsupp.part.mfgr",
				"s_address" : "$partsupp.supplier.address",
				"s_phone"   : "$partsupp.supplier.phone",
				"s_comment" : "$partsupp.supplier.comment"
	            }
	        }
	    }, {
	        "$sort": {
				"partsupp.supplier.acctbal" : -1,
				"partsupp.supplier.nation.name": 1,
				"partsupp.supplier.name":1,
				"partsupp.part.partkey":1
	        }
	    }]
	).pretty()






//min_supp_cost: { "$min": "$partsupp.supplycost"}


	function Start() {
		var start = new Date().getTime();

		db.lineitem.aggregate(
			    [{
					"$match": {
						"partsupp.part.size": 17,
						"partsupp.part.type": { "$regex": 'BRASS', "$options": 'g' },
						"partsupp.supplier.nation.region.name": "EUROPE"

					}
			    },
				 {
			        "$project": {
						"partsupp.supplier.acctbal": 1,
						"partsupp.supplier.name":1,
						"partsupp.supplier.nation.name":1,
						"partsupp.part.partkey":1,
						"partsupp.part.mfgr":1,
						"partsupp.supplier.address":1,
						"partsupp.supplier.phone":1,
						"partsupp.supplier.comment":1,
						"partsupp.supplycost" : 1

			        }
			    }, {
			        "$group": {
			            "_id": {
						"acctbal"   : "$partsupp.supplier.acctbal",
						"s_name"    : "$partsupp.supplier.name",
						"n_name"    : "$partsupp.supplier.nation.name",
						"p_partkey" : "$partsupp.part.partkey",
						"p_mfgr"    : "$partsupp.part.mfgr",
						"s_address" : "$partsupp.supplier.address",
						"s_phone"   : "$partsupp.supplier.phone",
						"s_comment" : "$partsupp.supplier.comment"
			            }
			        }
			    }, {
			        "$sort": {
						"partsupp.supplier.acctbal" : -1,
						"partsupp.supplier.nation.name": 1,
						"partsupp.supplier.name":1,
						"partsupp.part.partkey":1
			        }
			    }]
			);
		var stop = new Date().getTime();

		var total_time = stop - start;
		printjson(total_time);
	}
