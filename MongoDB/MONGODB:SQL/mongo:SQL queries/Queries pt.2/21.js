db = connect("localhost:27017/TPCH");

var nation_name = "UNITED STATES";

cursor = db.lineitem.aggregate( [
    {
        $project: {
            orderkey: 1,
            orderstatus: "$order.orderstatus",
            receiptdate: 1,
            commitdate: 1,
            s_name: "$partsupp.supplier.name",
            n_name: "$partsupp.supplier.nation.name"
        }
    },
    {
        $group: {
            _id: "$orderkey",
            orderkey_lineitem_count: { $sum: 1 },
            orderstatus: { $first: "$orderstatus" },
            receiptdate: { $first: "$receiptdate" },
            commitdate: { $first: "$commitdate" },
            n_name: { $first: "$n_name" },
            s_name: { $first: "$s_name" }
        }
    },
    {
        $match: {
            orderstatus: "F",
            receiptdate: { $gt: "$commitdate" },
            n_name: nation_name
        }
    },
    {
        $group: {
            _id: "$s_name",
            numwait: { $sum: 1 }
        }
    }
] ).pretty();

cursor.forEach(printjson);
