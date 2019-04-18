# Spark RDD API to Dataframe API program transformation

## Prerequisites

1. flex
2. bison
3. glib - for hash table (libglib2.0-dev)
4. build-essential pkg-config

```shell
sudo apt-get install libglib2.0-dev
```

## Usage

```shell
./run.sh
```

If no permission `chmod +x run.sh`

## Notes

Suppressing conflict Warnings `%expect n`, n = 8 for my CFG

`#define YYERROR_VERBOSE`

## Choices

Currently 3 approaches:

1. use bison to build AST, transform by myself (this project)
2. use tools like Stratego/XT or TXL to do transformation
3. ANTLR

TXL is a tool that combined scanner, parser, AST construction and transfromation
ANTLR looks friendlier to use, but uses top down LL parser to generate AST.

For now, I pick flex & bison as my tools

## TODO

- [ ] self-implemented hash table (get rid of glib)

## Test Programs

```scala
\\ testFile1
sc.range(10,50)
  .map(i=>{val j=i%3;(i, if(j==0)i*10 else i*2)})
  .map(r=>r._1+r._2)
  .collect()

\\ testFile1_gen
spark.range(10,50).selectExpr("id as _1")
  .selectExpr("_1 as _1","if((_1%3)==0,_1*10,_1*2) as _2")
  .selectExpr("_1+_2 as _1")
  .collect()

\\ result
Array[org.apache.spark.sql.Row] = Array([30], [33], [132], [39], [42], [165], [48], [51], [198], [57], [60], [231], [66], [69], [264], [75], [78], [297], [84], [87], [330], [93], [96], [363], [102], [105], [396], [111], [114], [429], [120], [123], [462], [129], [132], [495], [138], [141], [528], [147])

\\ testFile2
sc.range(10,50)
  .map(i=>(i,i*10,i+i))
  .map(i=>i._1+i._2+i._3)
  .collect()

\\ testFile2_gen
spark.range(10,50).selectExpr("id as _1")
  .selectExpr("_1 as _1","_1*10 as _2","_1+_1 as _3")
  .selectExpr("_1+_2+_3 as _1")
  .collect()

\\ result
Array[org.apache.spark.sql.Row] = Array([130], [143], [156], [169], [182], [195], [208], [221], [234], [247], [260], [273], [286], [299], [312], [325], [338], [351], [364], [377], [390], [403], [416], [429], [442], [455], [468], [481], [494], [507], [520], [533], [546], [559], [572], [585], [598], [611], [624], [637])

\\ testFile3
sc.range(10,50)
  .map(i=>{val j=i*i-3;(j,i*10,i+i,if(j>0)j-1 else j+1)})
  .map(i=>i._1*i._2-i._3%i._4)
  .collect()

\\ testFile3_gen
spark.range(10,50).selectExpr("id as _1")
  .selectExpr("(_1*_1-3) as _1","_1*10 as _2","_1+_1 as _3","if((_1*_1-3)>0,(_1*_1-3)-1,(_1*_1-3)+1) as _4")
  .selectExpr("_1*_2-_3%_4 as _1")
  .collect()

\\ result
Array[org.apache.spark.sql.Row] = Array([9680], [12958], [16896], [21554], [26992], [33270], [40448], [48586], [57744], [67982], [79360], [91938], [105776], [120934], [137472], [155450], [174928], [195966], [218624], [242962], [269040], [296918], [326656], [358314], [391952], [427630], [465408], [505346], [547504], [591942], [638720], [687898], [739536], [793694], [850432], [909810], [971888], [1036726], [1104384], [1174922])

\\ testFile4
sc.range(10,50)
  .map(i=>{val j=i*i-3;val k=j*i;(j,k)})
  .collect()

\\ testFile4_gen
spark.range(10,50).selectExpr("id as _1")
  .selectExpr("(_1*_1-3) as _1","((_1*_1-3)*_1) as _2")
  .collect()

\\ result
Array[org.apache.spark.sql.Row] = Array([97,970], [118,1298], [141,1692], [166,2158], [193,2702], [222,3330], [253,4048], [286,4862], [321,5778], [358,6802], [397,7940], [438,9198], [481,10582], [526,12098], [573,13752], [622,15550], [673,17498], [726,19602], [781,21868], [838,24302], [897,26910], [958,29698], [1021,32672], [1086,35838], [1153,39202], [1222,42770], [1293,46548], [1366,50542], [1441,54758], [1518,59202], [1597,63880], [1678,68798], [1761,73962], [1846,79378], [1933,85052], [2022,90990], [2113,97198], [2206,103682], [2301,110448], [2398,117502])
```

## Implementation

The [transform.l](transform.l) is a Flex file that describe all the tokens for the transformer. It then generate a lexer that map the input program.

The [transform.y](transform.y) is a Bison file that specified the CFG and associated rules that can help build AST.

The [transfor.h](transform.h) is a header file specified what APIs and types I defined for AST and tree walk functions.

The [transform_func.c](transform_func.c) is the file that all of the AST functions tree-walk functions and transform functions belongs to. The basic idea is to build an AST using ad-hoc SDT. Multiple types of node has been used to generate the AST as IR. After the AST has been generated, there is the tree-walk function that walk through the tree and generate the desired SQL API code. In this part, some rules have been made to do the transformation. For example, I used a hash table to do the symbol table look up that, I can deal with the assignment function and also varible scopes. The idea behind the transformation rules of UDF is that the typical UDF of a map function is the program trying to pass each entry a function that either does some calculation on the original data or change each entry to a tuple, in other words, one can see this as adding a column to the dataframe. So the dataframe described in SQL is seperated by comma, and also every element in a transformed SQL has a field like '\_1' and '\_2'.

## References

[bison manual](https://www.gnu.org/software/bison/manual/bison.html)

[how to use literal string tokens in bison](https://stackoverflow.com/questions/43095501/how-to-use-literal-string-tokens-in-bison)

[yacc for c](http://www.lysator.liu.se/c/ANSI-C-grammar-y.html)

[directly returning terminal characters](http://aquamentus.com/flex_bison.html)

[O'REILLY flex & bison](https://www.oreilly.com/library/view/flex-bison/9780596805418/ch01.html)

[TXL](https://www.txl.ca/)

[ANTLR](https://www.antlr.org)

[bison AST example](https://efxa.org/2014/05/25/how-to-create-an-abstract-syntax-tree-while-parsing-an-input-stream/)
