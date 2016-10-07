--TEST--
Check for Levenshtein parse
--FILE--
<?php 
$obj = new \Nfilin\Libs\Levenshtein('asdsad','asdasdqwe');
echo "--------------DUMP----------\n";
echo "--------------STORAGE----------\n";
var_dump($obj->getStorage());
echo "--------------PATTERN----------\n";
var_dump($obj->pattern);
echo "--------------OBJ----------\n";
var_dump($obj);
echo "--------------DIST----------\n";
var_dump($obj->distance);
echo "--------------PATT----------\n";
var_dump($obj->pattern);
echo "--------------STR----------\n";
var_dump($obj->string);
echo "--------------PATH----------\n";
var_dump($obj->path);
echo "--------------BLOCKS----------\n";
var_dump($obj->blocks);
echo "--------------DUMPED----------\n";
echo "--------------SET P xxxxxx----------\n";
var_dump($obj->pattern);
$obj->setPattern("xxxxxx");
var_dump($obj->pattern);
var_dump($obj->distance);
var_dump($obj->path);
echo "--------------s = xxx----------\n";
$obj->string = "xxx";
var_dump($obj->pattern);
var_dump($obj->string);
var_dump($obj->distance);
var_dump($obj->path);
echo "--------------P = xxxxxx----------\n";
var_dump($obj->pattern);
$obj->pattern = "xxxxxx";
var_dump($obj->pattern);
var_dump($obj->distance);
var_dump($obj->path);
if(is_numeric($obj->distance)){
	print 'All ok';
}
/*
	you can add regression tests for your extension here

  the output of your test code has to be equal to the
  text in the --EXPECT-- section below for the tests
  to pass, differences between the output and the
  expected text are interpreted as failure

	see php7/README.TESTING for further information on
  writing regression tests
*/
?>
--EXPECTREGEX--
.*All ok.*