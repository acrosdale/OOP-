
/*
#### this file contains the neccesary code to make a denornamalized model for the TPCH bechmark data in MongoDB####

## data field ###
        customer [0-15000)
        lineitem [0-600572)
        nation  [0-24)
        orders  [150000)
        part    [1-20000]
        partsupp [0-80000)
        region [0-4)
        supplier [1-1000]
*/




//-----------  useful code/ MONGO import/export ------------
/*

mongoimport -d $DATABSE -c $COLLECTION --file  $FILENAME.json


mongoexport -db $DATABSE -c $COLLECTION --out $FILENAME.json

*/

//============================


//-----------  useful code/ MONGO EMBEDDING/Denormalization ------------
// this section contains the necessary code to emebed the required collection
// just copy and paste into mongodb terminal




function denorm(){
    var i=0;
    var temp;
    var temp2;

    db.nation.update({},{$set : {"region":1}},false,true)
    db.supplier.update({},{$set : {"nation":1}},false,true)
    db.customer.update({},{$set : {"nation":1}},false,true);

    printjson("Denormalizing region into nation");

    while (i<5){
        temp2 = db.region.findOne({"regionkey": i });
        db.nation.update({"regionkey": i}, {$set:{"region": temp2}},false,true);
        printjson(i);
        i++;
    }

    printjson("Denormalizing nation into supplier");
    i=0;
    temp=0;
    temp2=0;

    while (i<=24){
        temp= i;
        temp2 = db.nation.findOne({"nationkey": temp });
        db.supplier.update({"nationkey": temp}, {$set:{"nation": temp2}},false,true);
        printjson(i);
        i++;
    }

    i=0;
    temp=0;
    temp2=0;


    while (i<24){
        temp= i;
        temp2 = db.nation.findOne({"nationkey": temp });
        db.customer.update({"nationkey": temp}, {$set:{"nation": temp2}},false,true);
        i++;
        printjson(i);
    }

}


function denorm(){
    var i=0,j=0;
    var temp;
    var temp2;
    var i= db.part.find().size();


    db.partsupp.update({},{$set : {"part":1}},false,true);
    db.partsupp.update({},{$set : {"supllier":1}},false,true);

    printjson("Denormalizing part into parsupp");

    while (j<i){
        temp= j;
        temp2 = db.part.findOne({"partkey": temp });
        db.partsupp.update({"partkey": temp}, {$set:{"part": temp2}},false,true);
        printjson("Part "+ j + " of "+ i);
        j++;
    }

    printjson("Denormalizing supplier into partsupp");
    i=0;
    temp=0;
    temp2=0;
    i=db.supplier.find().size();
    j =1;

    while (j<=i){
        temp= j;
        temp2 = db.supplier.findOne({"suppkey": temp });
        db.partsupp.update({"suppkey": temp}, {$set:{"supplier": temp2}},false,true);
        printjson("supplier " + j + " of "+ i);
        j++;
    }
}




function denorm(){
    var j=0;
    var i=db.customer.find().size();
    var temp=db.customer.find();
    var temp2;

    db.orders.update({},{$set : {"customer":1}},false,true);
    db.lineitem.update({},{$set : {"order":1}},false,true);
    db.lineitem.update({},{$set : {"partsupp":1}},false,true);

    while (j<i){
        temp2=temp[j]
        db.orders.update({"custkey" : temp2.custkey }, {$set:{"customer": temp2}},false,true);
        printjson("Customer " + j + " of "+ i);
        j++;
    }

    i=db.orders.distinct("orderkey").length;
    j =0;
    temp=db.orders.distinct("orderkey");
    temp2;

    while(j<i){
        temp2 = db.orders.find({"orderkey": temp[j]});
        db.lineitem.update({"orderkey": temp[j] }, {$set:{"order": temp2[0]}}, false ,true);
        printjson("order " + j + " of "+ i);
        j++;
    }

    i=db.partsupp.find().size();
    j= 0;
    temp=db.partsupp.find();
    temp2=0;

    while (j<i){
        temp2=temp[j];
        db.lineitem.update({"partkey" : temp2.partkey,"suppkey" : temp2.suppkey}, {$set:{"partsupp": temp2}},false,true);
        printjson("partsupp " + j + " of "+ i);
        j++;

    }

}




//---------------------------------------Unquote the mongodb field----------------------------
    //due to mongodb/json data storage methods certain field are string but any numerial analystic/aggregation cannot be done on strings
    //these quick code parse the necessary string to float numbers



db.customer.find( { 'custkey' : { "$type" : 2 } } ).forEach( function (x) {
  x.custkey = parseFloat(x.custkey); // convert field to string
  db.customer.save(x);
});
db.customer.find( { "nationkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.nationkey = parseFloat(x.nationkey); // convert field to string
  db.customer.save(x);
});
db.customer.find( { "acctbal" : { "$type" : 2 } } ).forEach( function (x) {
  x.acctbal = parseFloat(x.acctbal); // convert field to string
  db.customer.save(x);
});




db.lineitem.find( { "orderkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.orderkey = parseFloat(x.orderkey); // convert field to string
  db.lineitem.save(x);
});
db.lineitem.find( { "suppkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.suppkey = parseFloat(x.suppkey); // convert field to string
  db.lineitem.save(x);
});
db.lineitem.find( { "partkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.partkey = parseFloat(x.partkey); // convert field to string
  db.lineitem.save(x);
});



db.lineitem.find( { "linenumber" : { "$type" : 2 } } ).forEach( function (x) {
  x.linenumber = parseFloat(x.linenumber); // convert field to string
  db.lineitem.save(x);
});
db.lineitem.find( { "quantity" : { "$type" : 2 } } ).forEach( function (x) {
  x.quantity = parseFloat(x.quantity); // convert field to string
  db.lineitem.save(x);
});
db.lineitem.find( { "extendedprice" : { "$type" : 2 } } ).forEach( function (x) {
  x.extendedprice = parseFloat(x.extendedprice); // convert field to string
  db.lineitem.save(x);
});



db.lineitem.find( { "discount" : { "$type" : 2 } } ).forEach( function (x) {
  x.discount= parseFloat(x.discount); // convert field to string
  db.lineitem.save(x);
});
db.lineitem.find( { "tax" : { "$type" : 2 } } ).forEach( function (x) {
  x.tax = parseFloat(x.tax); // convert field to string
  db.lineitem.save(x);
});



db.orders.find( { "orderkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.orderkey = parseFloat(x.orderkey); // convert field to string
  db.orders.save(x);
});
db.orders.find( { "custkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.custkey = parseFloat(x.custkey); // convert field to string
  db.orders.save(x);
});
db.orders.find( { "totalprice" : { "$type" : 2 } } ).forEach( function (x) {
  x.totalprice = parseFloat(x.totalprice); // convert field to string
  db.orders.save(x);
});




db.supplier.find( { 'suppkey' : { "$type" : 2 } } ).forEach( function (x) {
  x.suppkey = parseFloat(x.suppkey); // convert field to string
  db.supplier.save(x);
});
db.supplier.find( { "nationkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.nationkey = parseFloat(x.nationkey); // convert field to string
  db.supplier.save(x);
});
db.supplier.find( { "acctbal" : { "$type" : 2 } } ).forEach( function (x) {
  x.acctbal = parseFloat(x.acctbal); // convert field to string
  db.supplier.save(x);
});






db.partsupp.find( { 'suppkey' : { "$type" : 2 } } ).forEach( function (x) {
  x.suppkey = parseFloat(x.suppkey); // convert field to string
  db.partsupp.save(x);
});
db.partsupp.find( { "partkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.partkey = parseFloat(x.partkey); // convert field to string
  db.partsupp.save(x);
});
db.partsupp.find( { "supplycost" : { "$type" : 2 } } ).forEach( function (x) {
  x.supplycost = parseFloat(x.supplycost); // convert field to string
  db.partsupp.save(x);
});
db.partsupp.find( { "availqty" : { "$type" : 2 } } ).forEach( function (x) {
  x.availqty = parseFloat(x.availqty); // convert field to string
  db.partsupp.save(x);
});





db.part.find( { "partkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.partkey = parseFloat(x.partkey); // convert field to string
  db.part.save(x);
});
db.part.find( { "size" : { "$type" : 2 } } ).forEach( function (x) {
  x.size= parseFloat(x.size); // convert field to string
  db.part.save(x);
});
db.part.find( { "retailprice" : { "$type" : 2 } } ).forEach( function (x) {
  x.retailprice = parseFloat(x.retailprice); // convert field to string
  db.part.save(x);
});





db.region.find( { "regionkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.regionkey = parseFloat(x.regionkey); // convert field to string
  db.region.save(x);
});
db.nation.find( { "regionkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.regionkey = parseFloat(x.regionkey); // convert field to string
  db.nation.save(x);
});
db.nation.find( { "nationkey" : { "$type" : 2 } } ).forEach( function (x) {
  x.nationkey = parseFloat(x.nationkey); // convert field to string
  db.nation.save(x);
});
