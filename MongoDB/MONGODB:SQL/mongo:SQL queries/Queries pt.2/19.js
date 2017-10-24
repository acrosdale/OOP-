db = connect("localhost:27017/TPCH");

var brand1 = "Brand#";
var brand2 = "Brand#";
var brand3 = "Brand#";
var quantity1 = 0;
var quantity2 = 0;
var quantity3 = 0;

cursor = db.lineitem.aggregate( [
    {
        $project: {
            brand: "$partsupp.part.brand",
            container: "$partsupp.part.container",
            quantity: 1,
            size: "$partsupp.part.size",
            shipmode: 1,
            shipinstruct: 1,
            extendedprice: 1,
            discount: 1
        }
    },
    {
        $match: {
            $or: [
                { $and: [
                    {brand: /Brand#.*/ },
                    {container: { $in: ["SM CASE", "SM BOX", "SM PACK", "SM PKG"] } },
                    {quantity: {$gte: 0, $lte: 100} },
                    {size: {$gt: 1, $lt: 5} },
                    {shipmode: {$in: ["AIR", "AIR REG"] } },
                    {shipinstruct: "DELIVER IN PERSON"}
                ] },
                { $and: [
                    {brand: /Brand#.*/ },
                    {container: { $in: ["MED BAG", "MED BOX", "MED PKG", "MED PACK"] } },
                    {quantity: {$gte: 0, $lte: 100} },
                    {size: {$gt: 1, $lt: 10} },
                    {shipmode: {$in: ["AIR", "AIR REG"] } },
                    {shipinstruct: "DELIVER IN PERSON"}
                ] },
                { $and: [
                    {brand: /Brand#.*/},
                    {container: { $in: ["LG CASE", "LG BOX", "LG PACK", "LG PKG"] } },
                    {quantity: {$gte: 0, $lte: 100} },
                    {size: {$gt: 1, $lt: 15} },
                    {shipmode: {$in: ["AIR", "AIR REG"] } },
                    {shipinstruct: "DELIVER IN PERSON"}
                ] }
            ]
        }
    },
    {
        $project: {
            extendedprice: 1,
            price_modifier: {$subtract: [1, "$discount"] },
        }
    },
    {
        $project: {
            revenue: {$multiply: ["$extendedprice", "$price_modifier"] }
        }
    },
    {
        $group: {
            _id: null,
            sum_revenue: {$sum: "$revenue"}
        }
    }
] ).pretty();

cursor.forEach(printjson);
