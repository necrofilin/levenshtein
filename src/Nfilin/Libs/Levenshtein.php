<?php
namespace Nfilin\Libs;

/**
 * Class Levenshtein
 *
 * @property double $distance
 * @property string[] $path
 * @property string[] $blocks
 *
 * @package Nfilin\Libs
 */
class Levenshtein
{
    /**
     * @var null|string
     */
    public $string = null;
    /**
     * @var null|string[]
     *
     * Symbol guide:
     *    - %- - dash or space,
     *    - %o - one or more digits,
     *    - %d - one or more digits, without leading zeroes,
     *    - %O - exactly one digit, filled with 0 if not present,
     *    - %D - exactly one digit, empty if not present,
     *    - %N - one digit or nothing,
     *    - %L - one english letter or nothing,
     *    - %s - one or more of any symbols,
     *    - %S - exactly one symbol,
     *    - %Z - one symbol or nothing,
     *    - %% - percent symbol,
     *    - any other symbol as him self
     *
     */
    public $pattern = null;
    /**
     * @var Costs
     */
    public $costs;

    /**
     * @var Storage
     */
    private $storage;

    /**
     * Levenshtein constructor.
     * @param null|string $pattern
     * @param null|string $string
     * @param int $insert_cost
     * @param int $replace_cost
     * @param int $delete_cost
     */
    public function __construct($pattern = null, $string = null, $insert_cost = 1, $replace_cost = 1, $delete_cost = 1)
    {

    }

    /**
     * @param string $string
     * @return $this
     */
    public function setString($string)
    {
        $this->string = $string;
        return $this;
    }

    /**
     * @param string $pattern
     * @return $this
     */
    public function setPattern($pattern)
    {
        $this->pattern = $pattern;
        return $this;
    }
}
