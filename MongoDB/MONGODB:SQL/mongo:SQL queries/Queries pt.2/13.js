db = connect("localhost:27017/TPCH");

var comment_1 = "special";
var comment_2 = "requests";

db.orders.aggregate( [
    {
        $match: {
            comment: {
                $not: /comment_1.*comment_2/
            }
        }
    },
    {
        $out: "matched_orders"
    }
] );

cursor = db.customer.aggregate( [
    {
        $lookup: {
            from: "matched_orders",
            localField: "custkey",
            foreignField: "custkey",
            as: "customer_orders"
        }
    },
    {
        $project: {
            _id: 0,
            c_count: { $size: "$customer_orders" },
            custdist: 1,
            custkey: 1
        }
    },
    {
        $group: {
            _id: "$c_count",
            custdist: { $sum: 1 }
        }
    },
    {
        $sort: {
            custdist: -1,
            c_count: -1
        }
    }
] ).pretty();

cursor.forEach(printjson);
