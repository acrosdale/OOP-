-- $ID$
-- TPC-H/TPC-R Large Volume Customer Query (Q18)
-- Function Query Definition
-- Approved February 1998
:x
:o
select
	c_name,
	c_custkey,
	o_orderkey,
	o_orderdate,
	o_totalprice,
	sum(l_quantity)
from
	customer,
	orders,
	lineitem
where
	o_orderkey in (
		select
			l_orderkey
		from
			lineitem
		group by
			l_orderkey having
				sum(l_quantity) > 200
	)
	and c_custkey = o_custkey
	and o_orderkey = l_orderkey
group by
	c_name,
	c_custkey,
	o_orderkey,
	o_orderdate,
	o_totalprice
order by
	o_totalprice desc,
	o_orderdate;
:n 100


var l_orderkey = [];

db.lineitem.aggregate(
    [
	 {
        "$project": {
			"orderkey": 1,
			"quantity":1
        }
    },{
        "$group": {
            "_id": {
				"l_orderkey": "$orderkey"
            },
			"quantity":{
				"$sum": "$quantity"
			}
        }
    },{
	   "$project": {

	   	"orderkey": "$_id.l_orderkey",
	   	"quantity": "$quantity",
	   	"quantity_check":{"$gt": [ "$quantity",200]}}

   },{
      "$match": {

	   "quantity_check": true

      }
   },{
	   "$group": {
		   "_id": {
			   "l_orderkey": "$orderkey"
		   }
	   }
   }
]).forEach(function(u) {
	l_orderkey.push(u._id.l_orderkey)
});

db.lineitem.aggregate(
    [{
        "$match": {
			"orderkey": {
				"$in": l_orderkey
			}
        }
    },
	 {
        "$project": {

			"order.customer.name":1,
			"order.customer.custkey":1,
			"order.orderkey":1,
			"order.orderdate":1,
			"order.totalprice":1,
			"quantity":1,
        }
    },{
        "$group": {
            "_id": {
			"c_name"	  :"$order.customer.name",
			"c_custkey"	  :"$customer.custkey",
			"o_orderkey"  :"$order.orderkey",
			"o_orderdate" :"$order.orderdate",
			"o_totalprice":"$order.totalprice",
            },
			"l_quantity":{
				"$sum": "$quantity"
			}
        }
    },{
        "$sort": {
            "order.totalprice":-1,
            "order.orderdate":1,
        }
    }]
).pretty()
