<?php
namespace Nfilin\Libs;


/**
 * Class Storage
 * @package Nfilin\Libs
 */
class Storage
{
    /**
     * @var double
     */
    public $cost_ins = 1;
    /**
     * @var double
     */
    public $cost_rep = 1;
    /**
     * @var double
     */
    public $cost_del = 1;
    /**
     * @var string[]|null
     */
    public $pattern;
    /**
     * @var string[]|null
     */
    public $path;
    /**
     * @var string[]|null
     */
    public $blocks;
    /**
     * @var string[]|null
     */
    public $searches;
    /**
     * @var double[][]|null
     */
    public $lv;
    /**
     * @var int[][]|null
     */
    public $match_map;
    /**
     * @var double
     */
    public $distance = 0;
}