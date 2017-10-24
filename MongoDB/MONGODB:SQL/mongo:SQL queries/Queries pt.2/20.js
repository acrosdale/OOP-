db = connect("localhost:27017/TPCH");

var name_prefix = "";
var date_year_interval_begin = new Date("1994-01-01");
var nation_name = "UNITED STATES";

l_availqty = db.lineitem.aggregate( [
    {
        $match: {
            shipdate: { $gte: "1994-01-01", $lt: "1995-01-01" }
        }
    },
    {
        $group: {
            _id: null,
            sum_quantity: {$sum: "$quantity" }
        }
    },
    {
        $project: {
            _id: 0,
            sum_quantity: { $multiply: [0.1, "$sum_quantity" ] }
        }
    }
] ).toArray();

cursor = db.partsupp.aggregate( [
    {
        $project: {
            p_name: "$part.name",
            n_name: "$supplier.nation.name",
            s_name: "$supplier.name",
            s_address: "$supplier.address",
            availqty: 1
        }
    },
    {
        $match: {
            p_name: /^/,
            n_name: nation_name,
            availqty: { $gt: l_availqty[0].sum_quantity }
        }
    },
    {
        $group: {
            _id: {
                S_NAME: "$s_name",
                S_ADDRESS: "$s_address"
            }
        }
    },
    {
        $sort: {
            s_name: 1
        }
    }
] ).pretty();

cursor.forEach(printjson);
