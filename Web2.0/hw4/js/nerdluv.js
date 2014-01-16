/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

window.onload=function(){
    var seeking_gender = document.getElementById("seeking").getElementsByTagName("input");
       for(var input=0;input<seeking_gender.length; input++){
        seeking_gender[input].parentNode.nextSibling.onclick=click_label;
        }
function click_label(evt){
    var check_box = this.previousSibling.getElementsByTagName("input")[0];
    if(check_box.checked === true){
        check_box.checked = false;
    }else{
        check_box.checked = true;
    }
}
var sign = document.getElementById("signup");
    sign.onclick=function(){
        var form = document.getElementById("sign_up_form");
        form.submit();
    };
};



