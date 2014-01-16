/* /
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
//十五个pieces
var pieces = new Array();

//由位置下标找所放的元素
var pos2pie = [];
//由所放的元素找位置在哪
var pie2pos = [];
var avai = 15;
window.addEventListener("load",onLoad);


function onLoad(){
    initData();
    initOnclick();
    shuffle();
}
var initOnclick = function(){
    document.getElementById("shufflebutton").addEventListener('click',shuffle);;
};
var shuffle = function(){
    function randomsort(a, b) {
        return Math.random()>.5 ? -1 : 1;//用Math.random()函数生成0~1之间的随机数与0.5比较，返回-1或1
    }
    var tobesort = pie2pos.slice(1);
    tobesort.sort(randomsort);
    for(var j = 0; j < tobesort.length; j++){
        pie2pos[j+1] = tobesort[j];
    }
    for(var i =1; i < pie2pos.length; i++){
        pos2pie[pie2pos[i]] = i;
    }
    format();
    updateMovable(avai);
};
var format = function(){
    pieces = document.getElementById("puzzlearea")
                .getElementsByTagName("div");
    var row = 0;
    var column = 0;
    for(index  in pieces){
        if(index === 'length'){
            return;
        }
        var pos = getRowCol(pie2pos[(parseInt(index)+1)]);
        var bgpos =  getRowCol(index);
        pieces[index].style.backgroundPosition = "-"+(bgpos["col"]*100)+"px -"+(bgpos['row']*100)+"px";
        pieces[index].style.left = (pos['col']*100)+"px";
        pieces[index].style.top = (pos['row']*100)+"px";
        pieces[index].addEventListener("click",onPieceClick);
    }
   
};
var onPieceClick = function(){
    process(this.textContent);
};
var process = function(curId){
    var avaPos = getRowCol(avai);
    var curPos = getRowCol(pie2pos[curId]);
    if(Math.abs(avaPos['col']-curPos['col']) + 
        Math.abs(avaPos['row']-curPos['row']) > 1){
            return;//不可移动
        }
    updateRemove(avai);
    pos2pie[pie2pos[curId]] = -1;
    pos2pie[avai] = parseInt(curId);
    var tmp = pie2pos[curId];
    pie2pos[curId] = avai;
    avai = tmp;
    pieces[curId-1].style.left = (avaPos['col']*100)+"px";
    pieces[curId-1].style.top = (avaPos['row']*100)+"px";
    updateMovable(avai);
    if(checkwin()){
        alert("You Win~");
        shuffle();
    }
    
};
var initData = function(){
    for(var i = 0; i < 15; i++){
        pos2pie[i] = i+1;
        pie2pos[i+1] = i;
    }
    pos2pie[15] = -1;
    avai = 15;
};
//下标从0开始
//输入位置从0开始
var getRowCol = function(curId){
    var tmpRow;
    var tmpCol;
    var result = [];
    if(Math.ceil(curId/4) !== Math.floor(curId/4))
        tmpRow = Math.ceil(curId/4)-1;
    else{
        tmpRow = Math.ceil(curId/4);
    }
    if(tmpRow < 0){tmpRow = 0;}
        tmpCol = curId-tmpRow*4;
        
    result["row"] = tmpRow;
    result["col"] = tmpCol;
    if(tmpCol === 4){
        alert("4"+curId+" "+tmpRow);
    }
    return result;
};

var updateMovable = function(avaiId){
    var pos =  getRowCol(avaiId);
    if(pos['col'] !== 3){
        addClass(pieces[pos2pie[(pos['row']*4+pos['col']+1)]-1],"movablepiece");
    }
    if(pos['col']!== 0){
        addClass(pieces[pos2pie[(pos['row']*4+pos['col']-1)]-1],"movablepiece");
    }
    if(pos['row'] !==0){
        addClass(pieces[pos2pie[((pos['row']-1)*4+pos['col'])]-1],"movablepiece");
    }
    if(pos['row'] !== 3){
        addClass(pieces[pos2pie[((pos['row']+1)*4+pos['col'])]-1],"movablepiece");
    }
};
var updateRemove = function(avaiId){
   var pos =  getRowCol(avaiId);
    if(pos['col'] !== 3){
        removeClass(pieces[pos2pie[(pos['row']*4+pos['col']+1)]-1]);
    }
    if(pos['col']!== 0){
        
        removeClass(pieces[pos2pie[(pos['row']*4+pos['col']-1)]-1]);
    }
    if(pos['row'] !==0){
        removeClass(pieces[pos2pie[((pos['row']-1)*4+pos['col'])]-1]);
    }
    if(pos['row'] !== 3){
        removeClass(pieces[pos2pie[((pos['row']+1)*4+pos['col'])]-1]);
    }
};
var addClass = function(ele,classname){
    var ori = ele.className;
    ori +=(" "+classname);
    ele.className = ori;
};

var removeClass = function(ele){
    ele.className = 'puzzlepiece';
};

var checkwin = function(){
    var flag = true;
    for(var i = 0; i < 15; i++){
        if(pos2pie[i] !== i+1){
            flag = false;
            break;
        }
    }
    return flag;
};