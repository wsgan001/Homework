var custom = "\n" + 
"                      __------__\n" + 
"                    /~          ~\\\n" + 
"                   |    //^\\\\//^\\|\n" + 
"                 /~~\\  ||  o| |o|:~\\\n" + 
"                | |6   ||___|_|_||:| \n" + 
"                 \\__.  /      o  \\/\'\n" + 
"                  |   (       O   )\n" + 
"         /~~~~\\    `\\  \\         /\n" + 
"        | |~~\\ |     )  ~------~`\\\n" + 
"       /\' |  | |   /     ____ /~~~)\\\n" + 
"      (_/\'   | | |     /\'    |    ( |\n" + 
"             | | |     \\    /   __)/ \\\n" + 
"             \\  \\ \\      \\/    /\' \\   `\\\n" + 
"               \\  \\|\\        /   | |\\___|\n" + 
"                 \\ |  \\____/     | |\n" + 
"                 /^~>  \\        _/ <\n" + 
"                |  |         \\       \\\n" + 
"                |  | \\        \\        \\\n" + 
"                -^-\\  \\       |        )\n" + 
"                     `\\_______/^\\______/\n";

ANIMATIONS["Custom"] = custom;
var allFrames;
var currentFrame;
var frameKeys;
var frameSize;
var intervalBundle;
var textArea;
var timeInterval;
var isPlaying;
window.addEventListener("load",onLoad);
/*注意要采用function的方式定义handler onLoad，否则在解析初期是找不到的，使用 var 方式
 * 定义的函数是在运行时才赋值的，因此除非onLoad定义在addEventListener 之前，否则是不
 * 可能找得到元素的*/
/*
 * 这里进行必要的初始化工作
 */
function onLoad(){
   //document.getElementById("displayarea").value = juggler;
   initOnclickListener();
   initData();
}
var initOnclickListener = function(){
    var btn_start = document.getElementById("btn-start");
    var btn_stop = document.getElementById("btn-stop");
    var select_animation = document.getElementById("select-animation");
    var size = document.getElementsByName("size");
    var speed = document.getElementById("speed");
    textArea = document.getElementById("displayarea");
    btn_start.addEventListener('click',btn_start_click);
    btn_stop.addEventListener('click',btn_stop_click);
    select_animation.addEventListener('change',animation_change);
    frameSize = select_animation.length;
    for(var i = 0; i < size.length; i++){
        size[i].addEventListener("click",sizeClick);
    }
    speed.addEventListener("click",speedCheck);
};
var initData = function(){
   currentFrame = 0;
   timeInterval = 200;
   allFrames = getAllFrames();
   isPlaying = false;
};
var btn_start_click = function(){
   if(isPlaying) return;
   intervalBundle = setInterval("showNext()",timeInterval);
   isPlaying = true;
};
var btn_stop_click = function(){
    if(!isPlaying) return;
    clearInterval(intervalBundle);
    textArea.value = allFrames;
    isPlaying = false;
};
var animation_change = function(){
    textArea.value = ANIMATIONS[this.value];
};

var sizeClick = function(){
    textArea.className = this.value+"Font";
};
var speedCheck = function(){
    if(this.checked === true){
        if(timeInterval === 50) return;
        
        timeInterval = 50;
        if(!isPlaying) return;
        clearInterval(intervalBundle);
        intervalBundle = setInterval("showNext()",timeInterval);
    }else{
        if(timeInterval === 200) return;
        
        timeInterval = 200;
        if(!isPlaying) return;
        clearInterval(intervalBundle);
        intervalBundle = setInterval("showNext()",timeInterval);
    }
};
var showNext = function(){
    if(currentFrame === frameSize){//播完这一次就得换了
        currentFrame = 0;
    }
    textArea.value = ANIMATIONS[frameKeys[currentFrame]]; 
    currentFrame ++;
};
function getAllFrames(){
    var results = "";
    var i = 0;
    frameKeys = {};
    for(frame in ANIMATIONS){
        results += (ANIMATIONS[frame]+"=====\n");
        frameKeys[i] = frame;
        i++;
    }
    return results;
}

