db = connect("localhost:27017/TPCH");

var cntrycode1 = "11";
var cntrycode2 = "12";
var cntrycode3 = "13";
var cntrycode4 = "14";
var cntrycode5 = "15";
var cntrycode6 = "16";
var cntrycode7 = "17";



avg_acctbal = db.customer.aggregate( [
    {
        $project: {
            cntrycode: { $substr: [ "$phone", 0, 2] },
            acctbal: 1,
        }
    },
    {
        $match: {
            acctbal: { $gt: 0 },
            cntrycode: { $in: [cntrycode1, cntrycode2, cntrycode3, cntrycode4, cntrycode5, cntrycode6, cntrycode7] }
        }
    },
    {
        $group: {
            _id: null,
            avg_acctbal: { $avg: "$acctbal" }
        }
    },
    {
        $project: {
            _id: 0,
            avg_acctbal: 1,
        }
    },
] ).toArray();

cursor = db.customer.aggregate( [
    {
        $project: {
            cntrycode: { $substr: [ "$phone", 0, 2] },
            acctbal: 1,
            orders: 1
        }
    },
    {
        $match: {
            cntrycode: { $in: [cntrycode1, cntrycode2, cntrycode3, cntrycode4, cntrycode5, cntrycode6, cntrycode7] },
            acctbal: { $gt: avg_acctbal[0].avg_acctbal },
            orders: { $exists: false }
        }
    },
    {
        $group: {
            _id: "$cntrycode",
            numcust: { $sum: 1 },
            totalacctbal: { $sum: "$acctbal" }
        }
    },
    {
        $sort: {
            _id: 1
        }
    }
] ).pretty();

cursor.forEach(printjson);
