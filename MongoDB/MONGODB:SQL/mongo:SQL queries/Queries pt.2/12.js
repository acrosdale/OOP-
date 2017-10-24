var ship_mode_1 = "MAIL";
var ship_mode_2 = "SHIP";
var receipt_date_range_start = "1994-01-01";
var receipt_date_range_end = "1995-01-01";

db = connect("localhost:27017/TPCH");

cursor = db.lineitem.aggregate( [
    {
        $match: {
            shipmode: {
                $in: [
                    ship_mode_1, 
                    ship_mode_2
                ]
            },
            commitdate: {
                $gt: "$receiptdate"
            },
            shipdate: {
                $gt: "$commitdate"
            },
            receiptdate: {
                $gte: receipt_date_range_start
            },
            receiptdate: {
                $lt: receipt_date_range_end
            }
        }
    },
    {
        $project: {
            shipmode: 1,
            high_line: {
                $cond: [ 
                    {
                        $setIsSubset: [
                            ["$order.orderpriority"],
                            ["1-URGENT", "2-HIGH"]
                        ]
                    },
                    1,
                    0
                ]
            },   
            low_line: {
                $cond: [ 
                    {
                        $setIsSubset: [
                            ["$order.orderpriority"],
                            ["1-URGENT", "2-HIGH"]
                        ]
                    },
                    0,
                    1
                ]
            },
            commitdate: 1,
            receiptdate: 1,
            shipdate: 1,
        }
    }, 
    {
        $group: {
            _id: {
                shipmode: "$shipmode"
            },
            high_line_count: {
                $sum: "$high_line"
            },
            low_line_count: {
                $sum: "$low_line"
            }
        }
    }, 
	{
        $project: {
            shipmode: 1,
            high_line_count: 1,
            low_line_count: 1
        }
    }, 
    {
        $sort: {
            shipmode: 1
        }
    }
] ).pretty();

cursor.forEach(printjson);
