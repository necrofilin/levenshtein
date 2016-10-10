--TEST--
Check for Levenshtein class instantiation
--FILE--
<?php 
$obj = new \Nfilin\Libs\Levenshtein();
 if ($obj instanceof \Nfilin\Libs\Levenshtein){ 
	echo "\ninstance of Levenshtein created\n";
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
.*instance of Levenshtein created.*
