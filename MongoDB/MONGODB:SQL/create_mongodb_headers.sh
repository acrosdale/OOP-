#!/bin/bash

sed -i '1s/^/nationkey|name|regionkey|comment\n/' nation.tbl
sed -i '1s/^/regionkey|name|comment\n/' region.tbl
sed -i '1s/^/partkey|name|mfgr|brand|type|size|container|retailprice|comment\n/' part.tbl
sed -i '1s/^/suppkey|name|address|nationkey|phone|acctbal|comment\n/' supplier.tbl
sed -i '1s/^/partkey|suppkey|availqty|supplycost|comment\n/' partsupp.tbl
sed -i '1s/^/custkey|name|address|nationkey|phone|acctbal|mktsegment|comment\n/' customer.tbl
sed -i '1s/^/orderkey|custkey|orderstatus|totalprice|orderdate|orderpriority|clerk|shippriority|comment\n/' orders.tbl
sed -i '1s/^/orderkey|partkey|suppkey|linenumber|quantity|extendedprice|discount|tax|returnflag|linestatus|shipdate|commitdate|receiptdate|shipinstruct|shipmode|comment\n/' lineitem.tbl
