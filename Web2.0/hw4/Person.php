<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of Person
 *
 * @author Kevin
 */
class Person {
    //put your code here
    var $name;
    var $gender;
    var $age;
    var $personality;
    var $favoriteos;
    var $seeking_female;
    var $seeking_male;
    var $floor_age;
    var $ceil_age;
    var $rating;
    function __construct($config){
        if(!isset($config)){
            return;
        }
        $this->name=$config["usr_name"];
        $this->gender=$config["gender"];
        $this->age=$config['age'];
        $this->personality=$config['personality'];
        $this->favoriteos=$config['favoriteos'];
        if(isset($config['seeking_female']))
            $this->seeking_female = $config['seeking_female'];
        if(isset($config['seeking_male']))
            $this->seeking_male=$config['seeking_male'];
        $this->floor_age=$config['floor_age'];
        $this->ceil_age=$config['ceil_age'];
    }
    
    function info(){
        return "".$this->name.",".$this->gender.",".$this->age.",".$this->personality.",".$this->favoriteos.",".$this->seeking_female.$this->seeking_male.",".$this->floor_age.",".$this->ceil_age;
    }
    function setInfo($config){
        $this->name=$config[0];
        $this->gender=$config[1];
        $this->age=$config[2];
        $this->personality=$config[3];
        $this->favoriteos=$config[4];
        if(isset($config[5])){
            if(strlen($config[5]) > 1){
                $this->seeking_male= substr($config[5], 0,1);
                $this->seeking_female = substr($config[5], 1,1);
            }else if($config[5] === "F"){
                $this->seeking_female = $config[5];
                 $this->seeking_male = "";
            }else if($config[5] === "M"){
                 $this->seeking_female ="";
                 $this->seeking_male = $config[5];
            }
        }
        $this->floor_age=$config[6];
        $this->ceil_age=$config[7];
    }
    
}

?>
